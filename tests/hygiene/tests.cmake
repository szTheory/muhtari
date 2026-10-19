# Hygiene tests, registered only by the hygiene preset (MUHTARI_HYGIENE).

# No personal data, unlisted binary, game image or GPL text in any file git
# would track.
muhtari_add_test(NAME hygiene.tree
  COMMAND ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/hygiene.sh --tree
  LABELS hygiene TIMEOUT 60)

# Every tracked C file but the vendored libretro.h is formatted as
# .clang-format says.
muhtari_add_test(NAME hygiene.format
  COMMAND ${MUHTARI_SH} -c "cd \"$1\" && ${GIT_EXECUTABLE} ls-files -z '*.c' '*.h' \
':!:libretro/libretro.h' | xargs -0 \"$2\" --dry-run --Werror" sh
    ${CMAKE_SOURCE_DIR} ${MUHTARI_CLANG_FORMAT}
  LABELS hygiene TIMEOUT 60)

# No floating point in the core's text. The self test scans a fixture with
# seven float lines beside three lines that only look like them; the SPDX
# pair shows a licence expression is passed over and the code after it is
# not.
set(hygiene_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)
muhtari_add_test(NAME hygiene.no_float
  COMMAND ${CMAKE_COMMAND} "-DROOTS=${CMAKE_SOURCE_DIR}/src$<SEMICOLON>${CMAKE_SOURCE_DIR}/include"
    -P ${hygiene_scripts}/no_float.cmake
  LABELS hygiene TIMEOUT 30)
set(hygiene_fixture ${CMAKE_BINARY_DIR}/hygiene/no_float_fixture)
file(WRITE ${hygiene_fixture}/sample.c "int y = 0x1E5;\nint double_free = 2;\nx = 1.5;\n"
  "z = 0x1p3;\nw = 1e3;\ndouble d;\n#include <math.h>\n#include <float.h>\n"
  "/* SPDX-License-Identifier: MIT AND CC0-1.0 */\n/* SPDX-License-Identifier: MIT */ double e;\n")
set(hygiene_cmd ${CMAKE_COMMAND} -DROOTS=${hygiene_fixture} -P ${hygiene_scripts}/no_float.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME hygiene.no_float.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=sample.c:3: x = 1.5.*sample.c:8: #include <float.h>.*sample.c:10: .*no_float: 7 lines"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene TIMEOUT 30)

# The clang static analyzer finds nothing in the core. The Annex K check is
# off: the core may call only the C memory functions (abi.undefined), and the
# _s variants it asks for are optional in C11 and absent from glibc.
muhtari_add_test(NAME hygiene.analyzer
  COMMAND ${MUHTARI_SH} -c "cd \"$1\" && \"$2\" -p \"$3\" --quiet \
-checks='-*,clang-analyzer-*,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling' \
--warnings-as-errors='*' src/*.c" sh
    ${CMAKE_SOURCE_DIR} ${MUHTARI_CLANG_TIDY} ${CMAKE_BINARY_DIR}
  LABELS hygiene TIMEOUT 120)

# Every pins.txt row is well formed. The self test runs scripts/fetch-pin.sh
# on a row whose digest is wrong for its file, which must stop before DEST
# is touched.
muhtari_add_test(NAME policy.pins
  COMMAND ${CMAKE_COMMAND} -DPINS=${CMAKE_SOURCE_DIR}/pins.txt -P ${hygiene_scripts}/pins.cmake
  LABELS hygiene policy TIMEOUT 10)
set(hygiene_pins ${CMAKE_BINARY_DIR}/hygiene/pins_fixture)
file(WRITE ${hygiene_pins}/payload.txt "not the pinned bytes\n")
string(REPEAT "0" 64 hygiene_zeros)
file(WRITE ${hygiene_pins}/pins.txt "payload 1 file://${hygiene_pins}/payload.txt ${hygiene_zeros}\n")
set(hygiene_cmd ${CMAKE_COMMAND} -E env PINS=${hygiene_pins}/pins.txt
  ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/fetch-pin.sh payload ${hygiene_pins}/dest)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.pins.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1 "-DREGEX=payload: sha256 mismatch"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# Every manifest row matches its file and every fixture has a row. The self
# test runs the check on a copy of the manifest whose sample digest is zeros,
# which must name the sample.
set(hygiene_manifest ${CMAKE_SOURCE_DIR}/tests/roms/manifest.txt)
muhtari_add_test(NAME policy.rom_hash_coverage
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -DMANIFEST=${hygiene_manifest}
    -P ${hygiene_scripts}/rom_hash_coverage.cmake
  LABELS hygiene policy TIMEOUT 30)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${hygiene_manifest})
