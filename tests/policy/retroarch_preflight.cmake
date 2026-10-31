# Fault prevented: an access denial falls through to any hosted content launch.
# Additional launches and truncated expected movies must also fail offline.
# Support reporting must accept valid growth and still reject missing inputs.
cmake_minimum_required(VERSION 3.25)
if(NOT DEFINED WORKFLOW)
  set(WORKFLOW "${ROOT}/.github/workflows/ci.yml")
endif()
file(READ "${WORKFLOW}" workflow)
set(gate [=[      - name: Keyboard access preflight before all content
        run: |
          mkdir -p build/frames
          cc -std=c17 -Wall -Wextra -Werror tests/retroarch/keyboard_preflight.c \
            -framework ApplicationServices -o build/frames/keyboard-preflight
          build/frames/keyboard-preflight --preflight
          if [ "${{ inputs.keyboard_preflight_only }}" = true ]; then
            echo "Access permitted; permission-only stop before any RetroArch/content launch"
            exit 1
          fi]=])
string(FIND "${workflow}" "${gate}" at)
string(FIND "${workflow}" "  retroarch:\n" job)
string(FIND "${workflow}" "      - name: Core and runner from the build" content)
if(at LESS job OR at GREATER content OR at EQUAL -1 OR content EQUAL -1 OR
    NOT workflow MATCHES "keyboard_preflight_only:[\n ]+description: [^\n]+[\n ]+type: boolean[\n ]+required: false[\n ]+default: false")
  message(FATAL_ERROR "keyboard preflight must stop before all content in the existing Mac job")
endif()
set(ordinary "RetroArch runs the dma diagnostic" "DMA screenshot equals the retained runner frame pin"
  "RetroArch holds and releases input on the owned diagnostic" "Fresh frontend and runner battery roundtrip"
  "The compare's negative control")
