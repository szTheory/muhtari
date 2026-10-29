# The local driver (local.cmake) on public bytes only. Run with -P:
#   cmake -DPREDICATE=<muhtari-test-predicate-testdb> -DLOCAL=<local.cmake>
#     -DBIOS=<openbios.bin> -DBUILT=<built dir> -DPROBE=<muhtari-test-images dir>
#     -DWORK=<scratch dir> -DROOT=<source tree> [-DEMULATOR=<wrapper>]
#     -P local_fixture.cmake
# Faults this prevents: a private run that counts a row present that is not,
# or the reverse, that reports a refused image or a failing predicate as a
# harness fault, that passes on a bad BIOS, a malformed predicate or an
# unwritable output, that treats a name CMake lists cannot carry as a file,
# that lets a relative OUT delete a directory it did not create, that replaces
# a scratch tree it did not make, or that prints a path, a file name or an
# image hash.
# The files are the test list's stripped sample (twice, with and without a
# header, one under a name with balanced brackets), an image the list
# identifies and the core refuses, a file above the image maximum, a text
# file, a dangling link, a valid link and a file the running user cannot read.
# The same rule runs under the "library" prefix on three of those directories
# (the library preset's configure-time scans use it), and library.cmake's own
# source is checked for the two scans that must go through it.
foreach(var PREDICATE LOCAL BIOS BUILT PROBE WORK ROOT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "local fixture: -D${var} is required")
  endif()
endforeach()
get_filename_component(compat_dir ${LOCAL} DIRECTORY)

file(REMOVE_RECURSE ${WORK})
file(MAKE_DIRECTORY ${WORK}/dir)
file(SHA1 ${BUILT}/simple/simple.bas.bin simple_sha)
file(SHA1 ${PROBE}/not-a-cartridge-4k refused_sha)
file(COPY ${BUILT}/simple/simple.bas.bin ${BUILT}/simple/simple.bas.a78 ${PROBE}/not-a-cartridge-4k
  DESTINATION ${WORK}/dir)
string(REPEAT "x" 1048705 big)
file(WRITE ${WORK}/dir/big.rom "${big}")
file(WRITE ${WORK}/dir/notes.txt "not a cartridge\n")
# A space sorts before a dot, so this copy is the file the row is read from.
file(COPY_FILE ${BUILT}/simple/simple.bas.a78 "${WORK}/dir/simple [!].a78")
# A link is counted unread and never followed, dangling or not.
set(unread_want 0)
set(links_note "")
file(CREATE_LINK "${WORK}/missing.a78" "${WORK}/dir/gone.a78" RESULT gone_rc SYMBOLIC)
file(CREATE_LINK "${WORK}/dir/notes.txt" "${WORK}/dir/alias.a78" RESULT alias_rc SYMBOLIC)
if(gone_rc STREQUAL "0" AND alias_rc STREQUAL "0")
  math(EXPR unread_want "${unread_want} + 2")
elseif(CMAKE_HOST_WIN32)
  file(REMOVE "${WORK}/dir/gone.a78" "${WORK}/dir/alias.a78")
  set(links_note "local fixture: links not created on this host")
else()
  message(FATAL_ERROR "local fixture: cannot create a symbolic link")
endif()
# A write-only file is unreadable to the user who owns it, and file(SIZE) on it
# prints its full path. A user who ignores the mode bits (root) or a host with
# no read bit (Windows) reads it like any other file.
file(WRITE "${WORK}/dir/locked.a78" "locked file\n")
file(CHMOD "${WORK}/dir/locked.a78" PERMISSIONS OWNER_WRITE)
set(locked_note "")
if(IS_READABLE "${WORK}/dir/locked.a78")
  set(locked_note "local fixture: the write-only file is readable on this host")
else()
  math(EXPR unread_want "${unread_want} + 1")
endif()
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

