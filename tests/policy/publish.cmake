# Fault prevented: publication accepting another workflow's valid attestation.
cmake_minimum_required(VERSION 3.25)
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/bin" "${WORK}/dist")
file(WRITE "${WORK}/bin/gh" [=[#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$PUBLISH_WORK/calls"
case "$*" in
  --version)
    [ "$PUBLISH_VERSION" != failed ] || exit 1
    [ "$PUBLISH_VERSION" = empty ] || printf '%s\n' "$PUBLISH_VERSION"
    exit 0 ;;
  'attestation verify '*)
    case "$3" in
      "$PUBLISH_WORK"/dist/*) ;;
      *) [ "$PUBLISH_CASE" = tampered ] ;;
    esac
    case "$PUBLISH_CASE" in wrong-signer | prefix-signer)
      case " $* " in
        *" --signer-workflow example/public/.github/workflows/release.yml "*) exit 1 ;;
      esac
      ;; wrong-ref | case-ref)
      case " $* " in *" --source-ref refs/heads/main "*) exit 1 ;; esac ;;
    esac ;;
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
script=$1/scripts/publish.sh
if [ "$PUBLISH_ENTRY" = verify ]; then set -- verify rehearsal-fixture; else set -- attested; fi
sh "$script" "$@" "$PUBLISH_WORK/dist" &&
gh api -X PATCH repos/example/public/releases/7 -F draft=false
]=])
set(cmd "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:${WORK}/bin"
  "PUBLISH_WORK=${WORK}" "RUNNER_TEMP=${WORK}/temp"
  GITHUB_REPOSITORY=example/public SOURCE_REF=refs/heads/main)
foreach(case correct tampered old wrong-signer prefix-signer wrong-ref case-ref failed empty partial nonnumeric unknown prerelease patch minor major)
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
    if(entry STREQUAL "verify" AND NOT regex MATCHES "supported stable")
      continue()
    endif()
  file(WRITE "${WORK}/calls" "")
  execute_process(COMMAND ${cmd} "PUBLISH_CASE=${case}" "PUBLISH_VERSION=${version}" "PUBLISH_ENTRY=${entry}" sh -c "${publish}" sh "${ROOT}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
  file(READ "${WORK}/calls" calls)
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
