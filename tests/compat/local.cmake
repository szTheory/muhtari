# The library preset's local pass: judges the ledger's softlist rows (ids
# with no "/") on whichever private files a directory holds, and counts
# every row, so nothing is silently skipped. Run with -P:
#   cmake -DLEDGER=<ledger.csv> -DPREDICATE=<muhtari-test-predicate>
#     -DBIOS=<bios> -DDIR=<directory of files> -DOUT=<local.csv>
#     -DROOT=<source tree> [-DEMULATOR=<wrapper command>] -P local.cmake
# A file is matched to a row only through the core's own identification
# (muhtari-test-predicate --identify), whose printed chip SHA-1 is the row's
# first sha1_raw field. Every present row that is not skipped is judged on the
# fast boot and through the BIOS; a file the core identifies and then refuses
# to load is a fail, since that is the verdict on the file. Links and
# unreadable entries of DIR are counted, never read or named.
# Faults this prevents: a private run that silently skips a row, miscounts,
# leaks a path, a file name or an image hash into a log, or reports a
# harness fault (a bad BIOS, a usage error, a crash, a timeout, an unwritable
# output) as data. Every predicate and git stream is captured and never
# printed; the only output is the "local:" lines, ids and counts only.
foreach(var LEDGER PREDICATE BIOS DIR OUT ROOT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "local: -D${var} is required")
  endif()
endforeach()
include(${CMAKE_CURRENT_LIST_DIR}/files.cmake)

# A BIOS is 4 KiB or 16 KiB mapped to end at $FFFF, and its reset vector,
# the last bytes but two, must point inside it (A7800-SYSTEM section 7); the
# same rule as src/instance.c.
execute_process(COMMAND ${CMAKE_COMMAND} -E cat ${BIOS} RESULT_VARIABLE bios_rc
  OUTPUT_QUIET ERROR_QUIET)
set(bios_ok FALSE)
if(bios_rc STREQUAL "0")
  file(SIZE ${BIOS} bios_size)
  if(bios_size EQUAL 4096 OR bios_size EQUAL 16384)
    math(EXPR bios_at "${bios_size} - 4")
    file(READ ${BIOS} bios_vector OFFSET ${bios_at} LIMIT 2 HEX)
    string(SUBSTRING "${bios_vector}" 0 2 bios_lo)
    string(SUBSTRING "${bios_vector}" 2 2 bios_hi)
    math(EXPR bios_reset "0x${bios_hi}${bios_lo}")
    math(EXPR bios_floor "65536 - ${bios_size}")
    if(bios_reset GREATER_EQUAL bios_floor)
      set(bios_ok TRUE)
    endif()
  endif()
endif()
if(NOT bios_ok)
  message(FATAL_ERROR "local: the BIOS is unreadable or invalid")
endif()

# The output is probed before any evaluation. file(WRITE) runs only after
# this passes, so CMake's own error text, which names the path, never
# reaches the log.
get_filename_component(out_dir ${OUT} DIRECTORY)
set(out_rows ${out_dir}/local)
foreach(step "rm;-rf;${out_rows}" "make_directory;${out_rows}" "touch;${OUT}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E ${step} RESULT_VARIABLE out_rc
    OUTPUT_QUIET ERROR_QUIET)
  if(NOT out_rc STREQUAL "0")
    message(FATAL_ERROR "local: cannot write the counts file")
  endif()
endforeach()

# The population: every row whose id has no "/". The ledger's fields hold no
# comma, quote or newline; a predicate's ";why=" is kept from list splitting
# as a placeholder. A row's key is its first chip's SHA-1.
file(READ ${LEDGER} text)
string(REPLACE ";" "<semicolon>" text "${text}")
string(REGEX REPLACE "\n$" "" text "${text}")
string(REPLACE "\n" ";" lines "${text}")
list(POP_FRONT lines header)
set(ids "")
foreach(line IN LISTS lines)
  string(REPLACE "," ";" f "${line}")
  list(GET f 0 id)
  if(id MATCHES "/")
    continue()
  endif()
  list(GET f 2 raw)
  list(GET f 9 predicate)
  list(GET f 10 status)
  list(APPEND ids ${id})
  set(row_predicate_${id} "${predicate}")
  set(row_status_${id} absent)
  set(row_key_${id} "")
  string(REGEX MATCH "^[^+]*" key "${raw}")
  if(NOT status STREQUAL "skipped" AND key MATCHES "^[0-9a-f]+$")
    string(LENGTH "${key}" key_length)
    if(key_length EQUAL 40)
      if(DEFINED key_id_${key})
        message(FATAL_ERROR "local: ${key_id_${key}} and ${id} share one chip SHA-1")
      endif()
      set(key_id_${key} ${id})
      set(row_key_${id} ${key})
    endif()
  endif()
endforeach()
list(LENGTH ids total)

