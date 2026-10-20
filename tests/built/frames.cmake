# Check a built sample's 600-frame chain against its pin, run as
#   cmake -DCHAIN=<muhtari-test-chain> [-DEMULATOR=<list>] -DROM=<file>
#         -DNAME=<row> -DEXPECTED=<expected.txt> [-DWRITE=ON] -P frames.cmake
# EMULATOR is the build's CMAKE_CROSSCOMPILING_EMULATOR, so an emulated lane
# runs the guest driver.
# Fault prevented: an unintended hash change pinned silently. The pin holds
# only at the behaviour revision the file names, and WRITE never replaces a
# pin unless the core's revision is greater (docs/emulation-tests.md).
#
# The chain is h_0 = 32 zero bytes, h_i = SHA256(h_{i-1} || frame_i) over
# frames 1 to 600 of a fast boot, so frame order is part of the pin.
#
# Without WRITE the check fails unless the file's revision equals the core's
# and column 6 equals the chain. With WRITE=ON:
#   - core revision below the file's: refused;
#   - equal: a "-" column is filled, a pin equal to the chain is kept, any
#     other pin is refused;
#   - greater: this row's column and the revision line are rewritten, and
#     every other row's frame pin is reset to "-" so its own WRITE fills it.
cmake_minimum_required(VERSION 3.25)
foreach(var CHAIN ROM NAME EXPECTED)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "frames: -D${var} is required")
  endif()
endforeach()
set(frames 600)

file(STRINGS "${EXPECTED}" revision_lines REGEX "^# behaviour_revision ")
list(LENGTH revision_lines count)
if(NOT count EQUAL 1 OR NOT revision_lines MATCHES "^# behaviour_revision ([0-9]+)$")
  message(FATAL_ERROR "${NAME}: ${EXPECTED} needs exactly one '# behaviour_revision N' line")
endif()
set(file_revision ${CMAKE_MATCH_1})

file(STRINGS "${EXPECTED}" rows REGEX "^${NAME}[ \t]")
list(LENGTH rows count)
if(NOT count EQUAL 1)
  message(FATAL_ERROR "${NAME}: ${EXPECTED} has ${count} rows for it, not 1")
endif()
set(old_row "${rows}")
string(REGEX REPLACE "[ \t]+" ";" row "${rows}")
list(LENGTH row columns)
if(NOT columns EQUAL 7)
  message(FATAL_ERROR "${NAME}: its row has ${columns} columns, not 7")
endif()
list(GET row 5 want)

execute_process(COMMAND ${EMULATOR} "${CHAIN}" "${ROM}" ${frames}
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "${NAME}: muhtari-test-chain exited ${rc}\n${out}${err}")
endif()
if(NOT out MATCHES "behaviour_revision ([0-9]+)\nchain ([0-9a-f]+)\n")
  message(FATAL_ERROR "${NAME}: unexpected muhtari-test-chain output\n${out}")
endif()
set(core_revision ${CMAKE_MATCH_1})
set(got ${CMAKE_MATCH_2})

if(NOT WRITE)
  if(NOT core_revision EQUAL file_revision)
    message(FATAL_ERROR "${NAME}: ${EXPECTED} pins behaviour revision ${file_revision}, "
      "the core is at behaviour revision ${core_revision}")
  endif()
  if(want STREQUAL "-")
    message(FATAL_ERROR "${NAME}: no frame chain pinned; the core gave ${got}")
  endif()
  if(NOT got STREQUAL want)
    message(FATAL_ERROR "${NAME}: frame chain drift: expected ${want} got ${got}")
  endif()
  message(STATUS "${NAME}: frame chain ${got} as expected")
  return()
endif()

if(core_revision LESS file_revision)
  message(FATAL_ERROR "${NAME}: refusing to write at behaviour revision ${core_revision}, "
    "below the file's ${file_revision}")
endif()
if(core_revision EQUAL file_revision AND NOT want STREQUAL "-")
  if(got STREQUAL want)
    message(STATUS "${NAME}: frame chain ${got} already pinned")
    return()
  endif()
  message(FATAL_ERROR "${NAME}: refusing to rewrite a pin at behaviour revision ${file_revision}")
endif()

file(READ "${EXPECTED}" text)
if(core_revision GREATER file_revision)
  string(REPLACE "# behaviour_revision ${file_revision}\n"
    "# behaviour_revision ${core_revision}\n" text "${text}")
  # Every other row's pin belongs to the old revision.
  string(REGEX REPLACE "(\n[a-z0-9]+[ \t]+[^ \t\n]+[ \t]+[^ \t\n]+[ \t]+[^ \t\n]+[ \t]+[^ \t\n]+[ \t]+)[0-9a-f]+"
    "\\1-" text "${text}")
endif()
list(REMOVE_AT row 5)
list(INSERT row 5 "${got}")
list(JOIN row "  " new_row)
# The row as it now stands (its pin may have just been reset to "-").
string(REGEX MATCH "\n${NAME}[ \t][^\n]*" current "${text}")
string(REPLACE "${current}" "\n${new_row}" text "${text}")
file(WRITE "${EXPECTED}" "${text}")
message(STATUS "${NAME}: frame chain ${got} written at behaviour revision ${core_revision}")