file(READ ${hygiene_manifest} hygiene_rows)
string(REGEX REPLACE "(\ntests/vectors/6502-sample\\.n65v\t[^\t\n]*\t[^\t\n]*\t[^\t\n]*\t)[0-9a-f]+"
  "\\1${hygiene_zeros}" hygiene_bad_rows "${hygiene_rows}")
if(hygiene_bad_rows STREQUAL hygiene_rows)
  message(FATAL_ERROR "policy.rom_hash_coverage.self_test: no sample row in ${hygiene_manifest}")
endif()
set(hygiene_bad_manifest ${CMAKE_BINARY_DIR}/hygiene/rom_hash_fixture/manifest.txt)
file(WRITE ${hygiene_bad_manifest} "${hygiene_bad_rows}")
set(hygiene_cmd ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR} -DMANIFEST=${hygiene_bad_manifest}
  -P ${hygiene_scripts}/rom_hash_coverage.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.rom_hash_coverage.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=tests/vectors/6502-sample\\.n65v: sha256 is [0-9a-f]+, the manifest says 0+"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# Pull-request titles and bodies. Each fixture line is want|title|body, the
# body with \n escapes; scripts/check-title.sh must give every want, and the
# fixtures must hold both kinds, so a check that passes or fails everything
# fails here.
set(hygiene_titles [=[
script=$1
passes=0
fails=0
while IFS='|' read -r want title body
do
  if [ -z "$want" ]
  then continue
  fi
  if printf '%b\n' "$body" | sh "$script" "$title" > /dev/null 2>&1
  then got=pass
  else got=fail
  fi
  if [ "$got" != "$want" ]
  then
    echo "policy.title: want $want, got $got: $title|$body"
    exit 1
  fi
  if [ "$want" = pass ]
  then passes=$((passes + 1))
  else fails=$((fails + 1))
  fi
done < "$2"
echo "policy.title: $passes pass and $fails fail fixtures"
[ "$passes" -gt 0 ] && [ "$fails" -gt 0 ]
]=])
muhtari_add_test(NAME policy.title
  COMMAND ${MUHTARI_SH} -c "${hygiene_titles}" sh ${CMAKE_SOURCE_DIR}/scripts/check-title.sh
    ${CMAKE_CURRENT_LIST_DIR}/fixtures/titles.txt
  LABELS hygiene policy TIMEOUT 30)

# The CI required roll-up, run on synthetic needs. The self test runs it on a
# copy of ci.yml whose program ignores heavy, which must let a skipped build
# through and so fail the case that names it.
set(hygiene_ci ${CMAKE_SOURCE_DIR}/.github/workflows/ci.yml)
muhtari_add_test(NAME policy.ci_required
  COMMAND ${CMAKE_COMMAND} -DCI_YML=${hygiene_ci} -DJQ=${MUHTARI_JQ} -DREPORT_ONLY=wasm32
    -P ${hygiene_scripts}/ci_required.cmake
  LABELS hygiene policy TIMEOUT 30)