# The regular files of DIR that the core can read, in sorted order. Links and
# unreadable entries are counted, never read or named (files.cmake).
compat_files(local files unread ${DIR})

# Identify them in batches; the first file of a key is the row's file.
list(LENGTH files file_count)
set(at 0)
while(at LESS file_count)
  math(EXPR left "${file_count} - ${at}")
  if(left GREATER 64)
    set(left 64)
  endif()
  list(SUBLIST files ${at} ${left} batch)
  execute_process(COMMAND ${EMULATOR} ${PREDICATE} --identify ${batch}
    OUTPUT_VARIABLE keys ERROR_VARIABLE keys_error RESULT_VARIABLE keys_rc TIMEOUT 60)
  string(STRIP "${keys}" keys)
  string(REPLACE "\n" ";" keys "${keys}")
  list(LENGTH keys key_count)
  if(NOT keys_rc STREQUAL "0" OR NOT key_count EQUAL left)
    message(FATAL_ERROR "local: identification failed for a batch of ${left} files")
  endif()
  set(i 0)
  foreach(key IN LISTS keys)
    if(DEFINED key_id_${key} AND NOT DEFINED key_file_${key})
      list(GET batch ${i} key_file_${key})
    endif()
    math(EXPR i "${i} + 1")
  endforeach()
  math(EXPR at "${at} + ${left}")
endwhile()

# Judge each present row: it passes only when the fast boot and the BIOS
# path both pass. The predicate exits 3 when the core refuses an image it
# identified, which is a fail; every other end without a result line is a
# harness fault.
set(present 0)
foreach(id IN LISTS ids)
  set(key ${row_key_${id}})
  if(key STREQUAL "" OR NOT DEFINED key_file_${key})
    continue()
  endif()
  math(EXPR present "${present} + 1")
  set(predicate "${row_predicate_${id}}")
  string(REPLACE "<semicolon>" ";" predicate "${predicate}")
  execute_process(COMMAND ${EMULATOR} ${PREDICATE} ${key_file_${key}} "${predicate}"
      --bios ${BIOS} --id ${id} --result ${out_rows}/${id}.csv
    OUTPUT_VARIABLE eval_out ERROR_VARIABLE eval_error RESULT_VARIABLE eval_rc TIMEOUT 60)
  if(eval_rc STREQUAL "3")
    set(row_status_${id} fail)
  elseif(eval_rc STREQUAL "0" AND EXISTS ${out_rows}/${id}.csv)
    file(READ ${out_rows}/${id}.csv result)
    if(result MATCHES ",(pass|fail),[^,]*\n$")
      set(row_status_${id} ${CMAKE_MATCH_1})
    else()
      message(FATAL_ERROR "local: ${id}: evaluation ended without a result")
    endif()
  else()
    message(FATAL_ERROR "local: ${id}: evaluation ended without a result")
  endif()
endforeach()

# Every row is written, sorted by id then predicate as pin.cmake does.
set(rows "")
set(pass 0)
set(fail 0)
foreach(id IN LISTS ids)
  if(row_status_${id} STREQUAL "pass")
    math(EXPR pass "${pass} + 1")
  elseif(row_status_${id} STREQUAL "fail")
    math(EXPR fail "${fail} + 1")
  endif()
  list(APPEND rows "${id} ${row_predicate_${id}}\t${id},${row_predicate_${id}},${row_status_${id}}")
endforeach()
list(SORT rows)
list(TRANSFORM rows REPLACE "^[^\t]*\t" "")
list(LENGTH rows row_count)
if(NOT row_count EQUAL total)
  message(FATAL_ERROR "local: wrote ${row_count} rows, not ${total}")
endif()
list(JOIN rows "\n" body)
string(REPLACE "<semicolon>" ";" body "id,predicate,status\n${body}\n")
file(WRITE ${OUT} "${body}")
file(READ ${OUT} written)
if(NOT written STREQUAL body)
  message(FATAL_ERROR "local: cannot write the counts file")
endif()

# The commit and whether the tree is clean, with git's streams captured.
execute_process(COMMAND git -C ${ROOT} rev-parse --short=12 HEAD
  OUTPUT_VARIABLE commit ERROR_QUIET RESULT_VARIABLE git_rc OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git -C ${ROOT} status --porcelain --untracked-files=no
  OUTPUT_VARIABLE changes ERROR_QUIET RESULT_VARIABLE status_rc OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT git_rc STREQUAL "0" OR NOT commit MATCHES "^[0-9a-f]+$")
  set(commit unknown)
endif()
set(tree clean)
if(NOT status_rc STREQUAL "0" OR NOT changes STREQUAL "")
  set(tree dirty)
endif()
math(EXPR absent "${total} - ${present}")
message(STATUS "local: ${unread} files unread (links or unreadable)")
message(STATUS "local: ${present} of ${total} present, ${pass} pass, ${fail} fail, ${absent} absent, commit=${commit} tree=${tree}")
