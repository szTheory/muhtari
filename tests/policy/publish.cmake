# Fault prevented: publication accepting another workflow's valid attestation.
cmake_minimum_required(VERSION 3.25)
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/bin" "${WORK}/dist")
file(WRITE "${WORK}/bin/gh" [=[#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$PUBLISH_WORK/calls"
case "$*" in
  'attestation verify '*)
    case "$3" in
      "$PUBLISH_WORK"/dist/*) ;;
      *) [ "$PUBLISH_CASE" = tampered ] ;;
    esac
    if [ "$PUBLISH_CASE" = wrong-signer ]; then
      case " $* " in
        *" --signer-workflow example/public/.github/workflows/release.yml "*) exit 1 ;;
      esac
    fi ;;
  'api -X PATCH '*) ;;
  *) echo "unexpected transport call: $*" >&2; exit 1 ;;
esac
]=])
file(CHMOD "${WORK}/bin/gh" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
foreach(component library runner libretro)
  foreach(os linux macos windows)
    foreach(arch x64 arm64)
      file(WRITE "${WORK}/dist/muhtari-${component}-${os}-${arch}.zip" "synthetic asset\n")
    endforeach()
  endforeach()
endforeach()
# Execute the same short-circuit boundary as consecutive successful workflow steps.
set(publish [=[
sh "$1/scripts/publish.sh" attested "$PUBLISH_WORK/dist" &&
gh api -X PATCH repos/example/public/releases/7 -F draft=false
]=])
set(cmd "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:${WORK}/bin"
  "PUBLISH_WORK=${WORK}" "RUNNER_TEMP=${WORK}/temp"
  GITHUB_REPOSITORY=example/public SOURCE_REF=refs/heads/main)
foreach(case correct tampered wrong-signer)
  file(WRITE "${WORK}/calls" "")
  execute_process(COMMAND ${cmd} "PUBLISH_CASE=${case}" sh -c "${publish}" sh "${ROOT}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
  file(READ "${WORK}/calls" calls)
  if(case STREQUAL "correct")
    if(NOT rc STREQUAL "0" OR NOT calls MATCHES "api -X PATCH")
      message(FATAL_ERROR "publish assertion: correct signer/ref must reach publication: ${rc}: ${out}")
    endif()
  else()
    if(NOT rc STREQUAL "1" OR calls MATCHES "api -X PATCH")
      message(FATAL_ERROR "publish assertion: ${case} must stop before publication; got exit ${rc}: ${calls}")
    endif()
    if(case STREQUAL "tampered" AND NOT out MATCHES "copy with its last byte flipped verifies")
      message(FATAL_ERROR "publish assertion: tampered asset failed for the wrong cause: ${out}")
    endif()
    if(case STREQUAL "wrong-signer" AND NOT out MATCHES "attestation does not verify")
      message(FATAL_ERROR "publish assertion: wrong signer failed for the wrong cause: ${out}")
    endif()
  endif()
  message(STATUS "publish: ${case} control passed")
endforeach()