# Runs the driver once, from WORK, on drive_dir (a case may set it); sets out
# and rc in the caller.
set(drive_dir ${WORK}/dir)
function(drive name ledger bios outfile)
  execute_process(COMMAND ${CMAKE_COMMAND} -DLEDGER=${WORK}/${ledger}.csv -DPREDICATE=${PREDICATE}
      -DBIOS=${bios} "-DDIR=${drive_dir}" "-DOUT=${outfile}" -DROOT=${ROOT} "-DEMULATOR=${EMULATOR}"
      -P ${LOCAL}
    WORKING_DIRECTORY ${WORK}
    OUTPUT_VARIABLE o ERROR_VARIABLE o RESULT_VARIABLE r)
  # Nothing private reaches the log: no file name, no path, no image hash.
  foreach(secret simple.bas not-a-cartridge gone.a78 alias.a78 locked.a78 "[!]"
      ${WORK} ${PROBE} ${simple_sha} ${refused_sha})
    string(FIND "${o}" "${secret}" at)
    if(NOT at EQUAL -1)
      message(FATAL_ERROR "local fixture: ${name}: the output names a file, a path or a hash")
    endif()
  endforeach()
  # Every "local:" line is a counts line, the unread line or one of the named
  # faults.
  string(REPLACE "\n" ";" o_lines "${o}")
  foreach(line IN LISTS o_lines)
    if(line MATCHES "local:" AND NOT line MATCHES
        "^(-- )?local: [0-9]+ of 2 present, [0-9]+ pass, [0-9]+ fail, [0-9]+ absent, commit=[0-9a-f]+ tree=(clean|dirty)$"
        AND NOT line MATCHES "^(-- )?local: [0-9]+ files unread \\(links or unreadable\\)$"
        AND NOT line MATCHES
        "^ *local: (the BIOS is unreadable or invalid|cannot write the counts file|fixturepresent: evaluation ended without a result|a file name holds an unmatched bracket or a semicolon|the directory name holds a bracket or a semicolon|the scratch directory beside OUT was not made by this pass)$")
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
  if(NOT out MATCHES "local: ${unread_want} files unread \\(links or unreadable\\)\n")
    message(FATAL_ERROR "local fixture: ${name}: no unread line for ${unread_want}\n${out}")
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
# The line itself, so the log shows what the base run counted as unread.
string(REGEX MATCH "local: [0-9]+ files unread \\(links or unreadable\\)" unread_line "${out}")
message(STATUS "${unread_line}")

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

# 7. A file name with a ';' is a name CMake lists cannot carry.
file(MAKE_DIRECTORY ${WORK}/semi)
file(WRITE "${WORK}/semi/a;b.bin" "x")
set(drive_dir ${WORK}/semi)
drive(semi base ${BIOS} ${WORK}/semi.out.csv)
expect_fault(semi "a file name holds an unmatched bracket or a semicolon")

# 8. An unmatched '[' merges the names that follow it.
file(MAKE_DIRECTORY ${WORK}/bracket)
file(WRITE "${WORK}/bracket/b [x.bin" "x")
file(WRITE "${WORK}/bracket/c.bin" "x")
set(drive_dir ${WORK}/bracket)
drive(bracket base ${BIOS} ${WORK}/bracket.out.csv)
expect_fault(bracket "a file name holds an unmatched bracket or a semicolon")

# 9. A directory name with a bracket stops the scan before any glob.
file(MAKE_DIRECTORY "${WORK}/dir[x")
set(drive_dir "${WORK}/dir[x")
drive(dirname base ${BIOS} ${WORK}/dirname.out.csv)
expect_fault(dirname "the directory name holds a bracket or a semicolon")
set(drive_dir ${WORK}/dir)

# 10. A relative OUT is taken from the working directory; the old scratch tree
#     beside OUT, a directory named local, is not the pass's to delete.
file(MAKE_DIRECTORY ${WORK}/sub/local)
file(WRITE ${WORK}/sub/local/keep "keep")
drive(sub/relative base ${BIOS} sub/relative.out.csv)
expect_counts(sub/relative 1 0 "boots@120,pass")
if(NOT EXISTS ${WORK}/sub/local/keep)
  message(FATAL_ERROR "local fixture: relative: the run deleted a directory beside OUT")
endif()

