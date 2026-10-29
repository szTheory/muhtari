# The compatibility ledger's title predicates (docs/emulation-tests.md,
# title), on the fast boot and through a BIOS.
include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)

# simple's title on both paths, its predicate read from the ledger here, so
# a re-pinned row reaches the tests at the next configure.
file(STRINGS ${compat_ledger} compat_simple REGEX "^7800basic-0[.]41/simple,")
string(REGEX MATCH ",(title@120=[0-9a-f]+)," compat_simple "${compat_simple}")
if(NOT CMAKE_MATCH_1)
  message(FATAL_ERROR "compat: ${compat_ledger} has no pinned title@120 row for 7800basic-0.41/simple")
endif()
set(compat_simple_title ${CMAKE_MATCH_1})
set(compat_simple_rom ${MUHTARI_BUILT_DIR}/simple/simple.bas.a78)
set(compat_bios ${MUHTARI_BUILT_DIR}/openbios/openbios.bin)
muhtari_add_test(NAME title.simple.fast
  COMMAND muhtari-test-predicate ${compat_simple_rom} ${compat_simple_title}
  LABELS portable title TIMEOUT 2)  # 2 x 0.60 s, macOS x64, slowest of 12 runs, run 39375738182
muhtari_add_test(NAME title.simple.bios
  COMMAND muhtari-test-predicate ${compat_simple_rom} ${compat_simple_title} --bios ${compat_bios}
  LABELS portable title TIMEOUT 2)  # 2 x 0.60 s, macOS x64, slowest of 12 runs, run 39375738182
set_tests_properties(title.simple.fast title.simple.bios PROPERTIES
  FIXTURES_REQUIRED "built_simple;built_openbios")

# A PAL image on both boot paths. Fault: a predicate that crashes or hashes
# the wrong rows on a PAL image, which would turn every PAL pinned-list entry
# into a harness fault rather than a verdict.
muhtari_add_test(NAME title.pal.fast
  COMMAND muhtari-test-predicate ${PROJECT_SOURCE_DIR}/tests/diag/pal.a78 boots@120
  LABELS portable title TIMEOUT 2)  # provisional, not yet measured in CI
muhtari_add_test(NAME title.pal.bios
  COMMAND muhtari-test-predicate ${PROJECT_SOURCE_DIR}/tests/diag/pal.a78 boots@120
    --bios ${compat_bios}
  LABELS portable title TIMEOUT 2)  # provisional, not yet measured in CI
set_tests_properties(title.pal.bios PROPERTIES FIXTURES_REQUIRED built_openbios)

# Negative control for the pin writer: the linear seed cartridge, filler
# bytes behind a valid footer, shows one colour at frame 120, so --pin must
# refuse it rather than pin a blank screen. The image comes from
# muhtari-test-images.
set(compat_cmd ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:muhtari-test-predicate>
  ${CMAKE_BINARY_DIR}/runner-probe/linear-v3-16k title@120=- --pin)
