# Fault prevented: silently incomplete or privilege-unsafe hosted reports.
cmake_minimum_required(VERSION 3.25)
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/bin")
file(WRITE "${WORK}/bin/gh" [=[#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$DIGEST_WORK/calls"
run() {
  printf '{"id":%s,"run_attempt":2,"path":".github/workflows/ci.yml","event":"push","head_sha":"abc","created_at":"2026-10-02T00:00:00Z","updated_at":"2026-10-02T00:02:00Z","html_url":"https://github.com/example/public/actions/runs/%s"}' "$1" "$1"
}
case "$*" in
  'issue list '*)
    if [ "${DIGEST_CASE:-}" = duplicate ]; then echo '[{"number":1},{"number":2}]'
    elif [ -f "$DIGEST_WORK/prior.json" ]; then cat "$DIGEST_WORK/prior.json"
    else echo '[]'; fi ;;
  'issue create '*|'issue edit '*)
    [ "${DIGEST_CASE:-}" != write ] || { echo 'write failed' >&2; exit 1; }
    while [ "$#" -gt 0 ]; do
      if [ "$1" = --body-file ]; then cp "$2" "$DIGEST_WORK/published.md"; fi
      shift
    done ;;
  'api repos/example/public')
    private=false; [ "${DIGEST_CASE:-}" != private ] || private=true
    printf '{"private":%s,"default_branch":"main","created_at":"2026-10-01T00:00:00Z"}' "$private" ;;
  *actions/runs/1/attempts/*/jobs*)
    case "$*" in *attempts/1/*) n=1; result=failure ;; *) n=2; result=success ;; esac
    steps='[]'
    if [ "$n" = 1 ]; then
      case "${DIGEST_CASE:-}" in
        cancelled) result=cancelled ;;
        control) steps='[{"name":"Forced failure","conclusion":"failure"}]' ;;
      esac
    fi
    printf '{"total_count":1,"jobs":[{"id":%s,"run_id":1,"name":"native","conclusion":"%s","started_at":"2026-10-02T00:00:00Z","completed_at":"2026-10-02T00:01:00Z","html_url":"https://github.com/example/public/actions/runs/1","labels":["ubuntu"],"steps":%s}]}' "$n" "$result" "$steps" ;;
  *actions/runs*)
    [ "${DIGEST_CASE:-}" != api ] || { echo 'required API failed' >&2; exit 1; }
    count=1; [ "${DIGEST_CASE:-}" != truncated ] || count=2
    printf '{"total_count":%s,"workflow_runs":[' "$count"; run 1; printf ']}'
    printf '{"total_count":%s,"workflow_runs":[' "$count"; run 1; printf ']}' ;;
  *actions/runner-images*) [ "${DIGEST_CASE:-}" != optional ] || exit 1; echo '[]' ;;
  *issues*) echo '[{"number":7,"title":"$(touch SENTINEL) <script>bad</script>","labels":[],"created_at":"2026-10-01T00:00:00Z","updated_at":"2026-10-01T00:00:00Z","html_url":"https://github.com/example/public/issues/7"}]' ;;
  *pulls*) echo '[]' ;;
  *) echo "unexpected API: $*" >&2; exit 1 ;;
esac
]=])
# Native jq.exe uses CRLF unless binary output is requested; preserve its exit code.
find_program(DIGEST_JQ jq REQUIRED)
file(WRITE "${WORK}/bin/jq" [=[#!/bin/sh
set -eu
for arg in "$@"; do
  case "$arg" in -b|--binary) exec "$DIGEST_JQ" "$@" ;; esac
done
"$DIGEST_JQ" "$@" > "$DIGEST_WORK/jq-output" || exit "$?"
awk '{ printf "%s\r\n", $0 }' "$DIGEST_WORK/jq-output"
]=])
file(CHMOD "${WORK}/bin/gh" "${WORK}/bin/jq" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
# A literal semicolon is a legal POSIX path character and a CMake list boundary.
if(UNIX)
  set(ENV{PATH} "${WORK}/unused;entry:$ENV{PATH}")
endif()
# Native path separators also keep Windows PATH semicolons out of CMake lists.
set(cmd "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:${WORK}/bin"
  "DIGEST_WORK=${WORK}" "DIGEST_JQ=${DIGEST_JQ}"
  "GITHUB_REPOSITORY=example/public" sh "${ROOT}/scripts/digest.sh")
execute_process(COMMAND ${cmd} preview "${WORK}/report" 2026-10-01T00:00:00Z 2026-11-01T00:00:00Z
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "digest valid preview failed: ${out}")
endif()
if(NOT EXISTS "${WORK}/report/evidence.json")
  message(FATAL_ERROR "digest assertion: successful preview must persist structured evidence")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -E env "CDPATH=${WORK}" ${cmd} preview -relative-output
  2026-10-01T00:00:00Z 2026-11-01T00:00:00Z
  WORKING_DIRECTORY "${WORK}" COMMAND_ERROR_IS_FATAL ANY)
if(NOT EXISTS "${WORK}/-relative-output/evidence.json")
  message(FATAL_ERROR "digest assertion: relative output must resolve from the caller directory")
endif()
file(READ "${WORK}/report/evidence.json" evidence)
string(JSON jobs LENGTH "${evidence}" jobs)
if(NOT jobs EQUAL 2)
  message(FATAL_ERROR "digest assertion: both attempts must be counted exactly once")
endif()
file(READ "${WORK}/report/report.md" report)
foreach(text "Rerun-pass candidates: 1" "Runner minutes: 2" "unavailable" "touch SENTINEL" "2026-10-01")
  string(FIND "${report}" "${text}" found)
  if(found EQUAL -1)
    message(FATAL_ERROR "digest assertion: missing report evidence ${text}")
  endif()
endforeach()
if(EXISTS "${ROOT}/SENTINEL" OR EXISTS "${WORK}/SENTINEL")
  message(FATAL_ERROR "digest assertion: hostile title executed")
endif()
foreach(case api truncated)
  set(fail_cmd "${CMAKE_COMMAND}" -E env "DIGEST_CASE=${case}" ${cmd} preview
    "${WORK}/bad" 2026-10-01T00:00:00Z 2026-11-01T00:00:00Z)
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${fail_cmd}" -DCODE=1
    "-DREGEX=digest:.*(required|truncated)" -P "${ROOT}/tests/cmake/expect_fail.cmake"
    COMMAND_ERROR_IS_FATAL ANY)
endforeach()
foreach(case cancelled control)
  execute_process(COMMAND "${CMAKE_COMMAND}" -E env "DIGEST_CASE=${case}" ${cmd} preview
    "${WORK}/${case}" 2026-10-01T00:00:00Z 2026-11-01T00:00:00Z COMMAND_ERROR_IS_FATAL ANY)
  file(READ "${WORK}/${case}/report.md" cost)
  if(NOT cost MATCHES "Runner minutes: 2" OR NOT cost MATCHES "Rerun-pass candidates: 0")
    message(FATAL_ERROR "digest assertion: ${case} cost or candidacy incorrect")
  endif()
endforeach()
file(READ "${WORK}/calls" calls)
if(calls MATCHES "issue (create|edit)|-X (POST|PATCH)")
  message(FATAL_ERROR "digest assertion: preview attempted write")
endif()
execute_process(COMMAND ${cmd} preview "${WORK}/calendar" COMMAND_ERROR_IS_FATAL ANY)
file(READ "${WORK}/calendar/evidence.json" calendar)
string(JSON start GET "${calendar}" start)
string(JSON end GET "${calendar}" end)
if(NOT start MATCHES "-01T00:00:00Z$" OR NOT end MATCHES "-01T00:00:00Z$" OR NOT start STRLESS end)
  message(FATAL_ERROR "digest assertion: default calendar boundaries")
endif()
set(publish "${CMAKE_COMMAND}" -E env "GITHUB_ACTIONS=true" "GITHUB_EVENT_NAME=workflow_dispatch"
  "GITHUB_REF=refs/heads/main" "DIGEST_PUBLISH=true" ${cmd} publish "${WORK}/report")
execute_process(COMMAND ${publish} RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
if(NOT rc STREQUAL "0" OR NOT EXISTS "${WORK}/published.md")
  message(FATAL_ERROR "digest assertion: trusted publisher must create a rolling issue: ${out}")
endif()
foreach(case private duplicate write event branch input)
  set(bad "${CMAKE_COMMAND}" -E env "DIGEST_CASE=${case}" ${publish})
  set(regex "digest: required digest create failed")
  if(case STREQUAL "private")
    set(regex "digest: public repository required")
  elseif(case STREQUAL "duplicate")
    set(regex "digest: duplicate digest issues")
  elseif(case STREQUAL "event" OR case STREQUAL "branch" OR case STREQUAL "input")
    set(variable GITHUB_EVENT_NAME=pull_request)
    if(case STREQUAL "branch")
      set(variable GITHUB_REF=refs/heads/feature)
    elseif(case STREQUAL "input")
      set(variable DIGEST_PUBLISH=false)
    endif()
    set(bad ${publish})
    list(INSERT bad 4 "${variable}")
    # Insert after the corresponding trusted assignment so the denied value wins.
    list(INSERT bad 8 "${variable}")
    set(regex "digest: forbidden publisher")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${bad}" -DCODE=1 "-DREGEX=${regex}"
    -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
endforeach()
file(WRITE "${WORK}/old.md" "<!-- digest-month:start -->\nRetained complete month\nWindow: 2026-08-01T00:00:00Z to 2026-09-01T00:00:00Z UTC\n<!-- digest-month:end -->\n")
execute_process(COMMAND jq -n --rawfile body "${WORK}/old.md" "[{number:7,body:$body}]"
  OUTPUT_FILE "${WORK}/prior.json" COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${cmd} preview "${WORK}/report" 2026-10-01T00:00:00Z 2026-10-20T00:00:00Z COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${publish} COMMAND_ERROR_IS_FATAL ANY)
file(READ "${WORK}/published.md" retained)
if(NOT retained MATCHES "Retained complete month")
  message(FATAL_ERROR "digest assertion: partial update erased latest month")
endif()
execute_process(COMMAND ${cmd} preview "${WORK}/report" 2026-09-01T00:00:00Z 2026-10-01T00:00:00Z COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${publish} COMMAND_ERROR_IS_FATAL ANY)
file(READ "${WORK}/published.md" replacement)
if(replacement MATCHES "Retained complete month" OR NOT replacement MATCHES "Window: 2026-09-01")
  message(FATAL_ERROR "digest assertion: newer complete month did not replace retained month")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -E env DIGEST_CASE=optional ${cmd} preview
  "${WORK}/optional" 2026-10-01T00:00:00Z 2026-10-20T00:00:00Z COMMAND_ERROR_IS_FATAL ANY)
file(READ "${WORK}/optional/report.md" optional)
if(NOT optional MATCHES "announcements: unavailable")
  message(FATAL_ERROR "digest assertion: optional source absence hidden")
endif()
file(READ "${WORK}/report/last-success.txt" successful)
set(bad "${CMAKE_COMMAND}" -E env DIGEST_CASE=write ${publish})
execute_process(COMMAND "${CMAKE_COMMAND}" "-DCMD=${bad}" -DCODE=1 "-DREGEX=digest: required digest edit failed"
  -P "${ROOT}/tests/cmake/expect_fail.cmake" COMMAND_ERROR_IS_FATAL ANY)
file(READ "${WORK}/report/last-success.txt" after)
if(NOT after STREQUAL successful)
  message(FATAL_ERROR "digest assertion: failed write changed successful timestamp")
endif()
file(READ "${ROOT}/.github/workflows/digest.yml" workflow)
foreach(contract "group: digest-publisher" "cancel-in-progress: false" "issues: read" "issues: write" "github.event.repository.default_branch" "inputs.publish")
  string(FIND "${workflow}" "${contract}" found)
  if(found EQUAL -1)
    message(FATAL_ERROR "digest assertion: workflow missing privilege/serialization contract ${contract}")
  endif()
endforeach()
if(workflow MATCHES "secrets[.]|create-github-app-token|pull_request_target")
  message(FATAL_ERROR "digest assertion: preview gained App or privileged event")
endif()