# 11. A scratch tree of OUT's name that the pass did not make is left alone;
#     the tree the pass made itself is replaced on a re-run.
file(MAKE_DIRECTORY ${WORK}/foreign.out.csv.rows)
file(WRITE ${WORK}/foreign.out.csv.rows/keep "keep")
drive(foreign base ${BIOS} ${WORK}/foreign.out.csv)
expect_fault(foreign "the scratch directory beside OUT was not made by this pass")
if(NOT EXISTS ${WORK}/foreign.out.csv.rows/keep)
  message(FATAL_ERROR "local fixture: foreign: the run deleted a directory it did not make")
endif()
drive(base base ${BIOS} ${WORK}/base.out.csv)
expect_counts(base 1 0 "boots@120,pass")
message(STATUS "local fixture: eleven runs as expected")

# The shared rule under the library prefix, through files.cmake's -P entry; the
# fixture runs it from the source tree, so a fault's call stack names only
# source files. Sets out and rc in the caller.
function(scan name dir)
  execute_process(COMMAND ${CMAKE_COMMAND} -DPREFIX=library "-DDIR=${dir}"
      -P ${compat_dir}/files.cmake
    OUTPUT_VARIABLE o ERROR_VARIABLE o RESULT_VARIABLE r)
  foreach(secret simple.bas not-a-cartridge gone.a78 alias.a78 locked.a78 "[!]"
      ${WORK} ${PROBE} ${simple_sha} ${refused_sha})
    string(FIND "${o}" "${secret}" at)
    if(NOT at EQUAL -1)
      message(FATAL_ERROR "local fixture: ${name}: the output names a file, a path or a hash")
    endif()
  endforeach()
  string(REPLACE "\n" ";" o_lines "${o}")
  foreach(line IN LISTS o_lines)
    if(line MATCHES "library:" AND NOT line MATCHES
        "^(-- )?library: [0-9]+ files kept, [0-9]+ unread$"
        AND NOT line MATCHES
        "^ *library: (a file name holds an unmatched bracket or a semicolon|the directory name holds a bracket or a semicolon)$")
      message(FATAL_ERROR "local fixture: ${name}: unexpected line: ${line}")
    endif()
  endforeach()
  set(out "${o}" PARENT_SCOPE)
  set(rc "${r}" PARENT_SCOPE)
endfunction()

function(expect_scan_fault name message_text)
  if(rc STREQUAL "0")
    message(FATAL_ERROR "local fixture: ${name}: a library scan fault exited 0\n${out}")
  endif()
  if(NOT out MATCHES "\n  library: ${message_text}\n")
    message(FATAL_ERROR "local fixture: ${name}: no \"library: ${message_text}\"\n${out}")
  endif()
endfunction()

# The images, the text file, the bracketed copy and the readable write-only file
# are kept; the links and the unreadable file are counted; the file above the
# maximum is neither.
set(kept_want 5)
if(NOT locked_note STREQUAL "")
  math(EXPR kept_want "${kept_want} + 1")
endif()
scan(library-dir ${WORK}/dir)
if(NOT rc STREQUAL "0" OR NOT out MATCHES "library: ${kept_want} files kept, ${unread_want} unread\n")
  message(FATAL_ERROR "local fixture: library-dir: not ${kept_want} kept and ${unread_want} unread\n${out}")
endif()
scan(library-semi ${WORK}/semi)
expect_scan_fault(library-semi "a file name holds an unmatched bracket or a semicolon")
scan(library-dirname "${WORK}/dir[x")
expect_scan_fault(library-dirname "the directory name holds a bracket or a semicolon")
message(STATUS "local fixture: three library scans as expected")
foreach(note "${links_note}" "${locked_note}")
  if(NOT note STREQUAL "")
    message(STATUS "${note}")
  endif()
endforeach()

