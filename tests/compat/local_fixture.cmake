# The local driver (local.cmake) on public bytes only. Run with -P:
#   cmake -DPREDICATE=<muhtari-test-predicate-testdb> -DLOCAL=<local.cmake>
#     -DBIOS=<openbios.bin> -DBUILT=<built dir> -DPROBE=<muhtari-test-images dir>
#     -DWORK=<scratch dir> -DROOT=<source tree> [-DEMULATOR=<wrapper>]
#     -P local_fixture.cmake
# Faults this prevents: a private run that counts a row present that is not,
# or the reverse, that reports a refused image or a failing predicate as a
# harness fault, that passes on a bad BIOS, a malformed predicate or an
# unwritable output, or that prints a path, a file name or an image hash.
# The files are the test list's stripped sample (twice, with and without a
# header), an image the list identifies and the core refuses, a file above
# the image maximum and a text file.
foreach(var PREDICATE LOCAL BIOS BUILT PROBE WORK ROOT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "local fixture: -D${var} is required")
  endif()
endforeach()

file(REMOVE_RECURSE ${WORK})
file(MAKE_DIRECTORY ${WORK}/dir)
file(SHA1 ${BUILT}/simple/simple.bas.bin simple_sha)
file(SHA1 ${PROBE}/not-a-cartridge-4k refused_sha)
file(COPY ${BUILT}/simple/simple.bas.bin ${BUILT}/simple/simple.bas.a78 ${PROBE}/not-a-cartridge-4k
  DESTINATION ${WORK}/dir)
string(REPEAT "x" 1048705 big)
file(WRITE ${WORK}/dir/big.rom "${big}")
file(WRITE ${WORK}/dir/notes.txt "not a cartridge\n")
file(WRITE ${WORK}/bios3 "abc")
file(WRITE ${WORK}/afile "")

# The committed simple row must never run here: a softlist run leaves every
# id with a "/" alone.
file(STRINGS ${ROOT}/tests/compat/ledger.csv ledger_header LIMIT_COUNT 1)
file(STRINGS ${ROOT}/tests/compat/ledger.csv owned REGEX "^7800basic-0[.]41/simple,")

# Writes the fixture ledger: the present row keyed to sha with predicate.
function(write_ledger name sha predicate)
  file(WRITE ${WORK}/${name}.csv "${ledger_header}\n${owned}\n"
    "fixtureabsent,Fixture absent,-,-,-,-,linear,NTSC,-,boots@120,unknown,local,-,-,-,-\n"
    "fixturepresent,Fixture present,${sha},-,-,-,linear,NTSC,-,${predicate},unknown,local,-,-,-,-\n")
endfunction()

# Runs the driver once; sets out, rc and the counts file's text in the caller.
function(drive name ledger bios outfile)
  execute_process(COMMAND ${CMAKE_COMMAND} -DLEDGER=${WORK}/${ledger}.csv -DPREDICATE=${PREDICATE}
      -DBIOS=${bios} -DDIR=${WORK}/dir -DOUT=${outfile} -DROOT=${ROOT} "-DEMULATOR=${EMULATOR}"
      -P ${LOCAL}
    OUTPUT_VARIABLE o ERROR_VARIABLE o RESULT_VARIABLE r)
  # Nothing private reaches the log: no file name, no path, no image hash.
  foreach(secret simple.bas not-a-cartridge ${WORK} ${PROBE} ${simple_sha} ${refused_sha})
    string(FIND "${o}" "${secret}" at)
    if(NOT at EQUAL -1)
      message(FATAL_ERROR "local fixture: ${name}: the output names a file, a path or a hash")
    endif()
  endforeach()
  # Every "local:" line is a counts line or one of the named faults.
  string(REPLACE "\n" ";" o_lines "${o}")
  foreach(line IN LISTS o_lines)
    if(line MATCHES "local:" AND NOT line MATCHES
        "^(-- )?local: [0-9]+ of 2 present, [0-9]+ pass, [0-9]+ fail, [0-9]+ absent, commit=[0-9a-f]+ tree=(clean|dirty)$"
        AND NOT line MATCHES
        "^ *local: (the BIOS is unreadable or invalid|cannot write the counts file|fixturepresent: evaluation ended without a result)$")
      message(FATAL_ERROR "local fixture: ${name}: unexpected line: ${line}")
    endif()
  endforeach()
  set(out "${o}" PARENT_SCOPE)
  set(rc "${r}" PARENT_SCOPE)
endfunction()

function(expect_counts name pass fail status)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "local fixture: ${name}: exit ${rc}, not 0\n${out}")
  endif()
  if(NOT out MATCHES "local: 1 of 2 present, ${pass} pass, ${fail} fail, 1 absent, commit=[0-9a-f]+ tree=(clean|dirty)\n")
    message(FATAL_ERROR "local fixture: ${name}: no counts line for ${pass} pass, ${fail} fail\n${out}")
  endif()
  file(READ ${WORK}/${name}.out.csv csv)
  set(want "id,predicate,status\nfixtureabsent,boots@120,absent\nfixturepresent,${status}\n")
  if(NOT csv STREQUAL want)
    message(FATAL_ERROR "local fixture: ${name}: the counts file differs\n${csv}")
  endif()
endfunction()

function(expect_fault name message_text)
  if(rc STREQUAL "0")
    message(FATAL_ERROR "local fixture: ${name}: a harness fault exited 0\n${out}")
  endif()
  if(NOT out MATCHES "\n  local: ${message_text}\n")
    message(FATAL_ERROR "local fixture: ${name}: no \"local: ${message_text}\"\n${out}")
  endif()
endfunction()

# 1. One present row that passes, one absent; the files above the maximum,
#    of no cartridge and of a second name for one image change nothing.
write_ledger(base ${simple_sha} boots@120)
drive(base base ${BIOS} ${WORK}/base.out.csv)
expect_counts(base 1 0 "boots@120,pass")

# 2. The present row's predicate fails: a verdict, not a fault.
set(zeros 0000000000000000000000000000000000000000000000000000000000000000)
write_ledger(failing ${simple_sha} "title@120=${zeros}")
drive(failing failing ${BIOS} ${WORK}/failing.out.csv)
expect_counts(failing 0 1 "title@120=${zeros},fail")

# 3. The row is keyed to an image the list identifies and the loader
#    refuses: the verdict on that file, a fail, not a fault.
write_ledger(refused ${refused_sha} boots@120)
drive(refused refused ${BIOS} ${WORK}/refused.out.csv)
expect_counts(refused 0 1 "boots@120,fail")

# 4. A BIOS the core cannot use is named before any evaluation.
drive(bios base ${WORK}/bios3 ${WORK}/bios.out.csv)
expect_fault(bios "the BIOS is unreadable or invalid")

# 5. A predicate the program does not accept is a harness fault naming the id.
write_ledger(malformed ${simple_sha} boots@0)
drive(malformed malformed ${BIOS} ${WORK}/malformed.out.csv)
expect_fault(malformed "fixturepresent: evaluation ended without a result")

# 6. An output beneath a regular file is a harness fault.
drive(output base ${BIOS} ${WORK}/afile/local.csv)
expect_fault(output "cannot write the counts file")
message(STATUS "local fixture: six runs as expected")
