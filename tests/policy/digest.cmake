# Fault prevented: silently incomplete or privilege-unsafe hosted reports.
cmake_minimum_required(VERSION 3.25)
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}/bin")
file(WRITE "${WORK}/bin/gh" [=[#!/bin/sh
set -eu
printf '%s\n' "$*" >> "$DIGEST_WORK/calls"
case "$*" in
  'api repos/example/public') echo '{"private":false,"default_branch":"main","created_at":"2026-10-01T00:00:00Z"}' ;;
  *actions/runs/1/attempts/*/jobs*)
    case "$*" in *attempts/1/*) n=1; result=failure ;; *) n=2; result=success ;; esac
    printf '{"total_count":1,"jobs":[{"id":%s,"run_id":1,"name":"native","conclusion":"%s","started_at":"2026-10-02T00:00:00Z","completed_at":"2026-10-02T00:01:00Z","html_url":"https://github.com/example/public/actions/runs/1","labels":["ubuntu"],"steps":[]}]}' "$n" "$result" ;;
  *actions/runs*)
    [ "${DIGEST_CASE:-}" != api ] || { echo 'required API failed' >&2; exit 1; }
    count=1; [ "${DIGEST_CASE:-}" != truncated ] || count=1001
    printf '{"total_count":%s,"workflow_runs":[{"id":1,"run_attempt":2,"path":".github/workflows/ci.yml","event":"push","head_sha":"abc","created_at":"2026-10-02T00:00:00Z","updated_at":"2026-10-02T00:02:00Z","html_url":"https://github.com/example/public/actions/runs/1"}]}' "$count" ;;
  *issues*) echo '[{"number":7,"title":"$(touch SENTINEL) <script>bad</script>","labels":[],"created_at":"2026-10-01T00:00:00Z","updated_at":"2026-10-01T00:00:00Z","html_url":"https://github.com/example/public/issues/7"}]' ;;
  *pulls*) echo '[]' ;;
  *actions/runner-images*) echo '[]' ;;
  *) echo "unexpected API: $*" >&2; exit 1 ;;
esac
]=])
file(CHMOD "${WORK}/bin/gh" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
set(cmd "${CMAKE_COMMAND}" -E env "PATH=${WORK}/bin:$ENV{PATH}" "DIGEST_WORK=${WORK}"
  "GITHUB_REPOSITORY=example/public" sh "${ROOT}/scripts/digest.sh")
execute_process(COMMAND ${cmd} preview "${WORK}/report" 2026-10-01T00:00:00Z 2026-11-01T00:00:00Z
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "digest valid preview failed: ${out}")
endif()
if(NOT EXISTS "${WORK}/report/evidence.json")
  message(FATAL_ERROR "digest assertion: successful preview must persist structured evidence")
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
file(READ "${WORK}/calls" calls)
if(calls MATCHES "issue (create|edit)|-X (POST|PATCH)")
  message(FATAL_ERROR "digest assertion: preview attempted write")
endif()