# library.cmake's two configure-time scans (the SHA-1 index of the roots and
# the local pass's candidate count) must go through the shared rule; the
# library CI job runs them only on its pass path, so this source check holds
# their fault direction on every ci lane. The text is normalised first, so an
# edit that keeps the rule still passes: comments are removed, case is folded
# (CMake commands are case-insensitive), whitespace and newlines collapse to a
# space and a space inside a call's parentheses is dropped.
function(library_normalise text out)
  # Placeholders keep the text's own ';', '[' and ']' out of list splitting.
  string(REPLACE ";" "<semi>" text "${text}")
  string(REPLACE "[" "<lb>" text "${text}")
  string(REPLACE "]" "<rb>" text "${text}")
  string(REPLACE "\n" ";" lines "${text}")
  set(plain "")
  foreach(line IN LISTS lines)
    # A '#' outside a quoted string starts a comment that runs to the line's end.
    string(REGEX REPLACE [[^(("[^"]*"|[^"#])*)#.*$]] [[\1]] line "${line}")
    string(APPEND plain "${line}\n")
  endforeach()
  string(TOLOWER "${plain}" plain)
  string(REGEX REPLACE "[ \t\r\n]+" " " plain "${plain}")
  string(REPLACE "( " "(" plain "${plain}")
  string(REPLACE " )" ")" plain "${plain}")
  string(STRIP "${plain}" plain)
  set(${out} "${plain}" PARENT_SCOPE)
endfunction()

# Sets result to an empty string when the text of library.cmake passes and to a
# fixed one-line reason otherwise. Past the files.cmake include it requires: no
# file(GLOB), file(GLOB_RECURSE) or file(SIZE); exactly two compat_files(library
# calls; every loop that runs file(SHA1) over a list that one of those calls
# returned, with the quoted loop variable as the file(SHA1) operand; and
# compat_name_check(library "${library_dir}") before MUHTARI_ROM_DIR is made
# absolute. The script-mode branch before the include is outside the check.
# Deliberate pins, and how to update them: exactly two compat_files(library
# calls (change the count here when a scan is added or removed), and file(SHA1)
# operands that are quoted loop variables only (a new SHA-1 site must loop over
# a compat_files list).
function(library_source_fault text result)
  set(${result} "" PARENT_SCOPE)
  library_normalise("${text}" plain)
  set(include_call "include(\${cmake_current_list_dir}/files.cmake)")
  string(FIND "${plain}" "${include_call}" at)
  if(at EQUAL -1)
    set(${result} "files.cmake is not included" PARENT_SCOPE)
    return()
  endif()
  string(LENGTH "${include_call}" include_length)
  math(EXPR from "${at} + ${include_length}")
  string(SUBSTRING "${plain}" ${from} -1 region)
  foreach(banned "file(glob" "file(size")
    string(FIND "${region}" "${banned}" at)
    if(NOT at EQUAL -1)
      set(${result} "a scan calls ${banned}) directly" PARENT_SCOPE)
      return()
    endif()
  endforeach()
  string(REGEX MATCHALL "compat_files\\(library [^ ]+" calls "${region}")
  list(LENGTH calls call_count)
  if(NOT call_count EQUAL 2)
    set(${result} "compat_files(library is called ${call_count} times, not twice" PARENT_SCOPE)
    return()
  endif()
  set(rule_lists "")
  foreach(call IN LISTS calls)
    string(REGEX REPLACE "^compat_files\\(library " "" rule_list "${call}")
    list(APPEND rule_lists "${rule_list}")
  endforeach()
  string(REGEX MATCHALL "file\\(sha1 " sha_sites "${region}")
  list(LENGTH sha_sites sha_total)
  set(sha_proven 0)
  string(REGEX MATCHALL "foreach\\([^ ]+ in lists [^ )]+\\)" heads "${region}")
  foreach(head IN LISTS heads)
    string(REGEX REPLACE "^foreach\\(([^ ]+) in lists ([^ )]+)\\)$" "\\1" var "${head}")
    string(REGEX REPLACE "^foreach\\(([^ ]+) in lists ([^ )]+)\\)$" "\\2" list_name "${head}")
    string(FIND "${region}" "${head}" at)
    string(SUBSTRING "${region}" ${at} -1 tail)
    string(FIND "${tail}" "endforeach()" end)
    if(end EQUAL -1)
      set(body "${tail}")
    else()
      string(SUBSTRING "${tail}" 0 ${end} body)
    endif()
    string(REGEX MATCHALL "file\\(sha1 [^ ]+" operands "${body}")
    if(operands STREQUAL "")
      continue()
    endif()
    list(FIND rule_lists "${list_name}" known)
    if(known EQUAL -1)
      set(${result} "a loop hashes a list no compat_files(library call returned" PARENT_SCOPE)
      return()
    endif()
    foreach(operand IN LISTS operands)
      if(NOT operand STREQUAL "file(sha1 \"\${${var}}\"")
        set(${result} "a file(SHA1) operand is not the quoted loop variable" PARENT_SCOPE)
        return()
      endif()
      math(EXPR sha_proven "${sha_proven} + 1")
    endforeach()
  endforeach()
  if(NOT sha_proven EQUAL sha_total)
    set(${result} "a file(SHA1) is outside a loop over a compat_files list" PARENT_SCOPE)
    return()
  endif()
  string(FIND "${region}" "compat_name_check(library \"\${library_dir}\")" named)
  string(FIND "${region}" "cmake_path(absolute_path library_dir" absolute)
  if(named EQUAL -1 OR absolute EQUAL -1 OR named GREATER absolute)
    set(${result} "the raw MUHTARI_ROM_DIR value is not name-checked before it is made absolute" PARENT_SCOPE)
    return()
  endif()
endfunction()

file(READ ${compat_dir}/library.cmake library_text)
library_source_fault("${library_text}" fault)
if(NOT fault STREQUAL "")
  message(FATAL_ERROR "local fixture: library source: ${fault}")
endif()

# Edits that keep the rule must pass: the call reflowed, the name check spaced,
# the loop variable renamed, and a comment that mentions file(SIZE.
set(call_old [[compat_files(library library_files library_unread ${library_roots})]])
set(call_new "compat_files(\n  library\n  library_files\n  library_unread\n  \${library_roots}\n)")
set(name_old [[compat_name_check(library "${library_dir}")]])
set(name_new [[compat_name_check( library "${library_dir}" )]])
set(loop_old "foreach(file IN LISTS library_files)\n  file(SHA1 \"\${file}\" sha)")
set(loop_new "foreach(path IN LISTS library_files)\n  file(SHA1 \"\${path}\" sha)")
set(sha_old [[file(SHA1 "${file}" sha)]])
set(sha_new [[file(SHA1 "${file}" sha)  # not file(SIZE ${file} size)]])
foreach(edit call name loop sha)
  string(REPLACE "${${edit}_old}" "${${edit}_new}" copy "${library_text}")
  if(copy STREQUAL library_text)
    message(FATAL_ERROR "local fixture: library source: a tolerance copy did not change")
  endif()
  library_source_fault("${copy}" fault)
  if(NOT fault STREQUAL "")
    message(FATAL_ERROR "local fixture: library source: rejected a tolerated edit ${edit}")
  endif()
endforeach()

# Edits that bypass the rule must fail: a raw glob and size loop appended, the
# name check removed, and a SHA-1 loop over the unchecked roots appended.
library_normalise("${library_text}" library_plain)
set(a_copy "${library_text}\nfile(GLOB_RECURSE extra \"\${library_dir}/*\")\nforeach(file IN LISTS extra)\n  file(SIZE \"\${file}\" size)\nendforeach()\n")
string(REPLACE "compat_name_check(library \"\${library_dir}\")" "" b_copy "${library_plain}")
set(c_copy "${library_text}\nforeach(file IN LISTS library_roots)\n  file(SHA1 \"\${file}\" sha)\nendforeach()\n")
foreach(fault_copy a b c)
  if(${fault_copy}_copy STREQUAL library_text OR ${fault_copy}_copy STREQUAL library_plain)
    message(FATAL_ERROR "local fixture: library source: a fault copy did not change")
  endif()
  library_source_fault("${${fault_copy}_copy}" fault)
  if(fault STREQUAL "")
    message(FATAL_ERROR "local fixture: library source check missed ${fault_copy}")
  endif()
endforeach()
message(STATUS "local fixture: library.cmake scans only through the shared rule")
