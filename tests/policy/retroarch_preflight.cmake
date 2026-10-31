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
if(DEFINED WORK)
  file(MAKE_DIRECTORY "${WORK}")
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
