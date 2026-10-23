# Writes docs/compatibility.md from the compatibility ledger, run as
#   cmake -DLEDGER=tests/compat/ledger.csv -DOUT=docs/compatibility.md
#     [-DCHECK=<committed page>] -P tools/compat_doc.cmake
# Fault prevented: a published compatibility page that disagrees with the
# ledger. With CHECK (policy.compat_doc_fresh, library.compat_doc) the page in
# OUT must equal CHECK byte for byte. The page counts rows by status, board,
# region and weak (boots@) predicate, then lists one row per id.
cmake_minimum_required(VERSION 3.25)
if("${LEDGER}" STREQUAL "" OR "${OUT}" STREQUAL "")
  message(FATAL_ERROR "compat_doc: -DLEDGER and -DOUT are required")
endif()

# No field holds a comma; a predicate's ";why=" is kept as a placeholder.
file(READ "${LEDGER}" text)
string(REPLACE ";" "<semicolon>" text "${text}")
string(REGEX REPLACE "\n$" "" text "${text}")
string(REPLACE "\n" ";" lines "${text}")
list(POP_FRONT lines)
list(LENGTH lines total)

set(weak 0)
set(table "")
foreach(line IN LISTS lines)
  string(REPLACE "," ";" f "${line}")
  list(GET f 0 1 6 7 8 9 10 g)
  list(POP_FRONT g id title board region peripherals predicate status)
  foreach(column status board region)
    list(APPEND all_${column} "${${column}}")
  endforeach()
  string(REGEX MATCH "^[a-z]+" kind "${predicate}")
  if(kind STREQUAL "boots")
    math(EXPR weak "${weak} + 1")
  endif()
  if(predicate MATCHES "<semicolon>why=(.+)$")
    string(APPEND kind " (${CMAKE_MATCH_1})")
  endif()
  string(APPEND table "| `${id}` | ${title} | ${board} | ${region} | ${peripherals} | ${kind} | ${status} |\n")
endforeach()

# Appends to counts a table of column's values, sorted, and their rows.
function(compat_counts column heading)
  set(values ${all_${column}})
  list(REMOVE_DUPLICATES values)
  list(SORT values)
  set(out "| ${heading} | Rows |\n|---|---|\n")
  foreach(value IN LISTS values)
    set(n 0)
    foreach(each IN LISTS all_${column})
      if(each STREQUAL value)
        math(EXPR n "${n} + 1")
      endif()
    endforeach()
    string(APPEND out "| ${value} | ${n} |\n")
  endforeach()
  set(counts "${counts}${out}\n" PARENT_SCOPE)
endfunction()
set(counts "")
compat_counts(status Status)
compat_counts(board Board)
compat_counts(region Region)

file(WRITE "${OUT}" "# Compatibility

Generated from `tests/compat/ledger.csv` by `tools/compat_doc.cmake`; do not edit it by hand.
`docs/emulation-tests.md`, ledger, says what each column and status means and how a row is judged.

NTSC board support includes linear, SuperGame, Activision, Absolute and Fractalus.
Owned board diagnostics verify selected banking and physical RAM semantics; they do not promote an
untested retail title to a passing gameplay result. PAL execution remains unsupported.
[Cartridge behavior](hardware/cartridge.md) describes identified-only Activision AM normalization,
original image identity, physical 2 KiB Fractalus RAM and selected compatibility/profile policies.
[Host memory maps](libretro.md#achievements) expose fixed ROM and only the documented RAM aliases;
banked windows and Fractalus mirrors above $4FFF remain descriptor holes.

The ledger has ${total} rows. ${weak} are judged by the weak predicate `boots@F` (the game booted, drew and
kept running), not by a pinned title screen.

${counts}| Id | Title | Board | Region | Peripherals | Predicate | Status |
|---|---|---|---|---|---|---|
${table}")
if(NOT "${CHECK}" STREQUAL "")
  file(READ "${OUT}" fresh)
  file(READ "${CHECK}" committed)
  if(NOT fresh STREQUAL committed)
    message(FATAL_ERROR "compat_doc: ${CHECK} is stale; regenerate it with\n"
      "  cmake -DLEDGER=tests/compat/ledger.csv -DOUT=docs/compatibility.md -P tools/compat_doc.cmake")
  endif()
endif()
