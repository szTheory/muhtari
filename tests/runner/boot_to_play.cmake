# Plays one game from boot through the real runner and judges the result
# (docs/emulation-tests.md, boot to play):
#   cmake -DRUNNER=<muhtari-run> -DJUDGE=<muhtari-test-boot-to-play>
#     [-DEMULATOR=<list>] -DROM=<spacephysics .a78> -DOUT=<directory>
#     -DF=<first input frame> -DH=<frames held> -DL=<response bound>
#     -P tests/runner/boot_to_play.cmake
# It writes four movies into OUT (no input; Up and Left held on port 1 from
# frame F for H frames; the same on port 2; Up and Right, the mirror on
# port 1), runs each with --hash-frame on every frame and --dump-frame at
# frame F - 1 and three frames across the hold window, and judges the port 1
# run and its mirror against the no-input run. The negative controls in
# tests.cmake judge these same outputs.
foreach(var RUNNER JUDGE ROM OUT F H L)
  if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
    message(FATAL_ERROR "boot_to_play: -D${var} is required")
  endif()
endforeach()

math(EXPR last "${F} + ${H} - 1")
math(EXPR before "${F} - 1")
math(EXPR dump1 "${F} + ${H} / 3 - 1")
math(EXPR dump2 "${F} + 2 * ${H} / 3 - 1")
set(dumps ${before} ${dump1} ${dump2} ${last})

set(hash_args "")
foreach(frame RANGE 1 ${last})
  list(APPEND hash_args --hash-frame ${frame})
endforeach()

# One movie per run, name=line: the line held from frame F, every earlier
# line released.
set(released "|......|......|...BB|")
set(runs "none=${released}" "input=|U.L...|......|...BB|" "port2=|......|U.L...|...BB|"
  "mirror=|U..R..|......|...BB|")
file(MAKE_DIRECTORY "${OUT}")
foreach(run IN LISTS runs)
  string(FIND "${run}" "=" eq)
  string(SUBSTRING "${run}" 0 ${eq} name)
  math(EXPR eq "${eq} + 1")
  string(SUBSTRING "${run}" ${eq} -1 held)
  string(REPEAT "${released}\n" ${before} lead)
  string(REPEAT "${held}\n" ${H} hold)
  file(WRITE "${OUT}/${name}.movie" "muhtari-movie 1\nport1 proline\nport2 proline\n${lead}${hold}")
  set(dump_args "")
  foreach(frame IN LISTS dumps)
    file(REMOVE "${OUT}/${name}-${frame}.png")
    list(APPEND dump_args --dump-frame "${frame}:${OUT}/${name}-${frame}.png")
  endforeach()
  file(REMOVE "${OUT}/${name}.txt")
  execute_process(COMMAND ${EMULATOR} "${RUNNER}" --movie "${OUT}/${name}.movie" ${hash_args}
      ${dump_args} "${ROM}"
    OUTPUT_FILE "${OUT}/${name}.txt" ERROR_VARIABLE err RESULT_VARIABLE rc)
  if(NOT rc STREQUAL "0" OR NOT err STREQUAL "")
    message(FATAL_ERROR "muhtari-run on ${name}.movie: exit ${rc}\n${err}")
  endif()
endforeach()

# Runs the judge with ARGN; fails unless it exits 0 with output matching
# want.
function(expect_judge want)
  execute_process(COMMAND ${EMULATOR} "${JUDGE}" ${ARGN}
    OUTPUT_VARIABLE out ERROR_VARIABLE out RESULT_VARIABLE rc)
  if(NOT rc STREQUAL "0" OR NOT out MATCHES "${want}")
    message(FATAL_ERROR "muhtari-test-boot-to-play ${ARGN}: expected exit 0 and ${want}\n"
      "got exit ${rc} and\n${out}")
  endif()
  message(STATUS "${out}")
endfunction()

expect_judge("^mask 0:" play "${OUT}" ${F} ${H} ${L} none input)
# Up and Left turn the ship's nose up as it thrusts; Up and Right, the
# mirror, must turn it down.
expect_judge("^sign flipped:" mirrored "${OUT}" ${F} ${H} none input mirror up)