string(REPLACE ";" "$<SEMICOLON>" compat_cmd "${compat_cmd}")
muhtari_add_test(NAME title.pin.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${compat_cmd}" -DCODE=1 "-DREGEX=single colour"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS portable title TIMEOUT 1)  # 2 x 0.16 s, macOS x64, slowest of 12 runs, run 39375738182
set_tests_properties(title.pin.self_test PROPERTIES FIXTURES_REQUIRED probe_images)

# The pin recipe (pin.cmake) on the committed ledger and on a fixture. The
# recipe runs on copies under the build tree; CHECK compares the result with
# the source file. The copies are regenerated when the ledger changes.
set(compat_pin_dir ${CMAKE_BINARY_DIR}/title)
set(compat_pin_inputs -DPREDICATE=$<TARGET_FILE:muhtari-test-predicate>
  -DBUILT=${MUHTARI_BUILT_DIR} -DDIAG=${PROJECT_SOURCE_DIR}/tests/diag)
string(REPLACE ";" "$<SEMICOLON>" compat_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")

# Fault: a committed ledger that is out of pin order, or that has a row the
# recipe has no file rule for, so the next re-pin rewrites unrelated lines.
configure_file(${compat_ledger} ${compat_pin_dir}/pin_order/ledger.csv COPYONLY)
muhtari_add_test(NAME title.pin.order
  COMMAND ${CMAKE_COMMAND} ${compat_pin_inputs}
    -DLEDGER=${compat_pin_dir}/pin_order/ledger.csv -DCHECK=${compat_ledger}
    -P ${CMAKE_CURRENT_LIST_DIR}/pin.cmake
  LABELS portable title TIMEOUT 1)  # provisional, not yet measured in CI

# Fault: a check that passes on any ledger. The same copy with its last two
# data rows swapped must fail by name.
file(READ ${compat_ledger} compat_text)
string(REPLACE ";" "<semicolon>" compat_text "${compat_text}")
string(REGEX REPLACE "\n$" "" compat_text "${compat_text}")
string(REPLACE "\n" ";" compat_rows "${compat_text}")
list(POP_BACK compat_rows compat_last)
list(POP_BACK compat_rows compat_prev)
list(APPEND compat_rows "${compat_last}" "${compat_prev}")
list(JOIN compat_rows "\n" compat_swapped)
string(REPLACE "<semicolon>" ";" compat_swapped "${compat_swapped}\n")
file(WRITE ${compat_pin_dir}/pin_swapped/ledger.csv "${compat_swapped}")
file(WRITE ${compat_pin_dir}/pin_swapped/expected.csv "${compat_swapped}")
set(compat_cmd ${CMAKE_COMMAND} ${compat_pin_inputs}
  -DLEDGER=${compat_pin_dir}/pin_swapped/ledger.csv -DCHECK=${compat_pin_dir}/pin_swapped/expected.csv
  -P ${CMAKE_CURRENT_LIST_DIR}/pin.cmake)
string(REPLACE ";" "$<SEMICOLON>" compat_cmd "${compat_cmd}")
muhtari_add_test(NAME title.pin.order.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${compat_cmd}" -DCODE=1 "-DREGEX=differs from"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS portable title TIMEOUT 1)  # provisional, not yet measured in CI

# Fault: the recipe dying on a mailbox@ row or on a row with no file rule.
# The fixture is the four mailbox@ rows, diag/cpu and one bare row, in pin
# order; the run must leave it byte-identical.
file(READ ${compat_ledger} compat_text)
string(REGEX MATCH "^[^\n]*" compat_fixture "${compat_text}")
foreach(compat_id bankset cpu pal xboard xm)
  string(REGEX MATCH "\n(diag/${compat_id},[^\n]*)" compat_row "${compat_text}")
  string(APPEND compat_fixture "\n${CMAKE_MATCH_1}")
endforeach()
string(APPEND compat_fixture "\nfixture,-,-,-,-,-,-,-,-,boots@120,unknown,local,-,-,-,-\n")
file(WRITE ${compat_pin_dir}/pin_recipe/ledger.csv "${compat_fixture}")
file(WRITE ${compat_pin_dir}/pin_recipe/expected.csv "${compat_fixture}")
muhtari_add_test(NAME title.pin.recipe
  COMMAND ${CMAKE_COMMAND} ${compat_pin_inputs}
    -DLEDGER=${compat_pin_dir}/pin_recipe/ledger.csv -DBIOS=${compat_bios}
    "-DEMULATOR=${compat_emulator}" -DCHECK=${compat_pin_dir}/pin_recipe/expected.csv
    -P ${CMAKE_CURRENT_LIST_DIR}/pin.cmake
  LABELS portable title TIMEOUT 3)  # provisional, not yet measured in CI
set_tests_properties(title.pin.recipe PROPERTIES FIXTURES_REQUIRED built_openbios)
