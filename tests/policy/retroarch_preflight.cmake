# Fault prevented: an access denial falls through to any hosted content launch.
cmake_minimum_required(VERSION 3.25)
if(NOT DEFINED WORKFLOW)
  set(WORKFLOW "${ROOT}/.github/workflows/ci.yml")
endif()
file(READ "${WORKFLOW}" workflow)
set(gate [=[      - name: Keyboard access preflight only
        if: github.event_name == 'workflow_dispatch' && inputs.keyboard_preflight_only
        run: |
          mkdir -p build/frames
          cc -std=c17 -Wall -Wextra -Werror tests/retroarch/keyboard_preflight.c \
            -framework ApplicationServices -o build/frames/keyboard-preflight
          build/frames/keyboard-preflight --preflight
          echo "Access permitted; permission-only stop before any RetroArch/content launch"
          exit 1]=])
string(FIND "${workflow}" "${gate}" at)
string(FIND "${workflow}" "  retroarch:\n" job)
string(FIND "${workflow}" "      - name: Core and runner from the build" content)
if(at LESS job OR at GREATER content OR at EQUAL -1 OR content EQUAL -1 OR
    NOT workflow MATCHES "keyboard_preflight_only:[\n ]+description: [^\n]+[\n ]+type: boolean[\n ]+required: false[\n ]+default: false")
  message(FATAL_ERROR "keyboard preflight must stop before all content in the existing Mac job")
endif()
if(DEFINED WORK)
  file(MAKE_DIRECTORY "${WORK}")
  foreach(fault IN ITEMS denial_fallthrough grant_fallthrough late_gate)
    set(bad "${workflow}")
    if(fault STREQUAL denial_fallthrough)
      string(REPLACE "keyboard-preflight --preflight\n" "keyboard-preflight --preflight || true\n" bad "${bad}")
    elseif(fault STREQUAL grant_fallthrough)
      string(REPLACE "exit 1" "exit 0" bad_gate "${gate}")
      string(REPLACE "${gate}" "${bad_gate}" bad "${bad}")
    else()
      string(REPLACE "${gate}" "" bad "${bad}")
      string(APPEND bad "\n${gate}\n")
    endif()
    file(WRITE "${WORK}/${fault}.yml" "${bad}")
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DROOT=${ROOT}" "-DWORKFLOW=${WORK}/${fault}.yml"
      -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 5)
    if(NOT rc STREQUAL "1" OR NOT "${out}${err}" MATCHES "keyboard preflight must stop")
      message(FATAL_ERROR "keyboard preflight missed ${fault}: ${rc} ${out}${err}")
    endif()
  endforeach()
endif()
