# Fault prevented: publication accepting another workflow's valid attestation.
cmake_minimum_required(VERSION 3.25)
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/bin" "${WORK}/dist")
file(WRITE "${WORK}/bin/gh" [=[#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$PUBLISH_WORK/calls"
if [ "${PUBLISH_CASE:-}" = unavailable ]; then echo 'simulated HTTP 503' >&2; exit 1; fi
case "$*" in
  --version)
    [ "$PUBLISH_VERSION" != failed ] || exit 1
    [ "$PUBLISH_VERSION" = empty ] || printf '%s\n' "$PUBLISH_VERSION"
    exit 0 ;;
  'attestation verify '*)
    case "$3" in
      "$PUBLISH_WORK"/dist/* | "$PUBLISH_WORK"/temp/publish-verify/download/*) ;;
      *) [ "$PUBLISH_CASE" = tampered ] ;;
    esac
    identity=$PUBLISH_CASE
    case "$identity" in
      download-*)
        if [ -f "$PUBLISH_WORK/download-started" ]; then identity=${identity#download-}; else identity=correct; fi ;;
    esac
    case "$identity" in wrong-signer | prefix-signer)
      case " $* " in
        *" --signer-workflow example/public/.github/workflows/release.yml "*) exit 1 ;;
      esac
      ;; wrong-ref | case-ref)
      case " $* " in *" --source-ref refs/heads/main "*) exit 1 ;; esac ;;
      portable-wrong-signer)
      case "$3" in */muhtari-libretro-emscripten.zip)
        case " $* " in *" --signer-workflow example/public/.github/workflows/release.yml "*) exit 1 ;; esac ;;
      esac ;;
      portable-wrong-ref)
      case "$3" in */muhtari-libretro-emscripten.zip)
        case " $* " in *" --source-ref refs/heads/main "*) exit 1 ;; esac ;;
      esac ;;
    esac ;;
  'release view '*)
    case "$*" in *'--json isDraft'*) echo false ;; *) (cd "$PUBLISH_WORK/dist" && printf '%s\n' *) ;; esac ;;
  'release verify '*) ;;
  'api repos/example/public/releases/7 --jq .draft')
    [ "$PUBLISH_DRAFT" != failed ] || exit 1
    printf '%s\n' "$PUBLISH_DRAFT" ;;
  'api -X PATCH '*) ;;
  'api '*)
    [ "${PUBLISH_DRAFT:-}" != failed ] || exit 1
    i=$(cat "$PUBLISH_WORK/position")
    i=$((i + 1))
    echo "$i" > "$PUBLISH_WORK/position"
    expected=$(cat "$PUBLISH_WORK/request-$i")
    [ "$*" = "$expected" ] || { echo "unexpected API call: $*; expected $expected" >&2; exit 1; }
    cat "$PUBLISH_WORK/response-$i"
    exit "$(cat "$PUBLISH_WORK/exit-$i")" ;;
  *) echo "unexpected transport call: $*" >&2; exit 1 ;;
