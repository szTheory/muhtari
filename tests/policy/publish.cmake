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
  'api '*)
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
foreach(component library runner libretro)
  foreach(os linux macos windows)
    foreach(arch x64 arm64)
      file(WRITE "${WORK}/dist/muhtari-${component}-${os}-${arch}.zip" "synthetic asset\n")
    endforeach()
  endforeach()
endforeach()

# Shared response queue exercises production command substitutions and conditional callers.
set(cmd "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:${WORK}/bin"
  "PUBLISH_WORK=${WORK}" "RUNNER_TEMP=${WORK}/temp"
  GITHUB_REPOSITORY=example/public SOURCE_REF=refs/heads/main)
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
set(delete_sequence "list_present;release;delete_release;release_absent;list_empty;tag;delete_tag;tag_absent;list_empty;release_absent;tag_absent")
set(rows "create|0||0||" "delete|0||0||" "delete|3|crlf|0||" "delete|7|crlf|0||"
  "delete|3|missing-separator|1|invalid HTTP response|git/ref" "delete|3|bad-header|1|invalid HTTP response|git/ref"
  "delete|3|contradiction|1|HTTP command/status disagreement|git/ref" "create|1|later|0||POST" "create|1|present|0||POST")
foreach(code 401 403 404 429 500 503 transport)
  list(APPEND rows "create|1|fail-${code}|1|release listing failed|POST|DELETE" "delete|1|fail-${code}|1|release listing failed|DELETE")
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
  "delete|5|fail-503|1|release listing failed|git/ref" "delete|6|json-{}|1|tag identity|DELETE.*git/refs"
  "delete|8|http-200|1|tag still exists|PATCH" "delete|9|present|1|release still listed|PATCH"
  "delete|10|http-200|1|release still exists|PATCH" "delete|11|http-200|1|tag still exists|PATCH")
foreach(code 403 429 500 503 transport)
  foreach(index 2 3 4 6 7 8)
    list(APPEND rows "delete|${index}|http-${code}|1|publish:|PATCH")
  endforeach()
  list(APPEND rows "create|2|http-${code}|1|publish:|PATCH")
endforeach()
foreach(code 409 422)
  list(APPEND rows "delete|7|http-${code}|1|DELETE status ${code}|PATCH")
endforeach()
foreach(row IN LISTS rows)
  string(REPLACE "|" ";" fields "${row}")
  list(GET fields 0 operation)
  list(GET fields 1 changed)
  list(GET fields 2 replacement)
  list(GET fields 3 expected_rc)
  list(GET fields 4 diagnostic)
  list(GET fields 5 forbidden)
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
  execute_process(COMMAND ${cmd} EXPECTED_SHA=fixture-sha sh "${ROOT}/scripts/publish.sh" "rehearsal-${operation}" rehearsal-fixture
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