file(READ ${hygiene_ci} hygiene_yml)
string(REPLACE "$heavy == \"false\" and " "" hygiene_yml "${hygiene_yml}")
file(WRITE ${CMAKE_BINARY_DIR}/hygiene/ci_required_fixture/ci.yml "${hygiene_yml}")
set(hygiene_cmd ${CMAKE_COMMAND} -DCI_YML=${CMAKE_BINARY_DIR}/hygiene/ci_required_fixture/ci.yml
  -DJQ=${MUHTARI_JQ} -DREPORT_ONLY=wasm32 -P ${hygiene_scripts}/ci_required.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.ci_required.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=build skipped when heavy=true: want fail, got exit 0"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# The Dependabot auto-merge decision, run on synthetic events. The self test
# runs it on a copy of the workflow whose program merges majors, which must
# fail the case that names them.
set(hygiene_dependabot ${CMAKE_SOURCE_DIR}/.github/workflows/dependabot-automerge.yml)
muhtari_add_test(NAME policy.dependabot
  COMMAND ${CMAKE_COMMAND} -DWORKFLOW=${hygiene_dependabot} -DJQ=${MUHTARI_JQ}
    -P ${hygiene_scripts}/dependabot.cmake
  LABELS hygiene policy TIMEOUT 30)
file(READ ${hygiene_dependabot} hygiene_yml)
string(REPLACE "else \"wait\" end" "else \"merge\" end" hygiene_yml "${hygiene_yml}")
file(WRITE ${CMAKE_BINARY_DIR}/hygiene/dependabot_fixture/dependabot-automerge.yml "${hygiene_yml}")
set(hygiene_cmd ${CMAKE_COMMAND}
  -DWORKFLOW=${CMAKE_BINARY_DIR}/hygiene/dependabot_fixture/dependabot-automerge.yml
  -DJQ=${MUHTARI_JQ} -P ${hygiene_scripts}/dependabot.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.dependabot.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=semver-major: want wait, got 'merge'"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# The changes job's classifier: heavy=false only for a non-empty list wholly
# under .planning/, both names of a rename included, from a pull request of at
# most 3000 files, the most the files API lists.
set(hygiene_classify [=[
script=$1
check() {
  got=$(printf '%b' "$2" | sh "$script" "${3-2}")
  if [ "$got" != "heavy=$1" ]
  then
    echo "ci.classify: want heavy=$1, got $got for: $2"
    exit 1
  fi
}
check true '.planning/STATE.md\nsrc/core.c\n'
check true 'src/core.c\n.planning/core.c\n'
check true ''
check true '\n\n'
check true '(listing failed)\n'
check true '.planning-notes/a.md\n'
check true 'docs/.planning/a.md\n'
check false '.planning/STATE.md\n.planning/phases/01/a.md\n'
check false '.planning/STATE.md'
check false '.planning/STATE.md\n' 3000
check true '.planning/STATE.md\n' 3001
check true '.planning/STATE.md\n' ''
check true '.planning/STATE.md\n' 'x'
echo "ci.classify: 13 lists"
]=])
muhtari_add_test(NAME ci.classify
  COMMAND ${MUHTARI_SH} -c "${hygiene_classify}" sh ${CMAKE_SOURCE_DIR}/scripts/classify-changes.sh
  LABELS hygiene TIMEOUT 30)

# Every workflow timeout names a run ID or says provisional. The self test
# scans a fixture with two good lines and two bad ones.
muhtari_add_test(NAME hygiene.timeout_comments
  COMMAND ${CMAKE_COMMAND} -DDIR=${CMAKE_SOURCE_DIR}/.github/workflows -P ${hygiene_scripts}/timeouts.cmake
  LABELS hygiene TIMEOUT 30)
set(hygiene_timeouts ${CMAKE_BINARY_DIR}/hygiene/timeouts_fixture)
file(WRITE ${hygiene_timeouts}/sample.yml "jobs:\n  a:\n    timeout-minutes: 5  # provisional\n"
  "  b:\n    timeout-minutes: 30\n  c:\n    timeout-minutes: 8  # 2 x 3 min, run 38935647258\n"
  "  d:\n    timeout-minutes: 15  # a guess\n")
set(hygiene_cmd ${CMAKE_COMMAND} -DDIR=${hygiene_timeouts} -P ${hygiene_scripts}/timeouts.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME hygiene.timeout_comments.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=sample.yml: timeout-minutes: 30.*sample.yml: timeout-minutes: 15 .*a guess.*timeout_comments: 2 lines"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene TIMEOUT 30)

# The release's asset set. scripts/publish.sh prepare runs on 18 zips that
# each hold LICENSE, so its firmware scan has a member to read, and six build
# legs' commit.txt; SHA256SUMS must list the 18 in C-locale name order. The
# self test leaves one zip out, which must fail naming it.
set(hygiene_publish ${CMAKE_BINARY_DIR}/hygiene/publish_set)
file(REMOVE_RECURSE ${hygiene_publish})
file(COPY ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${hygiene_publish}/stage)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar cf ${hygiene_publish}/one.zip --format=zip LICENSE
  WORKING_DIRECTORY ${hygiene_publish}/stage
  COMMAND_ERROR_IS_FATAL ANY)
file(MAKE_DIRECTORY ${hygiene_publish}/full ${hygiene_publish}/short)
foreach(c library runner libretro)
  foreach(o linux macos windows)
    foreach(a x64 arm64)
      file(COPY_FILE ${hygiene_publish}/one.zip ${hygiene_publish}/full/muhtari-${c}-${o}-${a}.zip
        INPUT_MAY_BE_RECENT)
      if(NOT "${c}-${o}-${a}" STREQUAL "runner-windows-x64")
        file(COPY_FILE ${hygiene_publish}/one.zip
          ${hygiene_publish}/short/muhtari-${c}-${o}-${a}.zip INPUT_MAY_BE_RECENT)
      endif()
    endforeach()
  endforeach()
endforeach()
foreach(leg RANGE 1 6)
  file(WRITE ${hygiene_publish}/full/legs/${leg}/commit.txt "fixture\n")
  file(WRITE ${hygiene_publish}/short/legs/${leg}/commit.txt "fixture\n")
endforeach()
set(hygiene_publish_check [=[
EXPECTED_SHA=fixture sh "$1" prepare "$2" || exit 1
names=$(awk '{ print $2 }' "$2/SHA256SUMS")
count=$(printf '%s\n' "$names" | grep -c '^muhtari-[a-z]*-[a-z]*-[a-z0-9]*\.zip$')
if [ "$count" -ne 18 ]
then
  echo "policy.publish_set: $count archives in SHA256SUMS, want 18"
  exit 1
fi
if ! printf '%s\n' "$names" | LC_ALL=C sort -c -u
then
  echo "policy.publish_set: SHA256SUMS is not in C-locale name order"
  exit 1
fi
echo "policy.publish_set: 18 archives in C-locale name order"
]=])
muhtari_add_test(NAME policy.publish_set
  COMMAND ${CMAKE_COMMAND} -E env CMAKE=${CMAKE_COMMAND} ${MUHTARI_SH} -c "${hygiene_publish_check}" sh ${CMAKE_SOURCE_DIR}/scripts/publish.sh
    ${hygiene_publish}/full
  LABELS hygiene policy TIMEOUT 30)
set(hygiene_cmd ${CMAKE_COMMAND} -E env EXPECTED_SHA=fixture CMAKE=${CMAKE_COMMAND}
  ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/publish.sh prepare ${hygiene_publish}/short)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.publish_set.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=publish: missing muhtari-runner-windows-x64.zip"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)
# Fault prevented: a release verifies against a pull request's attestation.
# verify on a release tag with a pull request's ref must refuse before any
# network call.
set(hygiene_cmd ${CMAKE_COMMAND} -E env GITHUB_REPOSITORY=fixture/fixture SOURCE_REF=refs/pull/1/merge
  ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/publish.sh verify fixture-release ${hygiene_publish}/full)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.publish_ref
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=publish: fixture-release verifies only from refs/heads/main, not refs/pull/1/merge"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# Fault prevented: a cartridge table edited by hand, or left behind by a
# change to its generator. The generator runs over the pinned MAME lists
# (fetched before configure, see CMakeLists.txt) into the build directory, and
# its output must equal src/cartdb.c byte for byte. The self test compares
# against a copy of src/cartdb.c with one byte changed, which must fail.
set(hygiene_cartdb [=[
"$1" "-DA7800=$3" "-DA2600=$4" "-DOUT=$5" -P "$2" || exit 1
if ! "$1" -E compare_files "$6" "$5"
then
  echo "policy.cartdb_generated: src/cartdb.c differs from the generator output. Regenerate it with"
  echo "  cmake -DA7800=$3 -DA2600=$4 -DOUT=src/cartdb.c -P tools/cartdb.cmake"
  exit 1
fi
echo "policy.cartdb_generated: src/cartdb.c equals the generator output"
]=])
set(hygiene_cartdb_args ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/tools/cartdb.cmake
  ${MUHTARI_MAME_PINS}/a7800.xml ${MUHTARI_MAME_PINS}/a2600.xml)
muhtari_add_test(NAME policy.cartdb_generated
  COMMAND ${MUHTARI_SH} -c "${hygiene_cartdb}" sh ${hygiene_cartdb_args}
    ${CMAKE_BINARY_DIR}/hygiene/cartdb/cartdb.c ${CMAKE_SOURCE_DIR}/src/cartdb.c
  LABELS hygiene policy TIMEOUT 30)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/cartdb.c)
file(READ ${CMAKE_SOURCE_DIR}/src/cartdb.c hygiene_rows)
string(FIND "${hygiene_rows}" "{{0x" hygiene_at)
if(hygiene_at EQUAL -1)
  message(FATAL_ERROR "policy.cartdb_generated.self_test: no key row in src/cartdb.c")
endif()
string(SUBSTRING "${hygiene_rows}" 0 ${hygiene_at} hygiene_head)
math(EXPR hygiene_at "${hygiene_at} + 4")
string(SUBSTRING "${hygiene_rows}" ${hygiene_at} -1 hygiene_tail)
file(WRITE ${CMAKE_BINARY_DIR}/hygiene/cartdb_fixture/cartdb.c "${hygiene_head}{{1x${hygiene_tail}")
set(hygiene_cmd ${MUHTARI_SH} -c "${hygiene_cartdb}" sh ${hygiene_cartdb_args}
  ${CMAKE_BINARY_DIR}/hygiene/cartdb_fixture/generated.c
  ${CMAKE_BINARY_DIR}/hygiene/cartdb_fixture/cartdb.c)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME policy.cartdb_generated.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=src/cartdb\\.c differs from the generator output"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene policy TIMEOUT 30)

# No tracked file is firmware, a built sample or a known ROM. The self test
# tracks a synthetic 256-byte A78 file in a fixture repository and lists the
# SHA-1 of its last 128 bytes, which must be found once the header is
# stripped.
set(hygiene_scan_lists -DEXPECTED=${CMAKE_SOURCE_DIR}/tests/built/expected.txt
  -DCARTDB=${CMAKE_SOURCE_DIR}/src/cartdb.c -DGIT=${GIT_EXECUTABLE} -DTRACKED=1)
muhtari_add_test(NAME hygiene.firmware_scan
  COMMAND ${CMAKE_COMMAND} -DROOT=${CMAKE_SOURCE_DIR}
    -DFIRMWARE=${CMAKE_SOURCE_DIR}/tests/release/firmware.txt ${hygiene_scan_lists}
    -P ${hygiene_scripts}/firmware_scan.cmake
  LABELS hygiene TIMEOUT 60)  # provisional, 9 samples
set(hygiene_scan ${CMAKE_BINARY_DIR}/hygiene/firmware_scan_fixture)
file(REMOVE_RECURSE ${hygiene_scan})
string(REPEAT "h" 118 hygiene_pad)
string(REPEAT "synthetic payload, not a ROM. " 5 hygiene_body)
string(SUBSTRING "${hygiene_body}" 0 128 hygiene_body)
file(WRITE ${hygiene_scan}/tree/synthetic.a78 "xATARI7800${hygiene_pad}${hygiene_body}")
string(SHA1 hygiene_sha1 "${hygiene_body}")
file(WRITE ${hygiene_scan}/firmware.txt "sha1 ${hygiene_sha1} synthetic\n")
execute_process(COMMAND ${GIT_EXECUTABLE} init -q WORKING_DIRECTORY ${hygiene_scan}/tree
  COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${GIT_EXECUTABLE} add synthetic.a78 WORKING_DIRECTORY ${hygiene_scan}/tree
  COMMAND_ERROR_IS_FATAL ANY)
set(hygiene_cmd ${CMAKE_COMMAND} -DROOT=${hygiene_scan}/tree -DFIRMWARE=${hygiene_scan}/firmware.txt
  ${hygiene_scan_lists} -P ${hygiene_scripts}/firmware_scan.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME hygiene.firmware_scan.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=synthetic.a78: matches synthetic \\(header stripped\\)"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene TIMEOUT 30)  # provisional, 9 samples