esac
]=])
file(CHMOD "${WORK}/bin/gh" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
file(WRITE "${WORK}/bin/curl" [=[#!/bin/sh
set -eu
[ "$1 $2 $3 $4" = "-fsSL --retry 3 -o" ] || exit 1
cp "$PUBLISH_WORK/dist/${6##*/}" "$5"
if [ "${PUBLISH_CASE:-}" = download-portable-corrupt ] && [ "${6##*/}" = muhtari-libretro-emscripten.zip ]; then
  printf '\001' >> "$5"
fi
: > "$PUBLISH_WORK/download-started"
]=])
file(CHMOD "${WORK}/bin/curl" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
foreach(component library runner libretro)
  foreach(os linux macos windows)
    foreach(arch x64 arm64)
      file(WRITE "${WORK}/dist/muhtari-${component}-${os}-${arch}.zip" "synthetic asset\n")
    endforeach()
  endforeach()
endforeach()

include("${ROOT}/tests/emscripten/bundle.cmake")
_muhtari_bundle_synthetic_inputs("${ROOT}" "${WORK}/portable-synthetic"
  synthetic_build synthetic_archive synthetic_commit synthetic_notices synthetic_native synthetic_fixture)
file(COPY_FILE "${synthetic_archive}" "${WORK}/dist/muhtari-libretro-emscripten.zip")

file(GLOB assets RELATIVE "${WORK}/dist" "${WORK}/dist/*.zip")
execute_process(COMMAND sh -c [=[
if command -v sha256sum > /dev/null 2>&1; then sha256sum -- "$@"; else shasum -a 256 -- "$@"; fi
]=] sh ${assets}
  WORKING_DIRECTORY "${WORK}/dist" OUTPUT_FILE "${WORK}/dist/SHA256SUMS" COMMAND_ERROR_IS_FATAL ANY)

# Shared response queue exercises production command substitutions and conditional callers.
set(cmd "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:${WORK}/bin"
  "PUBLISH_WORK=${WORK}" "RUNNER_TEMP=${WORK}/temp"
  GITHUB_REPOSITORY=example/public SOURCE_REF=refs/heads/main)

# Model the producer's actual upload, rather than copying unuploaded build files.
file(READ "${ROOT}/.github/workflows/ci.yml" producer_ci)
string(FIND "${producer_ci}" "name: emscripten-libretro-evidence\n" evidence_at)
if(evidence_at LESS 0)
  message(FATAL_ERROR "publish controls: portable evidence upload is missing")
endif()
string(SUBSTRING "${producer_ci}" ${evidence_at} -1 evidence_upload)
string(FIND "${evidence_upload}" "if-no-files-found:" evidence_end)
if(evidence_end LESS 0)
  message(FATAL_ERROR "publish controls: portable evidence upload has no file policy")
endif()
string(SUBSTRING "${evidence_upload}" 0 ${evidence_end} evidence_upload)
string(REGEX MATCHALL "build/emscripten/libretro/[A-Za-z0-9_.-]+" evidence_files "${evidence_upload}")

string(REGEX MATCHALL "build/ci/tests/(emscripten/ym(-worst)?-(native|measurement)[.]json|ym2151/ym2151(-worst)?[.]a78)" ym_evidence_files "${evidence_upload}")
list(LENGTH ym_evidence_files ym_evidence_count)
if(NOT ym_evidence_count EQUAL 6)
  message(FATAL_ERROR "publisher producer must transport the six exact YM evidence files")
endif()

function(publish_make_prepare_fixture destination portable_archive expected_sha evidence_build native reverse)
  file(REMOVE_RECURSE "${destination}")
  file(MAKE_DIRECTORY "${destination}/legs" "${destination}/emscripten-evidence/ci/tests/emscripten"
    "${destination}/emscripten-evidence/emscripten")
  file(GLOB fixture_archives "${WORK}/dist/*.zip")
  if(reverse)
    list(REVERSE fixture_archives)
  endif()
  foreach(archive IN LISTS fixture_archives)
    get_filename_component(name "${archive}" NAME)
    file(COPY_FILE "${archive}" "${destination}/${name}")
  endforeach()
  file(COPY_FILE "${portable_archive}" "${destination}/muhtari-libretro-emscripten.zip")
  file(MAKE_DIRECTORY "${destination}/emscripten-evidence/emscripten/libretro")
  foreach(evidence_file IN LISTS evidence_files)
    string(REPLACE "build/emscripten/" "" evidence_relative "${evidence_file}")
    file(COPY_FILE "${evidence_build}/${evidence_relative}"
      "${destination}/emscripten-evidence/emscripten/${evidence_relative}")
  endforeach()
  file(COPY_FILE "${native}"
    "${destination}/emscripten-evidence/ci/tests/emscripten/native.json")
  file(READ "${evidence_build}/libretro/muhtari_libretro.info" info)
  if(info MATCHES "Supports the selected YM2151 digital profile")
    foreach(path IN LISTS ym_evidence_files)
      string(REPLACE "build/ci/tests/" "" relative "${path}")
      get_filename_component(parent "${destination}/emscripten-evidence/ci/tests/${relative}" DIRECTORY)
      file(MAKE_DIRECTORY "${parent}")
      file(COPY_FILE "${ROOT}/${path}" "${destination}/emscripten-evidence/ci/tests/${relative}")
    endforeach()
  endif()
  foreach(index RANGE 1 6)
    file(MAKE_DIRECTORY "${destination}/legs/${index}")
    file(WRITE "${destination}/legs/${index}/commit.txt" "${expected_sha}\n")
  endforeach()
endfunction()

function(publish_run_prepare destination expected_sha production output_rc output_text)
  set(evidence "${destination}/emscripten-evidence")
  set(build "${evidence}/emscripten")
  set(validate_work "${build}/publish-validation")
  set(archive "${destination}/muhtari-libretro-emscripten.zip")
  set(native "${evidence}/ci/tests/emscripten/native.json")
  set(cli_root "${ROOT}/scripts/..")
  set(checker "${cli_root}/tests/emscripten/bundle.cmake")
  set(fixture "${cli_root}/tests/roms/manifest.txt")
  set(driver "${WORK}/validator-driver.cmake")
  set(notice_lines "")
  if(production)
    file(REMOVE "${driver}")
  else()
    foreach(record IN LISTS synthetic_notices)
      string(APPEND notice_lines "  \"${record}\"\n")
    endforeach()
    file(WRITE "${driver}"
      "include(\"${cli_root}/tests/emscripten/bundle.cmake\")\n"
      "set(notice_digests\n${notice_lines})\n"
      "muhtari_bundle_validate(\"\${ARCHIVE}\" \"\${ROOT}\" \"\${BUILD}\" \"\${WORK}\" \"\${EXPECTED_SHA}\" \"\${notice_digests}\" \"\${NATIVE}\")\n")
  endif()
  file(WRITE "${WORK}/cmake-calls" "")
  execute_process(COMMAND ${CMAKE_COMMAND} -E env
    --modify "PATH=path_list_prepend:${WORK}/bin"
    "PUBLISH_WORK=${WORK}" "PUBLISH_ROOT=${ROOT}" "PUBLISH_REAL_CMAKE=${CMAKE_COMMAND}"
    "PUBLISH_EXPECTED_ROOT=${cli_root}" "PUBLISH_EXPECTED_CHECKER=${checker}"
    "PUBLISH_EXPECTED_BUILD=${build}" "PUBLISH_EXPECTED_WORK=${validate_work}"
    "PUBLISH_EXPECTED_ARCHIVE=${archive}" "PUBLISH_EXPECTED_NATIVE=${native}"
    "PUBLISH_EXPECTED_FIXTURE=${fixture}" "PUBLISH_EXPECTED_SHA=${expected_sha}"
    "PUBLISH_EXPECTED_DIST=${destination}" "PUBLISH_VALIDATOR_DRIVER=${driver}"
    "PUBLISH_PRODUCTION=${production}" "CMAKE=${WORK}/bin/cmake" "EXPECTED_SHA=${expected_sha}"
    sh "${ROOT}/scripts/publish.sh" prepare "${destination}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
  set(${output_rc} "${rc}" PARENT_SCOPE)
  set(${output_text} "${out}${err}" PARENT_SCOPE)
endfunction()

# A missing portable archive must fail before the publisher can leave a fresh
# checksum file behind. Keep the native set and six build-leg receipts intact.
set(prepare_missing "${WORK}/prepare-missing")
file(MAKE_DIRECTORY "${prepare_missing}/native")
file(WRITE "${WORK}/bin/cmake" [=[#!/bin/sh
set -eu
if [ "$#" -eq 6 ] && [ "${1#-DZIPS=}" != "$1" ]; then
  [ "$2" = "-DFIRMWARE=$PUBLISH_EXPECTED_ROOT/tests/release/firmware.txt" ] || exit 91
  [ "$3" = "-DEXPECTED=$PUBLISH_EXPECTED_ROOT/tests/built/expected.txt" ] || exit 91
  [ "$4" = "-DCARTDB=$PUBLISH_EXPECTED_ROOT/src/cartdb.c" ] || exit 91
  [ "$5" = "-P" ] || exit 91
  [ "$6" = "$PUBLISH_EXPECTED_ROOT/tests/cmake/firmware_scan.cmake" ] || exit 91
  actual=$(printf '%s\n' "${1#-DZIPS=}" | tr ';' '\n' | sed "s|^$PUBLISH_EXPECTED_DIST/||" | LC_ALL=C sort)
  expected=$(
    for c in library runner libretro; do
      for o in linux macos windows; do
        for a in x64 arm64; do printf 'muhtari-%s-%s-%s.zip\n' "$c" "$o" "$a"; done
      done
    done
    echo muhtari-libretro-emscripten.zip
  )
  expected=$(printf '%s\n' "$expected" | LC_ALL=C sort)
  [ "$actual" = "$expected" ] || { echo "firmware scan did not receive the exact 19 ZIP set" >&2; exit 92; }
  printf 'firmware\n%s\n' "$actual" >> "$PUBLISH_WORK/cmake-calls"
  exit 0
fi
if [ "$#" -eq 10 ]; then
  [ "$1" = "-DMODE=validate" ] || exit 93
  [ "$2" = "-DROOT=$PUBLISH_EXPECTED_ROOT" ] || exit 93
  [ "$3" = "-DBUILD=$PUBLISH_EXPECTED_BUILD" ] || exit 93
  [ "$4" = "-DWORK=$PUBLISH_EXPECTED_WORK" ] || exit 93
  [ "$5" = "-DEXPECTED_SHA=$PUBLISH_EXPECTED_SHA" ] || exit 93
  [ "$6" = "-DARCHIVE=$PUBLISH_EXPECTED_ARCHIVE" ] || exit 93
  [ "$7" = "-DNATIVE=$PUBLISH_EXPECTED_NATIVE" ] || exit 93
  [ "$8" = "-DFIXTURE=$PUBLISH_EXPECTED_FIXTURE" ] || exit 93
  [ "$9" = "-P" ] || exit 93
  [ "${10}" = "$PUBLISH_EXPECTED_CHECKER" ] || exit 93
  printf 'validate\n%s\n%s\n' "$5" "$6" >> "$PUBLISH_WORK/cmake-calls"
  if [ "$PUBLISH_PRODUCTION" = 1 ]; then exec "$PUBLISH_REAL_CMAKE" "$@"; fi
  exec "$PUBLISH_REAL_CMAKE" "-DARCHIVE=${6#-DARCHIVE=}" \
    "-DROOT=${2#-DROOT=}" "-DBUILD=${3#-DBUILD=}" "-DWORK=${4#-DWORK=}" \
    "-DEXPECTED_SHA=${5#-DEXPECTED_SHA=}" "-DNATIVE=${7#-DNATIVE=}" \
    -P "$PUBLISH_VALIDATOR_DRIVER"
fi
echo "unexpected cmake call: $*" >&2
exit 94
]=])
file(CHMOD "${WORK}/bin/cmake" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
file(GLOB native_archives "${WORK}/dist/*.zip")
foreach(archive IN LISTS native_archives)
  get_filename_component(name "${archive}" NAME)
  if(name STREQUAL "muhtari-libretro-emscripten.zip")
    continue()
  endif()
  file(COPY_FILE "${archive}" "${prepare_missing}/${name}")
endforeach()
foreach(index RANGE 1 6)
  file(MAKE_DIRECTORY "${prepare_missing}/native/${index}")
  file(WRITE "${prepare_missing}/native/${index}/commit.txt" "fixture-sha\n")
endforeach()
execute_process(COMMAND ${CMAKE_COMMAND} -E env
  --modify "PATH=path_list_prepend:${WORK}/bin"
  "PUBLISH_WORK=${WORK}" "PUBLISH_ROOT=${ROOT}" "CMAKE=${WORK}/bin/cmake"
  EXPECTED_SHA=fixture-sha sh "${ROOT}/scripts/publish.sh" prepare "${prepare_missing}"
  RESULT_VARIABLE prepare_rc OUTPUT_VARIABLE prepare_out ERROR_VARIABLE prepare_err)
if(prepare_rc STREQUAL "0"
   OR NOT "${prepare_out}${prepare_err}" MATCHES "missing muhtari-libretro-emscripten[.]zip"
   OR EXISTS "${prepare_missing}/SHA256SUMS")
  message(FATAL_ERROR "publish preparation must reject an omitted portable ZIP before writing SHA256SUMS; exit ${prepare_rc}: ${prepare_out}${prepare_err}")
endif()
message(STATUS "publish: missing portable ZIP is rejected before checksums")

set(prepare_valid "${WORK}/prepare-valid")
publish_make_prepare_fixture("${prepare_valid}" "${synthetic_archive}" "${synthetic_commit}"
  "${synthetic_build}" "${synthetic_native}" FALSE)
publish_run_prepare("${prepare_valid}" "${synthetic_commit}" 0 prepare_rc prepare_out)
if(NOT prepare_rc STREQUAL "0" OR NOT prepare_out MATCHES "19 archives from ${synthetic_commit}")
  message(FATAL_ERROR "publish preparation rejected the valid synthetic source-bound set: exit ${prepare_rc}: ${prepare_out}")
endif()
file(READ "${WORK}/cmake-calls" prepare_calls)
string(FIND "${prepare_calls}" "validate\n" validate_index)
string(FIND "${prepare_calls}" "firmware\n" firmware_index)
if(validate_index LESS 0 OR firmware_index LESS 0 OR NOT validate_index LESS firmware_index)
  message(FATAL_ERROR "portable preflight must precede the firmware scan extraction: ${prepare_calls}")
endif()
file(STRINGS "${prepare_valid}/SHA256SUMS" checksum_rows)
list(LENGTH checksum_rows checksum_count)
if(NOT checksum_count EQUAL 19)
  message(FATAL_ERROR "publish preparation wrote ${checksum_count} checksums instead of 19")
endif()
file(GLOB published_assets "${prepare_valid}/*.zip" "${prepare_valid}/SHA256SUMS")
list(LENGTH published_assets published_asset_count)
if(NOT published_asset_count EQUAL 20)
  message(FATAL_ERROR "prepared release set contains ${published_asset_count} assets instead of 20")
endif()
file(READ "${WORK}/cmake-calls" validator_calls)
if(NOT validator_calls MATCHES "firmware\n" OR NOT validator_calls MATCHES "validate\n-DEXPECTED_SHA=${synthetic_commit}")
  message(FATAL_ERROR "publish preparation did not run the sorted 19-ZIP scan and exact bundle validator: ${validator_calls}")
endif()
file(READ "${prepare_valid}/SHA256SUMS" first_sums)
file(WRITE "${prepare_valid}/SHA256SUMS" "stale checksum receipt\n")
publish_run_prepare("${prepare_valid}" "${synthetic_commit}" 0 prepare_rc prepare_out)
file(READ "${prepare_valid}/SHA256SUMS" repeated_sums)
if(NOT prepare_rc STREQUAL "0" OR NOT repeated_sums STREQUAL first_sums)
  message(FATAL_ERROR "repeated preparation did not recompute the stable 19-checksum set: ${prepare_rc}: ${prepare_out}")
endif()
set(prepare_reordered "${WORK}/prepare-reordered")
publish_make_prepare_fixture("${prepare_reordered}" "${synthetic_archive}" "${synthetic_commit}"
  "${synthetic_build}" "${synthetic_native}" TRUE)
publish_run_prepare("${prepare_reordered}" "${synthetic_commit}" 0 prepare_rc prepare_out)
file(READ "${prepare_reordered}/SHA256SUMS" reordered_sums)
if(NOT prepare_rc STREQUAL "0" OR NOT reordered_sums STREQUAL first_sums)
  message(FATAL_ERROR "asset creation order changed the exact checksum receipt: ${prepare_rc}: ${prepare_out}")
endif()
message(STATUS "publish: exact 19 ZIPs, repeated preparation and reordered input controls passed")

set(strict_work "${synthetic_build}/strict-validation")
execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=validate "-DROOT=${ROOT}"
  "-DBUILD=${synthetic_build}" "-DWORK=${strict_work}" "-DEXPECTED_SHA=${synthetic_commit}"
  "-DARCHIVE=${synthetic_archive}" "-DNATIVE=${synthetic_native}"
  "-DFIXTURE=${synthetic_fixture}" -P "${ROOT}/tests/emscripten/bundle.cmake"
  RESULT_VARIABLE strict_rc OUTPUT_VARIABLE strict_out ERROR_VARIABLE strict_err)
if(strict_rc STREQUAL "0" OR NOT "${strict_out}${strict_err}" MATCHES "runtime notice digest mismatch")
  message(FATAL_ERROR "unshimmed production bundle validation must reject synthetic notices: ${strict_rc}: ${strict_out}${strict_err}")
endif()

_muhtari_bundle_members("${ROOT}" bundle_members)
set(wrong_commit "${WORK}/wrong-commit.zip")
set(damaged_archive "${WORK}/damaged-portable.zip")
set(wrong_commit_stage "${WORK}/wrong-commit-stage")
file(ARCHIVE_EXTRACT INPUT "${synthetic_archive}" DESTINATION "${wrong_commit_stage}")
string(REPEAT "c" 40 wrong_commit_sha)
_muhtari_bundle_control_metadata("${wrong_commit_stage}/muhtari-bundle.txt" "${wrong_commit_sha}"
  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "${_muhtari_bundle_emcc}")
file(ARCHIVE_CREATE OUTPUT "${wrong_commit}" PATHS ${bundle_members}
  FORMAT zip WORKING_DIRECTORY "${wrong_commit_stage}")
set(damaged_stage "${WORK}/damaged-stage")
file(ARCHIVE_EXTRACT INPUT "${synthetic_archive}" DESTINATION "${damaged_stage}")
file(APPEND "${damaged_stage}/cores/muhtari_libretro.wasm" "damaged\n")
file(ARCHIVE_CREATE OUTPUT "${damaged_archive}" PATHS ${bundle_members}
  FORMAT zip WORKING_DIRECTORY "${damaged_stage}")

foreach(fault missing extra empty wrong_commit damaged receipt_missing receipt_extra)
  set(fault_dir "${WORK}/prepare-${fault}")
  publish_make_prepare_fixture("${fault_dir}" "${synthetic_archive}" "${synthetic_commit}"
    "${synthetic_build}" "${synthetic_native}" FALSE)
  file(WRITE "${fault_dir}/SHA256SUMS" "stale checksum receipt\n")
  if(fault STREQUAL "missing")
    file(REMOVE "${fault_dir}/muhtari-libretro-emscripten.zip")
    set(fault_text "missing muhtari-libretro-emscripten[.]zip")
  elseif(fault STREQUAL "extra")
    file(WRITE "${fault_dir}/unexpected.zip" "unexpected\n")
    set(fault_text "unexpected unexpected[.]zip")
  elseif(fault STREQUAL "empty")
    file(WRITE "${fault_dir}/muhtari-libretro-emscripten.zip" "")
    set(fault_text "bundle archive is empty")
  elseif(fault STREQUAL "receipt_missing")
    file(REMOVE "${fault_dir}/legs/6/commit.txt")
    set(fault_text "5 commit[.]txt files, expected 6")
  elseif(fault STREQUAL "receipt_extra")
    file(MAKE_DIRECTORY "${fault_dir}/legs/7")
    file(WRITE "${fault_dir}/legs/7/commit.txt" "${synthetic_commit}\n")
    set(fault_text "7 commit[.]txt files, expected 6")
  elseif(fault STREQUAL "wrong_commit")
    file(COPY_FILE "${wrong_commit}" "${fault_dir}/muhtari-libretro-emscripten.zip")
    set(fault_text "bundle commit does not match EXPECTED_SHA")
  else()
    file(COPY_FILE "${damaged_archive}" "${fault_dir}/muhtari-libretro-emscripten.zip")
    set(fault_text "SHA256SUMS mismatch for cores/muhtari_libretro[.]wasm")
  endif()
  publish_run_prepare("${fault_dir}" "${synthetic_commit}" 0 fault_rc fault_out)
  if(fault_rc STREQUAL "0" OR NOT fault_out MATCHES "${fault_text}"
     OR EXISTS "${fault_dir}/SHA256SUMS")
    message(FATAL_ERROR "publish ${fault} control failed to reject before fresh checksums: ${fault_rc}: ${fault_out}")
  endif()
  file(READ "${WORK}/cmake-calls" fault_calls)
  if(fault MATCHES "^(empty|wrong_commit|damaged)$" AND NOT fault_calls MATCHES "validate\n")
    message(FATAL_ERROR "publish ${fault} did not reach the shared bundle validator: ${fault_calls}")
  elseif(fault MATCHES "^(missing|extra|receipt_missing|receipt_extra)$" AND fault_calls MATCHES "validate\n")
    message(FATAL_ERROR "publish ${fault} should fail inventory before bundle validation: ${fault_calls}")
  endif()
endforeach()
message(STATUS "publish: omitted, extra, empty, wrong-commit and damaged portable archive controls passed")

if(DEFINED PRODUCTION_ARCHIVE AND NOT PRODUCTION_ARCHIVE STREQUAL "")
  if(NOT EXISTS "${PRODUCTION_ARCHIVE}" OR EXPECTED_SHA STREQUAL "")
    message(FATAL_ERROR "publish production controls require the current ZIP and EXPECTED_SHA")
  endif()
  get_filename_component(production_build "${PRODUCTION_ARCHIVE}" DIRECTORY)
  set(production_native "${ROOT}/build/ci/tests/emscripten/native.json")
  if(NOT EXISTS "${production_native}")
    message(FATAL_ERROR "publish production controls require the producer's native source identity")
  endif()
  set(production_fixture "${WORK}/production-valid")
  publish_make_prepare_fixture("${production_fixture}" "${PRODUCTION_ARCHIVE}" "${EXPECTED_SHA}"
    "${production_build}" "${production_native}" FALSE)
  publish_run_prepare("${production_fixture}" "${EXPECTED_SHA}" 1 production_rc production_out)
  if(NOT production_rc STREQUAL "0" OR NOT production_out MATCHES "emscripten[.]bundle: validated")
    message(FATAL_ERROR "publisher rejected the real SDK ZIP or intercepted validation: ${production_rc}: ${production_out}")
  endif()
  if(EXISTS "${WORK}/validator-driver.cmake")
    message(FATAL_ERROR "production publisher controls must delegate validation without the synthetic notice driver")
  endif()
  file(STRINGS "${production_fixture}/SHA256SUMS" production_rows)
  list(LENGTH production_rows production_count)
  if(NOT production_count EQUAL 19)
    message(FATAL_ERROR "production publisher wrote ${production_count} checksums instead of 19")
  endif()

  set(missing_ym "${production_fixture}/emscripten-evidence/ci/tests/emscripten/ym-worst-native.json")
  file(RENAME "${missing_ym}" "${missing_ym}.held")
  publish_run_prepare("${production_fixture}" "${EXPECTED_SHA}" 1 missing_rc missing_out)
  if(missing_rc STREQUAL "0" OR NOT missing_out MATCHES "transported YM receipt is missing" OR
      EXISTS "${production_fixture}/SHA256SUMS")
    message(FATAL_ERROR "publisher accepted missing transported YM evidence")
  endif()
  file(RENAME "${missing_ym}.held" "${missing_ym}")

  muhtari_bundle_qualified_notice_digests(qualified_notices)
  foreach(record IN LISTS qualified_notices)
    string(FIND "${record}" "=" separator)
    string(SUBSTRING "${record}" 0 ${separator} member)
    string(REPLACE "/" "-" notice_name "${member}")
    set(stage "${WORK}/production-notice-${notice_name}")
    file(REMOVE_RECURSE "${stage}")
    file(ARCHIVE_EXTRACT INPUT "${PRODUCTION_ARCHIVE}" DESTINATION "${stage}")
    file(APPEND "${stage}/${member}" "notice mutation\n")
    set(changed_archive "${WORK}/production-notice-${notice_name}.zip")
    file(ARCHIVE_CREATE OUTPUT "${changed_archive}" PATHS ${bundle_members}
      FORMAT zip WORKING_DIRECTORY "${stage}")
    set(fault_dir "${WORK}/production-notice-check-${notice_name}")
    publish_make_prepare_fixture("${fault_dir}" "${changed_archive}" "${EXPECTED_SHA}"
      "${production_build}" "${production_native}" FALSE)
    file(WRITE "${fault_dir}/SHA256SUMS" "stale checksum receipt\n")
    publish_run_prepare("${fault_dir}" "${EXPECTED_SHA}" 1 production_fault_rc production_fault_out)
    string(REGEX REPLACE "[ \t\r\n]+" " " normalized_production_fault_out "${production_fault_out}")
    if(production_fault_rc STREQUAL "0"
       OR NOT normalized_production_fault_out MATCHES "runtime notice digest mismatch: ${member}"
       OR EXISTS "${fault_dir}/SHA256SUMS")
      message(FATAL_ERROR "publisher did not reject mutated qualified notice ${member}: ${production_fault_rc}: ${production_fault_out}")
    endif()
    file(READ "${WORK}/cmake-calls" production_calls)
    if(NOT production_calls MATCHES "validate\n-DEXPECTED_SHA=${EXPECTED_SHA}")
      message(FATAL_ERROR "production notice mutation bypassed the real bundle validator: ${member}: ${production_calls}")
    endif()
  endforeach()
  message(STATUS "publish: real SDK archive and all five qualified-notice mutation controls passed")
endif()

file(WRITE "${WORK}/calls" "")
execute_process(COMMAND ${cmd} PUBLISH_CASE=unavailable sh "${ROOT}/scripts/publish.sh" rehearsal-delete rehearsal-fixture
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
file(READ "${WORK}/calls" calls)
if(NOT rc STREQUAL "1" OR NOT out MATCHES "release listing failed" OR calls MATCHES "DELETE|release delete")
  message(FATAL_ERROR "publish cleanup assertion: failed listing must stop before deletion; exit ${rc}: ${out}: ${calls}")
endif()
set(endpoint "repos/example/public")
set(request_list "api --paginate --slurp ${endpoint}/releases?per_page=100")
set(request_create "api --include -X POST ${endpoint}/releases -f tag_name=rehearsal-fixture -f target_commitish=fixture-sha -f name=rehearsal-fixture -f body=Rehearsal; deleted at the end of this run. -F draft=true -F prerelease=true -f make_latest=false")
foreach(resource release tag)
  set(path_release releases/7)
  set(path_tag git/ref/tags/rehearsal-fixture)
  set(delete_release releases/7)
  set(delete_tag git/refs/tags/rehearsal-fixture)
  set(request_${resource} "api --include -X GET ${endpoint}/${path_${resource}}")
  set(request_delete_${resource} "api --include -X DELETE ${endpoint}/${delete_${resource}}")
endforeach()
set(body_empty "[[]]")
set(body_present "[[{\"id\":7,\"tag_name\":\"rehearsal-fixture\"}]]")
set(body_later "[[],[{\"id\":7,\"tag_name\":\"rehearsal-fixture\"}],[]]")
set(body_release "{\"id\":7,\"tag_name\":\"rehearsal-fixture\"}")
set(body_create "{\"id\":7,\"tag_name\":\"rehearsal-fixture\",\"draft\":true,\"prerelease\":true}")
set(body_tag "{\"ref\":\"refs/tags/rehearsal-fixture\"}")
set(create_sequence "list_empty;create")
set(absent_sequence "list_empty;list_empty;tag_absent;list_empty;tag_absent")
set(absent_known_sequence "list_empty;release_absent;release_absent;list_empty;tag_absent;list_empty;release_absent;tag_absent")
set(delete_sequence "list_present;release;delete_release;release_absent;list_empty;tag;delete_tag;tag_absent;list_empty;release_absent;tag_absent")
set(rows "create|0||0||" "delete|0||0||" "delete|3|crlf|0||" "delete|7|crlf|0||"
  "delete|3|missing-separator|1|invalid HTTP response|git/ref" "delete|3|bad-header|1|invalid HTTP response|git/ref"
  "delete|3|contradiction|1|HTTP command/status disagreement|git/ref" "create|1|later|0||POST" "create|1|present|0||POST")
foreach(code 401 403 404 429 500 503 transport)
  list(APPEND rows "create|1|fail-${code}|1|release listing failed|POST" "delete|1|fail-${code}|1|release listing failed|DELETE")
endforeach()
foreach(body "" "not JSON" "{}" "[]" "[{}]" "[[null]]" "[[{}]]" "[[{\"id\":7,\"tag_name\":null}]]")
  list(APPEND rows "create|1|json-${body}|1|invalid release listing|POST")
endforeach()
foreach(id null "\"7\"" 0 -1 1.5)
  list(APPEND rows "create|1|json-[[{\"id\":${id},\"tag_name\":\"rehearsal-fixture\"}]]|1|invalid release listing|POST"
    "create|2|json-{\"id\":${id},\"tag_name\":\"rehearsal-fixture\",\"draft\":true,\"prerelease\":true}|1|invalid create response|PATCH")
endforeach()
list(APPEND rows "create|1|json-[[{\"id\":7,\"tag_name\":\"rehearsal-fixture\"},{\"id\":7,\"tag_name\":\"rehearsal-fixture\"}]]|1|invalid release listing|POST"
  "create|1|partial|1|release listing failed|POST" "create|2|crlf|0||" "create|2|missing-status|1|invalid HTTP response|PATCH"
  "create|2|bad-status|1|invalid HTTP response|PATCH" "create|2|false-status|1|invalid HTTP response|PATCH"
  "create|2|contradiction|1|HTTP command/status disagreement|PATCH" "create|2|http-200|1|create status 200|PATCH"
  "create|2|json-{}|1|invalid create response|PATCH" "create|2|json-{\"id\":7,\"tag_name\":\"other\",\"draft\":true,\"prerelease\":true}|1|invalid create response|PATCH"
  "create|2|json-{\"id\":7,\"tag_name\":\"rehearsal-fixture\",\"draft\":false,\"prerelease\":true}|1|invalid create response|PATCH"
  "delete|2|json-{\"id\":7,\"tag_name\":\"other\"}|1|release identity|DELETE" "delete|2|http-404|1|listed release absent|DELETE"
  "delete|4|http-200|1|release still exists|git/ref" "delete|5|present|1|release still listed|git/ref"
  "delete|5|fail-503|1|release listing failed|git/ref" "delete|6|bad-status|1|invalid HTTP response|DELETE.*git/refs"
  "delete|7|false-status|1|invalid HTTP response|PATCH" "delete|8|missing-separator|1|invalid HTTP response|PATCH" "delete|6|json-{}|1|tag identity|DELETE.*git/refs"
  "delete|8|http-200|1|tag still exists|PATCH" "delete|9|present|1|release still listed|PATCH"
  "delete|10|http-200|1|release still exists|PATCH" "delete|11|http-200|1|tag still exists|PATCH")
list(APPEND rows "absent|0||0||" "absent_known|0||0|||7" "delete|0||0|||7"
  "delete|1|empty|0|||7" "delete|0||1|release identity conflict|DELETE|8" "delete|3|http-404|0||"
  "create|1|json-[[],[]]|0||" "create|2|json-not JSON|1|invalid create response|PATCH"
  "create|2|json-{\"id\":7,\"tag_name\":\"rehearsal-fixture\",\"draft\":true,\"prerelease\":false}|1|invalid create response|PATCH")
foreach(id null "\"7\"" 0 -1 1.5)
  list(APPEND rows "delete|0||1|invalid release ID|DELETE|${id}")
endforeach()
foreach(code 401 403 429 500 503 transport)
  foreach(index 2 3 4 6 7 8 9 10 11)
    set(cause_2 "release GET status")
    set(cause_3 "DELETE status")
    set(cause_4 "release still exists")
    set(cause_6 "tag still exists")
    set(cause_7 "DELETE status")
    set(cause_8 "tag still exists")
    set(cause_9 "release listing failed")
    set(cause_10 "release still exists")
    set(cause_11 "tag still exists")
    set(cause "${cause_${index}}")
    if(code STREQUAL "transport" AND NOT index EQUAL 9)
      set(cause "invalid HTTP response")
    endif()
    set(forbidden "PATCH")
    if(index LESS 6)
      set(forbidden "git/ref")
    endif()
    list(APPEND rows "delete|${index}|http-${code}|1|${cause}|${forbidden}")
  endforeach()
  set(cause "create status")
  if(code STREQUAL "transport")
    set(cause "invalid HTTP response")
  endif()
  list(APPEND rows "create|2|http-${code}|1|${cause}|PATCH")
endforeach()
foreach(code 409 422)
  list(APPEND rows "delete|7|http-${code}|1|DELETE status ${code}|PATCH")
endforeach()
set(lifecycle [=[
if id=$(sh "$1/scripts/publish.sh" "$2" rehearsal-fixture ${3:+"$3"}); then printf '%s\n' "$id"; else exit 1; fi
]=])
foreach(row IN LISTS rows)
  string(REPLACE "|" ";" fields "${row}")
  list(GET fields 0 operation)
  list(GET fields 1 changed)
  list(GET fields 2 replacement)
  list(GET fields 3 expected_rc)
  list(GET fields 4 diagnostic)
  list(GET fields 5 forbidden)
  list(APPEND fields "")
  list(GET fields 6 supplied)
  string(REGEX REPLACE "^absent.*" "delete" entry "${operation}")
  set(i 0)
  foreach(step IN LISTS ${operation}_sequence)
    math(EXPR i "${i}+1")
    string(REGEX REPLACE "_(empty|present|absent)$" "" request "${step}")
    string(REGEX REPLACE "^list_" "" body "${step}")
    set(status 200)
    set(rc 0)
    if(step STREQUAL "create")
      set(status 201)
    elseif(step MATCHES "^delete_")
      set(status 204)
    elseif(step MATCHES "_absent$")
      set(status 404)
      set(rc 1)
    endif()
    set(body "${body_${body}}")
    if(i EQUAL changed)
      if(replacement MATCHES "^json-(.*)$")
        set(body "${CMAKE_MATCH_1}")
      elseif(replacement MATCHES "^(empty|present|later)$")
        set(body "${body_${replacement}}")
      elseif(replacement MATCHES "^(fail|http)-([0-9]+|transport)$" OR replacement STREQUAL "partial")
        set(status "${CMAKE_MATCH_2}")
        set(rc 1)
        if(status MATCHES "^2")
          set(rc 0)
        endif()
      endif()
    endif()
    set(response "HTTP/2.0 ${status} Status\nContent-Type: application/json\n\n${body}")
    if(request STREQUAL "list")
      set(response "${body}")
    elseif(i EQUAL changed)
      if(replacement STREQUAL "crlf")
        string(REPLACE "\n" "\r\n" response "${response}")
      elseif(replacement MATCHES "^(missing-status|false-status)$")
        set(response "{\"message\":\"HTTP/2.0 404 Not Found\"}")
      elseif(replacement STREQUAL "missing-separator")
        set(response "HTTP/2.0 204 Status\nContent-Type: application/json\n")
      elseif(replacement STREQUAL "bad-header")
        set(response "HTTP/2.0 204 Status\ninvalid header\n\n")
      elseif(replacement STREQUAL "bad-status")
        set(response "HTTP/2.0 xyz Invalid\n\n{}")
      elseif(replacement STREQUAL "contradiction")
        set(rc 1)
      endif()
    endif()
    file(WRITE "${WORK}/request-${i}" "${request_${request}}")
    file(WRITE "${WORK}/response-${i}" "${response}")
    file(WRITE "${WORK}/exit-${i}" "${rc}")
  endforeach()
  file(WRITE "${WORK}/position" "0")
  file(WRITE "${WORK}/calls" "")
  execute_process(COMMAND ${cmd} EXPECTED_SHA=fixture-sha sh -c "${lifecycle}" sh "${ROOT}" "rehearsal-${entry}" "${supplied}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
  file(READ "${WORK}/calls" calls)
  if(NOT rc STREQUAL expected_rc OR (expected_rc STREQUAL "1" AND NOT out MATCHES "${diagnostic}") OR (NOT forbidden STREQUAL "" AND calls MATCHES "${forbidden}"))
    message(FATAL_ERROR "publish lifecycle assertion ${row}: exit ${rc}: ${out}: ${calls}")
  endif()
endforeach()
# Execute the same short-circuit boundary as consecutive successful workflow steps.
set(publish [=[
script=$1/scripts/publish.sh
if [ "$PUBLISH_ENTRY" = verify ]; then set -- verify rehearsal-fixture; else set -- attested; fi
sh "$script" "$@" "$PUBLISH_WORK/dist" &&
gh api -X PATCH repos/example/public/releases/7 -F draft=false
]=])
foreach(case correct tampered old wrong-signer prefix-signer wrong-ref case-ref failed empty partial nonnumeric unknown prerelease patch minor major
  portable-wrong-signer portable-wrong-ref download-wrong-signer download-prefix-signer
  download-wrong-ref download-case-ref download-portable-wrong-signer download-portable-wrong-ref
  download-portable-corrupt)
  set(version "gh version 2.102.0 (2026-09-30)")
  set(regex "attestation does not verify")
  if(case STREQUAL "old")
    set(version "gh version 2.101.0 (2026-09-15)")
  elseif(case MATCHES "^(failed|empty|unknown)$")
    set(version "${case}")
  elseif(case STREQUAL "partial")
    set(version "gh version 2.102 (2026-09-30)")
  elseif(case STREQUAL "nonnumeric")
    set(version "gh version 2.x.0 (2026-09-30)")
  elseif(case STREQUAL "prerelease")
    set(version "gh version 2.102.0-rc1 (2026-09-30)")
  elseif(case MATCHES "^(patch|minor|major)$")
    set(versions_patch 2.102.1)
    set(versions_minor 2.103.0)
    set(versions_major 3.0.0)
    set(version "gh version ${versions_${case}} (2026-09-30)")
  endif()
  if(case MATCHES "^(old|failed|empty|partial|nonnumeric|unknown|prerelease)$")
    set(regex "supported stable gh >=2.102.0 required")
  elseif(case STREQUAL "tampered")
    set(regex "copy with its last byte flipped verifies")
  endif()
  foreach(entry attested verify)
    if(case MATCHES "^download-" AND entry STREQUAL "attested")
      continue()
    endif()
    if(case MATCHES "^download-")
      set(regex "downloaded .* does not verify")
    endif()
    if(case STREQUAL "download-portable-corrupt")
      set(regex "a downloaded zip differs")
    endif()
    file(REMOVE "${WORK}/download-started")
  file(WRITE "${WORK}/calls" "")
  execute_process(COMMAND ${cmd} "PUBLISH_CASE=${case}" "PUBLISH_VERSION=${version}" "PUBLISH_ENTRY=${entry}" sh -c "${publish}" sh "${ROOT}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
  file(READ "${WORK}/calls" calls)
  if(case MATCHES "portable" AND NOT calls MATCHES "muhtari-libretro-emscripten[.]zip")
    message(FATAL_ERROR "publish assertion: ${case} did not visit the portable ZIP: ${calls}")
  endif()
  if(case MATCHES "^(correct|patch|minor|major)$")
    if(NOT rc STREQUAL "0" OR NOT calls MATCHES "api -X PATCH")
      message(FATAL_ERROR "publish assertion: correct signer/ref must reach publication: ${rc}: ${out}")
    endif()
  else()
    if(NOT rc STREQUAL "1" OR calls MATCHES "api -X PATCH")
      message(FATAL_ERROR "publish assertion: ${case} must stop before publication; got exit ${rc}: ${calls}")
    endif()
    if(NOT out MATCHES "${regex}" OR (regex MATCHES "supported stable" AND calls MATCHES "attestation verify"))
      message(FATAL_ERROR "publish assertion: ${case}/${entry} wrong cause or forbidden call: ${out}: ${calls}")
    endif()
  endif()
  message(STATUS "publish: ${case} control passed")
  endforeach()
endforeach()

# Both Ubuntu attestation jobs must consume the verified pin before using PATH.
file(READ "${ROOT}/.github/workflows/release.yml" workflow)
# Run actual workflow output blocks; failed lookups must emit no success output.
foreach(output draft state)
  string(REGEX MATCH "- id: ${output}\n        run: ([^\n]*\n)(          [^\n]*\n)*" block "${workflow}")
  string(REGEX REPLACE "^.*run: " "" block "${block}")
  string(REGEX REPLACE "^[|]\n" "" block "${block}")
  string(REPLACE "\n          " "\n" block "\n${block}")
  if(block STREQUAL "\n")
    message(FATAL_ERROR "publish caller assertion: missing ${output} output block")
  endif()
  foreach(value true false failed null)
    if(output STREQUAL "draft" AND value STREQUAL "null")
      continue()
    endif()
    file(WRITE "${WORK}/position" "0")
    file(WRITE "${WORK}/output" "")
    execute_process(COMMAND ${cmd} "PUBLISH_DRAFT=${value}" GITHUB_OUTPUT=${WORK}/output
      REHEARSAL_TAG=rehearsal-fixture ID=7 EXPECTED_SHA=fixture-sha
      sh -e -c "${block}" WORKING_DIRECTORY "${ROOT}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
    file(READ "${WORK}/output" emitted)
    if(value MATCHES "^(failed|null)$")
      if(rc STREQUAL "0" OR NOT emitted STREQUAL "")
        message(FATAL_ERROR "publish caller assertion: ${output}/${value} hid failure or emitted success: ${rc}: ${emitted}")
      endif()
    elseif(NOT rc STREQUAL "0" OR (output STREQUAL "state" AND NOT emitted STREQUAL "draft=${value}\n") OR
      (output STREQUAL "draft" AND NOT emitted STREQUAL "id=7\n"))
      message(FATAL_ERROR "publish caller assertion: ${output}/${value} did not emit its validated output: ${rc}: ${emitted}: ${out}")
    endif()
  endforeach()
endforeach()
foreach(job rehearsal publish)
  string(REGEX MATCH "\n  ${job}:\n((    [^\n]*\n|\n)*)" job_block "${workflow}")
  if(NOT job_block MATCHES "sh scripts/fetch-pin.sh gh-linux-x86_64 [^\n]+\n +echo [^\n]+/bin[^\n]+GITHUB_PATH[^\n]*\n +[^\n]+/bin/gh[^\n]+--version")
    message(FATAL_ERROR "publish provisioning assertion: ${job} must fetch and consume the verified CLI before attestation")
  endif()
endforeach()