foreach(name IN LISTS ordinary)
  set(guard "      - name: ${name}\n        if: \${{ !inputs.keyboard_probe_only }}")
  string(FIND "${workflow}" "${guard}" guarded)
  if(guarded EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: ordinary ${name} can fall through")
  endif()
endforeach()
string(REGEX MATCH "      - name: One-process keyboard feasibility\n        if: inputs.keyboard_probe_only\n        run: [|]\n(.*)      - name: Fresh frontend" probe "${workflow}")
string(REGEX MATCHALL "-P tests/retroarch/run_retroarch[.]cmake" launches "${probe}")
list(LENGTH launches launch_count)
string(FIND "${probe}" "for mode in held released" observations)
string(FIND "${probe}" "-P tests/retroarch/run_retroarch.cmake" launched)
if(launched GREATER 0)
  string(SUBSTRING "${probe}" 0 ${launched} before_launch)
endif()
if(NOT launch_count EQUAL 1 OR observations LESS launched OR
    before_launch MATCHES "\n +(for|while|until) " OR
    NOT probe MATCHES "-DINPUT_MODE=probe" OR NOT probe MATCHES "-DFRAMES=600" OR
    NOT probe MATCHES "-DRAW_SHOTS=[$]PWD/build/ra-input-probe/shots" OR
    NOT workflow MATCHES "\n            build/ra-input-probe/shots/[*][.]png\n" OR
    NOT probe MATCHES "One-process keyboard pixels passed; scoped stop, full host acceptance remains pending\"\n          exit 1")
  message(FATAL_ERROR "keyboard preflight must stop: probe needs exactly one content process and scoped stop")
endif()
string(REGEX MATCH "# Expected movie spans all 600 observed runner frames[.]\n(.*)# Judge the independent runner frame[.]" recipe "${probe}")
set(recipe "${CMAKE_MATCH_1}")
string(REGEX REPLACE "[ \t\r\n]+" " " recipe_tokens "${recipe}")
string(REGEX MATCH [=[frame=([0-9]+) while \[ "\$frame" -lt ([0-9]+) \]; do printf '%s\\n' "\$buttons" >> "build/frames/input-\$mode[.]movie" frame=\$\(\(frame \+ ([0-9]+)\)\) done]=]
  producer "${recipe_tokens}")
set(first_frame "${CMAKE_MATCH_1}")
set(stop_frame "${CMAKE_MATCH_2}")
set(stride "${CMAKE_MATCH_3}")
if(producer STREQUAL "" OR stride LESS 1 OR stop_frame LESS_EQUAL first_frame)
  message(FATAL_ERROR "keyboard preflight must stop: movie producer has no bounded append window")
endif()
math(EXPR movie_rows "(${stop_frame}-${first_frame}+${stride}-1)/${stride}")
string(REGEX MATCH [=[--movie "build/frames/input-\$mode[.]movie" --frames ([0-9]+)]=] observation "${probe}")
set(observed_frames "${CMAKE_MATCH_1}")
if(observation STREQUAL "" OR NOT movie_rows EQUAL observed_frames OR NOT observed_frames EQUAL 600)
  message(FATAL_ERROR "keyboard preflight must stop: movie append window does not cover actual runner frames")
endif()
string(REGEX REPLACE "[ \t\r\n]+" " " probe_tokens "${probe}")
string(REGEX MATCH [=[for mode in held released; do buttons='([^']+)' if \[ "\$mode" = released \]; then buttons='([^']+)'; fi]=]
  bindings "${probe_tokens}")
set(held_buttons "${CMAKE_MATCH_1}")
set(released_buttons "${CMAKE_MATCH_2}")
if(bindings STREQUAL "" OR NOT held_buttons STREQUAL "|U...1.|......|...BB|" OR
    NOT released_buttons STREQUAL "|......|......|...BB|")
  message(FATAL_ERROR "keyboard preflight must stop: actual held/released movie bindings differ")
endif()
file(READ "${ROOT}/tests/retroarch/run_retroarch.cmake" launch)
foreach(binding "input_player1_up = \\\"up\\\"" "input_player1_b = \\\"z\\\""
    "video_vsync = \\\"true\\\"" "set(launcher \"\${input_script}\" \"--\${INPUT_MODE}\")"
    "\${launcher} \"\${retroarch}\" --verbose")
  string(FIND "${launch}" "${binding}" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard launch binding absent: ${binding}")
  endif()
endforeach()
if(launch MATCHES "input_joypad_driver|test_input_file")
  message(FATAL_ERROR "keyboard launch still requests an absent test driver")
endif()
string(REGEX MATCH "\n          (wc -l tests/retroarch/[*])\n" support_report "${workflow}")
set(support_report "${CMAKE_MATCH_1}")
if(support_report STREQUAL "")
  message(FATAL_ERROR "keyboard preflight must stop: support line reporting is absent")
endif()
# Diagnostic wiring reuses this offline consumer: reject unsafe opt-in, substituted
# artifacts, ordinary-core drift and lost failure evidence before any host content.
function(trace_step name output)
  string(FIND "${workflow}" "      - name: ${name}\n" start)
  if(start EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: missing diagnostic ${name}")
  endif()
  string(SUBSTRING "${workflow}" ${start} -1 step)
  string(FIND "${step}" "\n      - " end)
  if(NOT end EQUAL -1)
    string(SUBSTRING "${step}" 0 ${end} step)
  endif()
  set(${output} "${step}" PARENT_SCOPE)
endfunction()
trace_step("Validate input diagnostic scope before preflight or content" trace_scope)
trace_step("Separate input diagnostic from the same source" trace_producer)
trace_step("Verify input diagnostic identity and build its helper" trace_identity)
trace_step("Retain bounded input diagnostic after any stopping reason" trace_dump)
string(FIND "${workflow}" "${trace_scope}" scope_at)
string(FIND "${workflow}" "${trace_identity}" identity_at)
string(FIND "${workflow}" "      - name: RetroArch 1.22.2" host_at)
if(scope_at LESS job OR scope_at GREATER at OR identity_at GREATER host_at OR
    NOT workflow MATCHES "input_trace_diagnostic:[\n ]+description: [^\n]+[\n ]+type: boolean[\n ]+required: false[\n ]+default: false")
  message(FATAL_ERROR "keyboard preflight must stop: diagnostic guard/default/order")
endif()
set(scope_check [=[          if [ "$TRACE" = true ]; then
            test "$EVENT" = workflow_dispatch && test "$PROBE" = true && test "$PREFLIGHT" != true
          fi]=])
string(FIND "${trace_scope}" "${scope_check}" bound)
if(bound EQUAL -1)
  message(FATAL_ERROR "keyboard preflight must stop: diagnostic mode conflicts can reach content")
endif()
set(trace_condition "github.event_name == 'workflow_dispatch' && inputs.input_trace_diagnostic && inputs.keyboard_probe_only && !inputs.keyboard_preflight_only && matrix.os == 'macos' && matrix.arch == 'arm64'")
foreach(binding IN ITEMS "if: ${trace_condition}" "-DMUHTARI_TESTS=ON"
    "name: input-diagnostic-macos-arm64\n          path: build/input-diagnostic/*\n          if-no-files-found: error"
    "name: input-diagnostic-macos-arm64\n          path: build/input-diagnostic\n"
    "if: always() && inputs.input_trace_diagnostic\n        with:\n          name: retroarch-input-diagnostic")
  string(FIND "${workflow}" "${binding}" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic artifact/config binding ${binding}")
  endif()
endforeach()
set(trace_sources ".github/workflows/ci.yml libretro/CMakeLists.txt libretro/libretro.c tests/libretro/tests.cmake tests/libretro/input_trace.c tests/libretro/input_trace.h tests/libretro/input_trace.cmake tests/libretro/test_input_trace.c tests/libretro/input_trace_host.c tests/retroarch/input_trace_helper.h tests/retroarch/test_input_trace_helper.c tests/retroarch/keyboard_preflight.c tests/retroarch/run_retroarch.cmake tests/policy/retroarch_preflight.c tests/policy/retroarch_preflight.cmake tests/policy/tests.cmake docs/testing.md")
foreach(step IN ITEMS trace_producer trace_identity)
  string(FIND "${${step}}" "git ls-tree -r HEAD -- ${trace_sources} > build/input-" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic source manifest incomplete")
  endif()
endforeach()
foreach(binding IN ITEMS
    [=[test "$(git rev-parse HEAD)" = "$GITHUB_SHA"]=]
    [=[printf '%s\n' "$GITHUB_SHA" "$GITHUB_RUN_ID" "$GITHUB_RUN_ATTEMPT" > build/input-identity.txt]=]
    "cmp build/input-identity.txt build/input-diagnostic/identity.txt"
    "cmp build/input-sources.txt build/input-diagnostic/sources.txt"
    "shasum -a 256 muhtari_libretro_input_trace.dylib muhtari-test-trace-protocol identity.txt sources.txt > observed-SHA256SUMS"
    "cmp build/input-diagnostic/SHA256SUMS build/input-diagnostic/observed-SHA256SUMS"
    "if cmp -s build/libretro/cores/muhtari_libretro.dylib build/input-diagnostic/muhtari_libretro_input_trace.dylib; then exit 1; fi"
    "-DMUHTARI_INPUT_TRACE tests/retroarch/keyboard_preflight.c"
    "tests/libretro/input_trace.c -framework ApplicationServices"
    "touch build/input-diagnostic-verified")
  string(FIND "${trace_identity}" "${binding}" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic identity/parser/helper binding ${binding}")
  endif()
endforeach()
set(selection [=[          core=build/libretro/cores/muhtari_libretro.dylib
          keyboard="$PWD/build/frames/keyboard-preflight"
          trace_args=()
          if [ "${{ inputs.input_trace_diagnostic }}" = true ]; then
            core=build/input-diagnostic/muhtari_libretro_input_trace.dylib
            keyboard="$PWD/build/frames/keyboard-input-trace"
            trace_args=("-DTRACE_DIR=$PWD/build/input-trace" "-DTRACE_PARSER=$PWD/build/input-diagnostic/muhtari-test-trace-protocol")
          fi]=])
foreach(binding IN ITEMS "${selection}" [=[-DCORE="$core"]=]
    [=["-DKEYBOARD=$keyboard" -DINPUT_MODE=probe "${trace_args[@]}" ]=]
    "> build/frames/input-wrapper.log 2>&1 || rc=$?" "test \"$rc\" -eq 0")
  string(FIND "${probe}" "${binding}" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic/ordinary probe selection/log binding")
  endif()
endforeach()
foreach(binding IN ITEMS "if: always() && inputs.input_trace_diagnostic"
    "if [ -f build/input-diagnostic-verified ]; then"
    "muhtari-test-trace-protocol --dump build/input-trace > build/input-dump.txt 2>&1 || rc=$?"
    "Diagnostic identity/setup unverified; parser not executed")
  string(FIND "${trace_dump}" "${binding}" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic failure dump guard")
  endif()
endforeach()
foreach(path IN ITEMS "build/input-trace/*.trace" "build/input-dump.txt" "build/input-runtime.txt"
    "build/input-diagnostic/identity.txt" "build/input-diagnostic/sources.txt"
    "build/input-diagnostic/SHA256SUMS" "build/input-diagnostic/helper-SHA256.txt"
    "build/frames/input-wrapper.log" "build/ra-input-probe/retroarch.log")
  string(FIND "${workflow}" "            ${path}\n" bound)
  if(bound EQUAL -1)
    message(FATAL_ERROR "keyboard preflight must stop: diagnostic failure upload omits ${path}")
  endif()
endforeach()
string(FIND "${workflow}" "  ci-required:\n" required_at)
string(SUBSTRING "${workflow}" ${required_at} -1 required)
if(required MATCHES "input_trace|keyboard_probe" OR NOT required MATCHES "needs: \\[changes, hygiene, title, build, asan, nofp, be32, hash-equality, library, retroarch, perf, quality\\]")
  message(FATAL_ERROR "keyboard preflight must stop: diagnostic changed required membership/predicate")
endif()
if(DEFINED WORK)
  file(MAKE_DIRECTORY "${WORK}")
  foreach(fault IN ITEMS mode_guard mode_default artifact_source parser_hash ordinary_core diagnostic_core
      missing_parser missing_upload failure_upload extra_launch required_gate)
    set(bad "${workflow}")
    if(fault STREQUAL mode_guard)
      string(REPLACE "${scope_check}" "          true" bad "${bad}")
    elseif(fault STREQUAL mode_default)
      string(REPLACE "input_trace_diagnostic:\n" "input_trace_unchecked:\n" bad "${bad}")
    elseif(fault STREQUAL artifact_source)
      string(REPLACE "tests/libretro/input_trace.h " "" bad "${bad}")
    elseif(fault STREQUAL parser_hash)
      string(REPLACE "muhtari-test-trace-protocol identity.txt sources.txt > observed" "identity.txt sources.txt > observed" bad "${bad}")
    elseif(fault STREQUAL ordinary_core)
      string(REPLACE "core=build/libretro/cores/muhtari_libretro.dylib" "core=unverified" bad "${bad}")
    elseif(fault STREQUAL diagnostic_core)
      string(REPLACE "core=build/input-diagnostic/muhtari_libretro_input_trace.dylib" "core=build/libretro/cores/muhtari_libretro.dylib" bad "${bad}")
    elseif(fault STREQUAL missing_parser)
      string(REPLACE "-DTRACE_PARSER=" "-DOMITTED_PARSER=" bad "${bad}")
    elseif(fault STREQUAL missing_upload)
      string(REPLACE "            build/input-trace/*.trace\n" "" bad "${bad}")
    elseif(fault STREQUAL failure_upload)
      string(REPLACE "if: always() && inputs.input_trace_diagnostic" "if: success() && inputs.input_trace_diagnostic" bad "${bad}")
    elseif(fault STREQUAL extra_launch)
      string(REPLACE "-DINPUT_MODE=probe" "-DINPUT_MODE=probe -P tests/retroarch/run_retroarch.cmake" bad "${bad}")
    else()
      string(REPLACE "if: always()\n    needs:" "if: inputs.input_trace_diagnostic\n    needs:" bad "${bad}")
    endif()
    file(WRITE "${WORK}/trace-${fault}.yml" "${bad}")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DROOT=${ROOT}" "-DWORKFLOW=${WORK}/trace-${fault}.yml"
      -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(NOT rc STREQUAL "1" OR NOT "${out}${err}" MATCHES "keyboard preflight must stop")
      message(FATAL_ERROR "diagnostic wiring missed ${fault}: ${rc} ${out}${err}")
    endif()
  endforeach()
  if(UNIX)
    foreach(trace IN ITEMS absent false true)
      foreach(event IN ITEMS workflow_dispatch pull_request workflow_call push)
        foreach(probe_mode IN ITEMS false true)
          foreach(preflight_mode IN ITEMS false true)
            set(value "${trace}")
            if(trace STREQUAL absent)
              set(value "")
            endif()
            execute_process(COMMAND "${CMAKE_COMMAND}" -E env "TRACE=${value}" "EVENT=${event}"
              "PROBE=${probe_mode}" "PREFLIGHT=${preflight_mode}" /bin/bash -ec "${scope_check}"
              RESULT_VARIABLE rc TIMEOUT 5)
            if(trace STREQUAL true AND (NOT event STREQUAL workflow_dispatch OR NOT probe_mode STREQUAL true OR preflight_mode STREQUAL true))
              if(NOT rc STREQUAL "1")
                message(FATAL_ERROR "diagnostic conflicting mode reached content")
              endif()
            elseif(NOT rc STREQUAL "0")
              message(FATAL_ERROR "diagnostic default/valid mode rejected")
            endif()
          endforeach()
        endforeach()
      endforeach()
      string(REPLACE "\${{ inputs.input_trace_diagnostic }}" "${value}" selector "${selection}")
      string(APPEND selector "\nprintf '%s\\n' \"$core\" \"$keyboard\" \"\${trace_args[@]}\"")
      execute_process(COMMAND /bin/bash -ec "${selector}" RESULT_VARIABLE rc OUTPUT_VARIABLE selected TIMEOUT 5)
      if(NOT rc STREQUAL "0" OR (trace STREQUAL true AND NOT selected MATCHES "input-diagnostic/muhtari_libretro_input_trace[.]dylib\n[^\n]+keyboard-input-trace\n-DTRACE_DIR=[^\n]+\n-DTRACE_PARSER=") OR
          (NOT trace STREQUAL true AND NOT selected MATCHES "^build/libretro/cores/muhtari_libretro[.]dylib\n[^\n]+keyboard-preflight\n$"))
        message(FATAL_ERROR "diagnostic actual selector ${trace}: ${rc} ${selected}")
      endif()
    endforeach()
    string(REGEX REPLACE "^.*        run: [|]\n" "" identity_script "${trace_identity}")
    string(FIND "${identity_script}" "          cc " compile_at)
    string(SUBSTRING "${identity_script}" 0 ${compile_at} identity_script)
    set(identity_fixture "${WORK}/identity")
    file(MAKE_DIRECTORY "${identity_fixture}/build/input-diagnostic" "${identity_fixture}/build/libretro/cores")
    execute_process(COMMAND git -C "${ROOT}" rev-parse HEAD OUTPUT_VARIABLE source_sha OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    separate_arguments(source_paths UNIX_COMMAND "${trace_sources}")
    execute_process(COMMAND git -C "${ROOT}" ls-tree -r HEAD -- ${source_paths}
      OUTPUT_VARIABLE source_manifest COMMAND_ERROR_IS_FATAL ANY)
    set(artifact "${identity_fixture}/build/input-diagnostic")
    foreach(fault IN ITEMS valid identity missing_identity source parser missing_parser omitted_hash ordinary_substitution)
      file(WRITE "${artifact}/identity.txt" "${source_sha}\n7\n1\n")
      file(WRITE "${artifact}/sources.txt" "${source_manifest}")
      file(WRITE "${artifact}/muhtari_libretro_input_trace.dylib" "synthetic diagnostic module")
      file(WRITE "${artifact}/muhtari-test-trace-protocol" "synthetic parser")
      file(WRITE "${identity_fixture}/build/libretro/cores/muhtari_libretro.dylib" "synthetic ordinary module")
      execute_process(COMMAND shasum -a 256 muhtari_libretro_input_trace.dylib muhtari-test-trace-protocol identity.txt sources.txt
        WORKING_DIRECTORY "${artifact}" OUTPUT_FILE "${artifact}/SHA256SUMS" COMMAND_ERROR_IS_FATAL ANY)
      if(fault STREQUAL identity)
        file(WRITE "${artifact}/identity.txt" "${source_sha}\n8\n1\n")
      elseif(fault STREQUAL missing_identity)
        file(REMOVE "${artifact}/identity.txt")
      elseif(fault STREQUAL source)
        file(APPEND "${artifact}/sources.txt" "wrong source\n")
      elseif(fault STREQUAL parser)
        file(WRITE "${artifact}/muhtari-test-trace-protocol" "wrong parser")
      elseif(fault STREQUAL missing_parser)
        file(REMOVE "${artifact}/muhtari-test-trace-protocol")
      elseif(fault STREQUAL omitted_hash)
        file(STRINGS "${artifact}/SHA256SUMS" digests)
        list(REMOVE_AT digests 1)
        string(JOIN "\n" digests ${digests})
        file(WRITE "${artifact}/SHA256SUMS" "${digests}\n")
      elseif(fault STREQUAL ordinary_substitution)
        file(COPY_FILE "${artifact}/muhtari_libretro_input_trace.dylib"
          "${identity_fixture}/build/libretro/cores/muhtari_libretro.dylib")
      endif()
      execute_process(COMMAND "${CMAKE_COMMAND}" -E env "GIT_DIR=${ROOT}/.git" "GITHUB_SHA=${source_sha}"
        GITHUB_RUN_ID=7 GITHUB_RUN_ATTEMPT=1 /bin/bash -ec "${identity_script}"
        WORKING_DIRECTORY "${identity_fixture}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
      if((fault STREQUAL valid AND NOT rc STREQUAL "0") OR (NOT fault STREQUAL valid AND rc STREQUAL "0"))
        message(FATAL_ERROR "diagnostic actual identity ${fault}: ${rc} ${out}${err}")
      endif()
    endforeach()
  endif()
  foreach(fault IN ITEMS valid_span denial_fallthrough grant_fallthrough late_gate ordinary_dma ordinary_input ordinary_battery two_processes probe_loop short_movie lost_shots movie_binding movie_window movie_stride missing_raw support_cap)
    set(bad "${workflow}")
    if(fault STREQUAL "valid_span")
      string(REPLACE "frame=0" "frame=1" shifted "${recipe}")
      string(REPLACE "-lt 600" "-lt 601" shifted "${shifted}")
      string(REPLACE "${recipe}" "${shifted}" bad "${bad}")
    elseif(fault STREQUAL denial_fallthrough)
      string(REPLACE "keyboard-preflight --preflight\n" "keyboard-preflight --preflight || true\n" bad "${bad}")
    elseif(fault STREQUAL grant_fallthrough)
      string(REPLACE "exit 1" "exit 0" bad_gate "${gate}")
      string(REPLACE "${gate}" "${bad_gate}" bad "${bad}")
    elseif(fault STREQUAL late_gate)
      string(REPLACE "${gate}" "" bad "${bad}")
      string(APPEND bad "\n${gate}\n")
    elseif(fault MATCHES "^ordinary_")
      set(name "Fresh frontend and runner battery roundtrip")
      if(fault STREQUAL ordinary_dma)
        set(name "RetroArch runs the dma diagnostic")
      elseif(fault STREQUAL ordinary_input)
        set(name "RetroArch holds and releases input on the owned diagnostic")
      endif()
      string(REPLACE "      - name: ${name}\n        if: \${{ !inputs.keyboard_probe_only }}"
        "      - name: ${name}" bad "${bad}")
    elseif(fault STREQUAL two_processes)
      string(REPLACE "-DINPUT_MODE=probe" "-DINPUT_MODE=probe -P tests/retroarch/run_retroarch.cmake" bad "${bad}")
    elseif(fault STREQUAL probe_loop)
      string(REPLACE "One-process keyboard feasibility\n        if: inputs.keyboard_probe_only\n        run: |"
        "One-process keyboard feasibility\n        if: inputs.keyboard_probe_only\n        run: |\n          for attempt in 1 2; do" bad "${bad}")
    elseif(fault STREQUAL short_movie)
      string(REPLACE "\"$frame\" -lt 600" "\"$frame\" -lt 1" bad "${bad}")
    elseif(fault STREQUAL lost_shots)
      string(REPLACE "            build/ra-input-probe/shots/*.png\n" "" bad "${bad}")
    elseif(fault STREQUAL movie_binding)
      string(REPLACE "buttons='|U...1.|......|...BB|'" "buttons='|......|......|...BB|'" bad "${bad}")
    elseif(fault STREQUAL movie_window)
      string(REPLACE "--frames 600" "--frames 601" bad_probe "${probe}")
      string(REPLACE "${probe}" "${bad_probe}" bad "${bad}")
    elseif(fault STREQUAL missing_raw)
      string(REPLACE "-DRAW_SHOTS=$PWD/build/ra-input-probe/shots" "-DUNUSED_SHOTS=missing" bad "${bad}")
    elseif(fault STREQUAL support_cap)
      string(REPLACE "${support_report}" [=[test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 300]=] bad "${bad}")
    else()
      string(REPLACE "frame + 1" "frame + 2" bad_recipe "${recipe}")
      string(REPLACE "${recipe}" "${bad_recipe}" bad "${bad}")
    endif()
    file(WRITE "${WORK}/${fault}.yml" "${bad}")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DROOT=${ROOT}" "-DWORKFLOW=${WORK}/${fault}.yml"
      -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(fault STREQUAL "valid_span")
      if(NOT rc STREQUAL "0")
        message(FATAL_ERROR "keyboard equivalent600-row span rejected: ${rc} ${out}${err}")
      endif()
    elseif(NOT rc STREQUAL "1" OR NOT "${out}${err}" MATCHES "keyboard preflight must stop")
      message(FATAL_ERROR "keyboard preflight missed ${fault}: ${rc} ${out}${err}")
    endif()
  endforeach()
  if(UNIX)
    set(support_fixture "${WORK}/support")
    file(MAKE_DIRECTORY "${support_fixture}/tests/retroarch")
    string(REPEAT "fixture\n" 301 support_lines)
    file(WRITE "${support_fixture}/tests/retroarch/support.txt" "${support_lines}")
    execute_process(COMMAND /bin/sh -ec "${support_report}" WORKING_DIRECTORY "${support_fixture}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(NOT rc STREQUAL "0" OR NOT out MATCHES "301[ \t]+tests/retroarch/support[.]txt")
      message(FATAL_ERROR "support report rejects valid growth: ${rc} ${out}${err}")
    endif()
    file(REMOVE_RECURSE "${support_fixture}/tests")
    execute_process(COMMAND /bin/sh -ec "${support_report}" WORKING_DIRECTORY "${support_fixture}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(rc STREQUAL "0")
      message(FATAL_ERROR "support report accepted missing inputs")
    endif()
    set(fixture "${WORK}/launch")
    file(MAKE_DIRECTORY "${fixture}")
    string(REGEX MATCH "          shots=([(][^\n]+\n)([^\n]+\n)+          cp [^\n]+" capture_recipe "${probe}")
    if(capture_recipe STREQUAL "")
      message(FATAL_ERROR "keyboard capture count/file consumer is absent")
    endif()
    string(REPLACE "build/ra-input-probe/shots" "${fixture}/ra/shots" capture_recipe "${capture_recipe}")
    string(REPLACE "build/frames/input-shot-held.png" "${fixture}/held.png" capture_recipe "${capture_recipe}")
    file(WRITE "${fixture}/keyboard" [=[#!/bin/sh
[ "$1" = --probe ] || exit 2
shift
raw=$1
shift
[ "$raw" = "${0%/*}/ra/shots" ] && [ -d "$raw" ] || exit 2
printf 'keyboard\n' >> "${0%/*}/calls"
case $(cat "${0%/*}/fault") in
  capture_missing) ;;
  capture_multiple) printf fixture > "$raw/one.png"; printf fixture > "$raw/two.png" ;;
  capture_directory) mkdir "$raw/one.png" ;;
  *) printf fixture > "$raw/held.png" ;;
esac
exec "$@"
]=])
    file(WRITE "${fixture}/frontend" [=[#!/bin/sh
printf 'frontend\n' >> "${0%/*}/calls"
[ "$1" = --verbose ] && [ "$2" = --config ] || exit 2
grep -F "screenshot_directory = \"${0%/*}/ra/shots\"" "$3" || exit 2
for arg do
  case $arg in --max-frames-ss-path=*) printf fixture > "${arg#*=}" ;; esac
done
]=])
    file(CHMOD "${fixture}/keyboard" "${fixture}/frontend" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
    file(WRITE "${fixture}/core" "fixture")
    file(WRITE "${fixture}/core.info" "fixture")
    file(REMOVE "${fixture}/calls")
    set(common "-DRETROARCH=${fixture}/frontend" "-DCORE=${fixture}/core" "-DINFO=${fixture}/core.info"
      "-DTEMPLATE=${ROOT}/tests/retroarch/test.cfg.in" "-DRA_DIR=${fixture}/ra"
      "-DSAVE_DIR=${fixture}/saves" -DFRAMES=600 "-DSHOT=${fixture}/shot.png")
    foreach(case valid missing directory inside mode raw_missing raw_outside terminal_raw capture_missing capture_multiple capture_directory)
      set(keyboard "${fixture}/keyboard")
      set(mode probe)
      set(raw "${fixture}/ra/shots")
      set(shot "${fixture}/shot.png")
      if(case STREQUAL missing)
        set(keyboard "${fixture}/missing")
      elseif(case STREQUAL directory)
        set(keyboard "${fixture}")
      elseif(case STREQUAL inside)
        set(keyboard "${fixture}/ra/keyboard")
        file(COPY_FILE "${fixture}/keyboard" "${keyboard}")
      elseif(case STREQUAL "mode")
        set(mode invalid)
      elseif(case STREQUAL raw_missing)
        set(raw "")
      elseif(case STREQUAL raw_outside)
        set(raw "${fixture}/other")
      elseif(case STREQUAL terminal_raw)
        set(shot "${raw}/terminal.png")
      endif()
      file(WRITE "${fixture}/ra/sentinel" "retain")
      file(WRITE "${fixture}/fault" "${case}")
      file(REMOVE "${fixture}/calls")
      execute_process(COMMAND "${CMAKE_COMMAND}" ${common} "-DKEYBOARD=${keyboard}" "-DINPUT_MODE=${mode}"
        "-DRAW_SHOTS=${raw}" "-DSHOT=${shot}"
        -P "${ROOT}/tests/retroarch/run_retroarch.cmake" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
      if(case STREQUAL valid)
        if(NOT rc STREQUAL "0")
          message(FATAL_ERROR "keyboard fixture launch failed: ${rc} ${out}${err}")
        endif()
      elseif(case MATCHES "^capture_")
        if(NOT rc STREQUAL "0")
          message(FATAL_ERROR "keyboard ${case} mock wrapper failed: ${rc} ${out}${err}")
        endif()
      elseif(NOT rc STREQUAL "1" OR NOT "${out}${err}" MATCHES "KEYBOARD|RAW_SHOTS" OR NOT EXISTS "${fixture}/ra/sentinel")
        message(FATAL_ERROR "keyboard unsafe ${case} did not refuse before cleanup: ${rc} ${out}${err}")
      endif()
      if(case STREQUAL "valid" OR case MATCHES "^capture_")
        execute_process(COMMAND /bin/bash -ec "${capture_recipe}" RESULT_VARIABLE captured
          OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
        if((case STREQUAL "valid" AND NOT captured STREQUAL "0") OR
            (case MATCHES "^capture_" AND NOT captured STREQUAL "1"))
          message(FATAL_ERROR "keyboard actual capture consumer ${case}: ${captured} ${out}${err}")
        endif()
        file(READ "${fixture}/calls" calls)
        if(NOT calls STREQUAL "keyboard\nfrontend\n")
          message(FATAL_ERROR "keyboard ${case} did not invoke exactly one wrapped child: ${calls}")
        endif()
      elseif(EXISTS "${fixture}/calls")
        message(FATAL_ERROR "keyboard unsafe ${case} launched a child")
      endif()
    endforeach()
  endif()
endif()
if(UNIX)
  set(movie_work "${CMAKE_CURRENT_BINARY_DIR}/keyboard-movie-fixture")
  if(DEFINED WORK)
    set(movie_work "${WORK}/movie")
  endif()
  file(MAKE_DIRECTORY "${movie_work}/build/frames")
  foreach(mode held released)
    set(buttons "${held_buttons}")
    if(mode STREQUAL released)
      set(buttons "${released_buttons}")
    endif()
    execute_process(COMMAND /bin/sh -c "mode=$1; buttons=$2; ${recipe}" _ "${mode}" "${buttons}"
      WORKING_DIRECTORY "${movie_work}" RESULT_VARIABLE rc TIMEOUT 5)
    file(STRINGS "${movie_work}/build/frames/input-${mode}.movie" records)
    list(LENGTH records count)
    if(NOT rc STREQUAL "0" OR NOT count EQUAL 603)
      message(FATAL_ERROR "keyboard preflight must stop: ${mode} movie does not span600 frames")
    endif()
    list(SUBLIST records 3 600 rows)
    foreach(row IN LISTS rows)
      if(NOT row STREQUAL buttons)
        message(FATAL_ERROR "keyboard preflight must stop: wrong ${mode} movie row")
      endif()
    endforeach()
  endforeach()
endif()
