# muhtari-run tests.
#
# This file is also the command-line checks themselves, run as
#   cmake -DRUN=<muhtari-run> [-DEMULATOR=<list>] -DCASE=<name> [-DOUT=<file>]
#     [-DVERSION_FILE=<file>] [-DFRAMES=<long run>] [-DDIR=<image directory>]
#     -P tests/runner/tests.cmake
# EMULATOR is the build's CMAKE_CROSSCOMPILING_EMULATOR, so an emulated lane
# runs its own guest binary.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
  if(CASE STREQUAL "hashes_control")
    set(HASHES_FUNCTIONS_ONLY ON)
    include("${HASHES}")
    file(READ "${REPORT}" observed)
    hashes_diagnostic(battery battery.control 120 2 "${observed}" unused)
    return()
  endif()
  if(CASE STREQUAL "controller_hashes_control")
    set(HASHES_FUNCTIONS_ONLY ON)
    include("${HASHES}")
    file(READ "${REPORT}" observed)
    hashes_controller_report(controller.motion.driving.both "${SCENARIO}" "${observed}" unused)
    return()
  endif()
  if(CASE STREQUAL "hashes")
    execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=write "-DRUN=${RUN}"
      "-DDRIVER=${DRIVER}" "-DDIAG=${DIAG}" "-DOUT=${OUT}"
      "-DEMULATOR=${EMULATOR}" "-DSOUNDTEST=${SOUNDTEST}" "-DPOKEY=${POKEY}"
      -P "${HASHES}" COMMAND_ERROR_IS_FATAL ANY)
    file(STRINGS "${OUT}/hashes.txt" rows)
    list(LENGTH rows count)
    if(NOT count EQUAL 342)
      message(FATAL_ERROR "hashes: expected 69 retained and 273 controller observation rows, got ${count}")
    endif()
    # Independent accepted source 54485d6, CI run 40104749040. Only the
    # seven named audio behaviour annotations advance from 4 to 5; all
    # earlier numeric and digest bytes remain pinned together.
    list(SUBLIST rows 0 69 retained)
    list(JOIN retained "\n" retained)
    string(SHA256 retained_digest "${retained}\n")
    if(NOT retained_digest STREQUAL "c4c9c310418422c30bbf32b204a157a564d343aadfabee4d5654b97695461761")
      message(FATAL_ERROR "hashes: retained observations changed beyond the seven audio revision annotations")
    endif()
    foreach(launch first second)
      foreach(field frame1 frame120 audio_transitions_sha256 audio_pcm_sha256 audio_frames
          audio_samples audio_rate audio_behaviour_revision audio_synthesis diagnostic save_sha256)
        set(found "${rows}")
        list(FILTER found INCLUDE REGEX "^battery[.]${launch}[.]${field} ")
        list(LENGTH found n)
        if(NOT n EQUAL 1)
          message(FATAL_ERROR "hashes: missing or duplicate battery.${launch}.${field}")
        endif()
      endforeach()
    endforeach()
    file(READ "${OUT}/hashes.txt" complete)
    foreach(fault missing_old duplicate_old missing_new duplicate_new missing_controller
        duplicate_controller reordered_controller malformed_numeric wrong_family blank_controller)
      set(prefix frame1)
      if(fault MATCHES "_new$")
        set(prefix "battery[.]second[.]save_sha256")
      elseif(fault MATCHES "controller$")
        set(prefix "controller[.]light[.]lightgun[.]both[.]frame1")
      endif()
      string(REGEX MATCH "${prefix} [^\n]+\n" row "${complete}")
      if(fault MATCHES "^missing_")
        string(REPLACE "${row}" "" bad "${complete}")
      elseif(fault MATCHES "^duplicate_")
        set(bad "${complete}${row}")
      elseif(fault STREQUAL "reordered_controller")
        string(REPLACE "${row}" "" bad "${complete}")
        string(APPEND bad "${row}")
      elseif(fault STREQUAL "malformed_numeric")
        string(REPLACE "controller.motion.driving.both.audio_frames 12"
          "controller.motion.driving.both.audio_frames x" bad "${complete}")
      elseif(fault STREQUAL "blank_controller")
        string(REPLACE "controller.motion.driving.both.audio_frames 12\n"
          "controller.motion.driving.both.audio_frames 12\n\n" bad "${complete}")
      else()
        string(REPLACE "controller.motion.driving.both." "controller.motion.foreign.both."
          bad "${complete}")
      endif()
      file(WRITE "${OUT}/hashes-battery/${fault}-a.txt" "${bad}")
      file(WRITE "${OUT}/hashes-battery/${fault}-b.txt" "${bad}")
      execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=compare -DCOUNT=2
        "-DFILES=${OUT}/hashes-battery/${fault}-a.txt;${OUT}/hashes-battery/${fault}-b.txt"
        -P "${HASHES}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
      if(NOT rc STREQUAL "1" OR NOT err MATCHES "hashes:")
        message(FATAL_ERROR "hashes: accepted ${fault} equality: ${rc} ${out}${err}")
      endif()
    endforeach()
    file(READ "${OUT}/hashes-controllers/driving-0/hashes-report.txt" controller_report)
    foreach(fault valid missing duplicate reordered blank leading_blank malformed_numeric short_digest version
        crc numeric wrong_family)
      set(bad "${controller_report}")
      set(scenario motion0)
      if(fault STREQUAL "blank")
        string(REPLACE "audio_frames 12\n" "audio_frames 12\n\n" bad "${bad}")
      elseif(fault STREQUAL "leading_blank")
        set(bad "\n${bad}")
      elseif(fault STREQUAL "missing")
        string(REGEX REPLACE "frame 12 sha256 [^\n]+\n" "" bad "${bad}")
      elseif(fault STREQUAL "duplicate")
        string(APPEND bad "${bad}")
      elseif(fault STREQUAL "reordered")
        string(REGEX MATCH "frame 1 sha256 [^\n]+\n" first "${bad}")
        string(REPLACE "${first}" "" bad "${bad}")
        string(APPEND bad "${first}")
      elseif(fault STREQUAL "malformed_numeric")
        string(REGEX REPLACE "audio_samples [0-9]+" "audio_samples 0" bad "${bad}")
      elseif(fault STREQUAL "short_digest")
        string(REGEX REPLACE "audio_pcm_sha256 [^\n]+" "audio_pcm_sha256 a" bad "${bad}")
      elseif(fault STREQUAL "version")
        string(REPLACE "audio_behaviour_revision 5" "audio_behaviour_revision 4" bad "${bad}")
      elseif(fault MATCHES "^(crc|numeric)$")
        set(which 4)
        if(fault STREQUAL "numeric")
          set(which 2)
        endif()
        file(READ "${OUT}/hashes-controllers/driving-0/fault-${which}.txt" mailbox)
        string(REGEX REPLACE "diagnostic [^\n]+\n" "${mailbox}" bad "${bad}")
      elseif(fault STREQUAL "wrong_family")
        set(scenario motion1)
      endif()
      file(WRITE "${OUT}/hashes-controllers/control.txt" "${bad}")
      execute_process(COMMAND "${CMAKE_COMMAND}" -DCASE=controller_hashes_control
        "-DHASHES=${HASHES}" "-DREPORT=${OUT}/hashes-controllers/control.txt"
        "-DSCENARIO=${scenario}" "-DDRIVER=${DRIVER}" "-DDIAG=${DIAG}"
        "-DOUT=${OUT}/hashes-controllers" "-DEMULATOR=${EMULATOR}"
        -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
      if((fault STREQUAL "valid" AND NOT rc STREQUAL "0") OR
          (NOT fault STREQUAL "valid" AND (NOT rc STREQUAL "1" OR NOT err MATCHES "hashes:")))
        message(FATAL_ERROR "hashes: controller report ${fault} control: ${rc} ${out}${err}")
      endif()
    endforeach()
    set(report "")
    foreach(row IN LISTS rows)
      if(row MATCHES "^battery[.]second[.]" AND NOT row MATCHES "[.]save_sha256 ")
        string(REGEX REPLACE "^battery[.]second[.]" "" row "${row}")
        string(REGEX REPLACE "^frame(1|120) " "frame \\1 sha256 " row "${row}")
        string(APPEND report "${row}\n")
      endif()
    endforeach()
    foreach(fault valid missing duplicate blank leading_blank revision mailbox)
      set(bad "${report}")
      if(fault STREQUAL "missing")
        string(REGEX REPLACE "frame 120 [^\n]+\n" "" bad "${bad}")
      elseif(fault STREQUAL "duplicate")
        string(APPEND bad "${report}")
      elseif(fault STREQUAL "blank")
        string(REPLACE "audio_frames 120\n" "audio_frames 120\n\n" bad "${bad}")
      elseif(fault STREQUAL "leading_blank")
        set(bad "\n${bad}")
      elseif(fault STREQUAL "revision")
        string(REPLACE "audio_behaviour_revision 5" "audio_behaviour_revision 4" bad "${bad}")
      elseif(fault STREQUAL "mailbox")
        string(REPLACE "diagnostic 4d374447" "diagnostic 00000000" bad "${bad}")
      endif()
      file(WRITE "${OUT}/hashes-battery/control.txt" "${bad}")
      execute_process(COMMAND "${CMAKE_COMMAND}" -DCASE=hashes_control "-DHASHES=${HASHES}"
        "-DREPORT=${OUT}/hashes-battery/control.txt" "-DDRIVER=${DRIVER}" "-DDIAG=${DIAG}"
        "-DOUT=${OUT}/hashes-battery" "-DEMULATOR=${EMULATOR}"
        -P "${CMAKE_CURRENT_LIST_FILE}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
      if((fault STREQUAL "valid" AND NOT rc STREQUAL "0") OR
          (NOT fault STREQUAL "valid" AND (NOT rc STREQUAL "1" OR NOT err MATCHES "hashes:")))
        message(FATAL_ERROR "hashes: report ${fault} control: ${rc} ${out}${err}")
      endif()
    endforeach()
    return()
  endif()
  if(CASE STREQUAL "save_roundtrip")
    execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" roundtrip RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "actual-main save roundtrip failed: ${rc}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --save-dir "${DIR}" "${DIR}/game.a78"
      RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "fresh runner save restore failed: ${rc}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${SAVE_TEST}" "${DIR}" verify RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "fresh process exact save bytes failed: ${rc}")
    endif()
    return()
  endif()
  if(CASE STREQUAL "audio")
    string(REPEAT "|....12|......|...BB|\n" 60 audio_input)
    file(WRITE "${CAPTURE}.movie" "muhtari-movie 1\nport1 proline\nport2 proline\n${audio_input}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${CAPTURE}.movie" --hash-audio "${ROM}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE got ERROR_VARIABLE err)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "audio runner failed: ${rc} ${err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${REFERENCE}" --reference-hashes "${ROM}" 60 fire
      RESULT_VARIABLE rc OUTPUT_VARIABLE want ERROR_VARIABLE err)
    string(REGEX REPLACE "^lag_frames [0-9]+\n" "" got "${got}")
    if(got MATCHES "audio_transitions_sha256 e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
      message(FATAL_ERROR "audio tracer was silent")
    endif()
    if(NOT rc STREQUAL "0" OR NOT got STREQUAL want)
      message(FATAL_ERROR "audio canonical reference mismatch: ${rc}\n${got}\n${want}\n${err}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --hash-audio --hash-audio
      RESULT_VARIABLE duplicate OUTPUT_QUIET ERROR_QUIET)
    if(NOT duplicate STREQUAL "2")
      message(FATAL_ERROR "duplicate audio flag accepted")
    endif()
    return()
  endif()
  set(card "9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4")

  # Runs muhtari-run with ARGN; fails unless it exits 0 with stdout want.
  function(expect_output want)
    execute_process(COMMAND ${EMULATOR} "${RUN}" ${ARGN}
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL want)
      message(FATAL_ERROR "muhtari-run ${ARGN}: expected exit 0 and output\n${want}"
        "got exit ${rc} and output\n${out}${err}")
    endif()
  endfunction()

  # Runs muhtari-run with ARGN through expect_fail.cmake: exit code and
  # output matching regex, or the test fails.
  function(expect_exit code regex)
    set(CMD ${EMULATOR} "${RUN}" ${ARGN})
    set(CODE ${code})
    set(REGEX "${regex}")
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cmake/expect_fail.cmake)
  endfunction()


  # The --probe line keys, in the order the runner prints them.
  set(identity_keys kind source board board_options rom_size ram_size sound devices
    port1 port2 region)
  set(header_keys title a78_version a78_rom_size a78_cart_type a78_port1 a78_port2 a78_tv
    a78_save a78_xm a78_mapper a78_mapper_options a78_audio a78_irq)
  set(hash_keys sha1 sha256 md5 crc32 sha1_headerless sha256_headerless md5_headerless
    crc32_headerless)

  # Runs muhtari-run --probe on DIR/name; sets out, err and rc here.
  macro(probe name)
    execute_process(COMMAND ${EMULATOR} "${RUN}" --probe "${DIR}/${name}"
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(out MATCHES "\r")
      message(FATAL_ERROR "--probe ${name}: standard output holds a carriage return")
    endif()
  endmacro()

  # Fails unless rc is code and the keys of out's lines are, in order, ARGN.
  function(expect_keys name code)
    string(REGEX MATCHALL "[^\n]+" lines "${out}")
    set(keys "")
    foreach(line IN LISTS lines)
      string(REGEX REPLACE " .*" "" key "${line}")
      list(APPEND keys "${key}")
    endforeach()
    if(NOT rc STREQUAL code OR NOT keys STREQUAL "${ARGN}")
      message(FATAL_ERROR "--probe ${name}: expected exit ${code} and keys\n${ARGN}\n"
        "got exit ${rc} and keys\n${keys}\noutput\n${out}${err}")
    endif()
  endfunction()

  # Sets var to the value of key's line in out: everything after the first
  # space.
  function(probe_value key var)
    string(REGEX MATCH "(^|\n)${key} ([^\n]*)" match "${out}")
    set(${var} "${CMAKE_MATCH_2}" PARENT_SCOPE)
  endfunction()

  # Fails unless the sha1, sha256 and md5 lines with suffix equal CMake's
  # own digests of file and the crc32 line is eight lowercase hex digits.
  function(expect_hashes suffix file)
    foreach(alg IN ITEMS SHA1 SHA256 MD5)
      file(${alg} "${file}" want)
      string(TOLOWER "${alg}${suffix}" key)
      probe_value(${key} got)
      if(NOT got STREQUAL want)
        message(FATAL_ERROR "--probe: ${key} is '${got}', CMake's ${alg} of ${file} is ${want}")
      endif()
    endforeach()
    probe_value(crc32${suffix} got)
    if(NOT got MATCHES "^[0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f][0-9a-f]$")
      message(FATAL_ERROR "--probe: crc32${suffix} is '${got}', not eight lowercase hex digits")
    endif()
  endfunction()

  # Fault prevented: an Activision probe hides original identity or confuses
  # detected input orientation with an assumed OM declaration. Kept inside
  # the existing real testdb probe case, including representative mutations.
  function(activision_probe_valid observed image body source orientation assumed result)
    set(valid TRUE)
    foreach(line IN ITEMS "kind 7800" "source ${source}" "board activision"
        "rom_size 131072" "ram_size 0")
      if(NOT observed MATCHES "(^|\n)${line}\n")
        set(valid FALSE)
      endif()
    endforeach()
    foreach(pair IN ITEMS "flag activision_half_swapped;${orientation}"
        "warning activision_layout_assumed;${assumed}")
      list(GET pair 0 line)
      list(GET pair 1 required)
      if(observed MATCHES "(^|\n)${line}\n")
        if(NOT required)
          set(valid FALSE)
        endif()
      elseif(required)
        set(valid FALSE)
      endif()
    endforeach()
    foreach(alg IN ITEMS SHA1 SHA256 MD5)
      string(TOLOWER "${alg}" key)
      file(${alg} "${image}" full_digest)
      file(${alg} "${body}" body_digest)
      if(NOT observed MATCHES "(^|\n)${key} ${full_digest}\n" OR
          NOT observed MATCHES "(^|\n)${key}_headerless ${body_digest}\n")
        set(valid FALSE)
      endif()
    endforeach()
    set(${result} "${valid}" PARENT_SCOPE)
  endfunction()

  # Writes a movie to OUT/name: the version line, the header lines in
  # header, then for each N:LINE in ARGN, N copies of the frame LINE. Sets
  # movie to its path.
  function(write_movie name header)
    set(text "muhtari-movie 1\n${header}")
    foreach(run IN LISTS ARGN)
      string(FIND "${run}" ":" colon)
      string(SUBSTRING "${run}" 0 ${colon} n)
      math(EXPR colon "${colon} + 1")
      string(SUBSTRING "${run}" ${colon} -1 line)
      string(REPEAT "${line}\n" ${n} lines)
      string(APPEND text "${lines}")
    endforeach()
    file(WRITE "${OUT}/${name}" "${text}")
    set(movie "${OUT}/${name}" PARENT_SCOPE)
  endfunction()

  # Fails unless out holds a "handoff_frame K" line with K in OpenBIOS's
# window, 300 to 340, as tests/boot/test_boot.c requires of simple.
function(expect_handoff out)
  string(REGEX MATCH "(^|\n)handoff_frame ([0-9]+)\n" match "${out}")
  if(match STREQUAL "" OR CMAKE_MATCH_2 LESS 300 OR CMAKE_MATCH_2 GREATER 340)
    message(FATAL_ERROR "expected handoff_frame from 300 to 340, got\n${out}")
  endif()
endfunction()

# Runs muhtari-run with ARGN twice; fails unless both exit 0 with the same
  # standard output and nothing on standard error. Sets var to the output.
  function(run_twice var)
    foreach(pass IN ITEMS 1 2)
      execute_process(COMMAND ${EMULATOR} "${RUN}" ${ARGN}
        OUTPUT_VARIABLE out${pass} ERROR_VARIABLE err RESULT_VARIABLE rc)
      if(NOT rc STREQUAL "0" OR NOT err STREQUAL "")
        message(FATAL_ERROR "muhtari-run ${ARGN}: exit ${rc}\n${out${pass}}${err}")
      endif()
    endforeach()
    if(NOT out1 STREQUAL out2)
      message(FATAL_ERROR "muhtari-run ${ARGN}: two runs differ\n${out1}--\n${out2}")
    endif()
    set(${var} "${out1}" PARENT_SCOPE)
  endfunction()

  # Sets var to the frame 8 hash of a 12-line movie with header that holds
  # line on lines 5 to 10, run twice on the input cart. The other lines are
  # ARGV4 when given, else the released ProLine line.
  function(held_hash var name header line)
    set(rest "${released}")
    if(ARGC GREATER 4)
      set(rest "${ARGV4}")
    endif()
    write_movie(${name} "${header}" "4:${rest}" "6:${line}" "2:${rest}")
    run_twice(out --hash-frame 8 --movie "${movie}" "${DIR}/input.a78")
    string(REGEX MATCH "frame 8 sha256 [0-9a-f]+" hash "${out}")
    set(${var} "${hash}" PARENT_SCOPE)
  endfunction()

  # Fails unless the values of ARGN are all different.
  function(expect_distinct what)
    set(seen "")
    foreach(v IN LISTS ARGN)
      if(v STREQUAL "" OR v IN_LIST seen)
        message(FATAL_ERROR "${what}: expected distinct frame hashes, got\n${ARGN}")
      endif()
      list(APPEND seen "${v}")
    endforeach()
  endfunction()

  set(usage_line "usage: muhtari-run ")
  set(prolines "port1 proline\nport2 proline\n")
  set(released "|......|......|...BB|")
  if(CASE STREQUAL "help")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --help
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
    foreach(claim IN ITEMS "muhtari-movie 1 or 2" lightgun paddle trakball driving keypad
        st-mouse amiga-mouse snes2atari mega7800
        "Controller models are uncalibrated; see docs/hardware/controllers.md")
      string(FIND "${out}" "${claim}" found)
      if(NOT rc STREQUAL "0" OR NOT err STREQUAL "" OR found EQUAL -1)
        message(FATAL_ERROR "runner help lacks executed support or model qualification: ${claim}")
      endif()
    endforeach()
  elseif(CASE STREQUAL "hash")
    # Fault prevented: the runner's printed hash, the line hosts and CI
    # compare, drifts from the pinned test card or exits non-zero, while the
    # library test still passes.
    expect_output("frame 1 sha256 ${card}\n" --frames 1 --hash-frame 1)
  elseif(CASE STREQUAL "hash.repeat")
    # Fault prevented: repeated or unordered --hash-frame options print
    # duplicate or out-of-order lines, so a host comparing line by line
    # sees a mismatch.
    expect_output("frame 1 sha256 ${card}\nframe ${FRAMES} sha256 ${card}\n"
      --frames ${FRAMES} --hash-frame ${FRAMES} --hash-frame 1 --hash-frame 1)
  elseif(CASE STREQUAL "dump_png")
    # Fault prevented: the PNG the runner dumps, the image compared with
    # RetroArch's screenshot, changes bytes (palette, layout or writer) while
    # the native-frame hash still passes. The pin is the PNG of the pinned
    # test card through the warm NTSC palette.
    set(want_sha256 "1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956")
    file(REMOVE "${OUT}")
    expect_output("" --frames 1 --dump-frame "1:${OUT}")
    file(SIZE "${OUT}" size)
    file(SHA256 "${OUT}" sha256)
    if(NOT size STREQUAL "232645" OR NOT sha256 STREQUAL want_sha256)
      message(FATAL_ERROR "expected 232645 bytes with SHA-256 ${want_sha256}\n"
        "got ${size} bytes with SHA-256 ${sha256}")
    endif()
  elseif(CASE STREQUAL "version")
    # Fault prevented: --version reports a version other than the release's.
    file(STRINGS "${VERSION_FILE}" version LIMIT_COUNT 1)
    expect_output("muhtari-run ${version}\n" --version)
  # Fault prevented for the usage cases: an invalid command line runs
  # anyway, or fails with a status other than 2, so a script cannot tell a
  # typo from a failed run.
  elseif(CASE STREQUAL "usage.no_args")
    expect_exit(2 "${usage_line}")
  elseif(CASE STREQUAL "usage.unknown")
    expect_exit(2 "unknown option.*${usage_line}" --nope)
  elseif(CASE STREQUAL "usage.positional")
    # One FILE is a cartridge; a second is not.
    expect_exit(2 "unexpected argument.*${usage_line}" --frames 1 a.a78 b.a78)
    expect_exit(2 "unexpected argument.*${usage_line}" --frames 1 --rom a.a78 b.a78)
    expect_exit(2 "--rom needs a FILE.*${usage_line}" --frames 1 --rom)
  elseif(CASE STREQUAL "load.hash")
    # Fault prevented: a cartridge given on the command line is not loaded,
    # so the run shows the test card, or runs differently from one run to
    # the next.
    set(args --frames 120 --hash-frame 120 "${DIR}/linear-v3-16k")
    execute_process(COMMAND ${EMULATOR} "${RUN}" ${args}
      OUTPUT_VARIABLE first ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT first MATCHES "^frame 120 sha256 [0-9a-f]+\n$"
        OR first MATCHES "${card}")
      message(FATAL_ERROR "muhtari-run ${args}: expected exit 0 and a frame 120 hash "
        "other than the test card's, got exit ${rc} and output\n${first}${err}")
    endif()
    expect_output("${first}" ${args})
    expect_output("${first}" --frames 120 --hash-frame 120 --rom "${DIR}/linear-v3-16k")
  elseif(CASE STREQUAL "load.pal")
    # Fault prevented: automatic PAL retains NTSC buffer extents or sample
    # counts in the real process. Hash repetition is additional evidence.
    set(args --frames 2 --hash-frame 2 --hash-audio --dump-frame "2:${DIR}/pal.png"
      "${DIR}/linear-v3-16k-pal")
    execute_process(COMMAND ${EMULATOR} "${RUN}" ${args}
      OUTPUT_VARIABLE first ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT first MATCHES "audio_samples 1922\n" OR NOT first MATCHES
        "frame 2 sha256 3aea76b218b6e0492af9bddc2a5609d8453624f1aaaccdc239a38effd91676b0\n")
      message(FATAL_ERROR "PAL run failed: ${rc} ${first}${err}")
    endif()
    # Independently SHA-256 over93440 zero native bytes, as this fixture's
    # jammed/default-background program emits;77440 zeros has a different hash.
    expect_output("${first}" ${args})
    file(READ "${DIR}/pal.png" header OFFSET 16 LIMIT 8 HEX)
    if(NOT header STREQUAL "0000014000000124")
      message(FATAL_ERROR "PAL PNG dimensions: ${header}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 1 --hash-audio
      "${DIR}/linear-v3-16k-pal" OUTPUT_VARIABLE one ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT one MATCHES "audio_samples 960\n")
      message(FATAL_ERROR "PAL first audio count: ${rc} ${one}${err}")
    endif()
  elseif(CASE STREQUAL "usage.frame_range")
    expect_exit(2 "${usage_line}" --frames 0)
    expect_exit(2 "${usage_line}" --frames 1 --hash-frame 0)
    expect_exit(2 "beyond --frames.*${usage_line}" --frames 5 --hash-frame 6)
  elseif(CASE STREQUAL "usage.dump_syntax")
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame 1)
    expect_exit(2 "N:FILE.*${usage_line}" --frames 1 --dump-frame x:f)
  elseif(CASE STREQUAL "exit.failure")
    # Fault prevented: a file the runner cannot write is reported as success.
    expect_exit(1 "cannot open" --frames 1 --dump-frame "1:${OUT}/missing/x.png")
  elseif(CASE STREQUAL "probe.accepted")
    # Fault prevented: --probe misnames a field, drops or reorders a line,
    # or prints a digest of the wrong bytes, so a host or a hash database
    # reads the wrong identity.
    probe(linear-v3-16k)
    expect_keys(linear-v3-16k 0 ${identity_keys} ${header_keys} ${hash_keys})
    string(FIND "${out}" "sha1 " at)
    string(SUBSTRING "${out}" 0 ${at} fields)
    set(want "kind 7800\nsource header\nboard linear\nboard_options none\nrom_size 16384\n"
      "ram_size 0\nsound none\ndevices none\nport1 proline\nport2 proline\nregion ntsc\n"
      "title Clean Linear\na78_version 0x03\na78_rom_size 16384\na78_cart_type 0x0000\n"
      "a78_port1 0x01\na78_port2 0x01\na78_tv 0x00\na78_save 0x00\na78_xm 0x00\n"
      "a78_mapper 0x00\na78_mapper_options 0x00\na78_audio 0x0000\na78_irq 0x00\n")
    string(CONCAT want ${want})
    if(NOT fields STREQUAL want)
      message(FATAL_ERROR "--probe linear-v3-16k: expected\n${want}got\n${fields}")
    endif()
    expect_hashes("" "${DIR}/linear-v3-16k")
    expect_hashes("_headerless" "${DIR}/linear-v3-16k-body")
  # Fault prevented for the refusal cases: a refused image exits with
  # another class's status or message, or prints lines a host would take as
  # the identity of an image the core refused.
  elseif(CASE STREQUAL "probe.2600")
    probe(2600-4k)
    expect_keys(2600-4k 77 kind source rom_size ${hash_keys})
    probe_value(kind kind)
    if(NOT kind STREQUAL "2600" OR NOT err MATCHES "muhtari-run: [^\n]*/2600-4k: unsupported: 2600 \\(planned\\)\n")
      message(FATAL_ERROR "--probe 2600-4k: kind '${kind}', standard error\n${err}")
    endif()
    expect_hashes("" "${DIR}/2600-4k")
  elseif(CASE STREQUAL "probe.unsupported_board")
    probe(souper-v4)
    expect_keys(souper-v4 77 ${identity_keys} ${header_keys} ${hash_keys})
    probe_value(board board)
    if(NOT board STREQUAL "souper" OR NOT err MATCHES "/souper-v4: unsupported board: souper\n")
      message(FATAL_ERROR "--probe souper-v4: board '${board}', standard error\n${err}")
    endif()
    expect_hashes("" "${DIR}/souper-v4")
  elseif(CASE STREQUAL "probe.malformed")
    foreach(name IN ITEMS size-zero bad-magic-1000)
      probe(${name})
      expect_keys(${name} 1 ${hash_keys})
      if(NOT err MATCHES "/${name}: not a 7800 or 2600 cartridge image\n")
        message(FATAL_ERROR "--probe ${name}: standard error\n${err}")
      endif()
      expect_hashes("" "${DIR}/${name}")
    endforeach()
  elseif(CASE STREQUAL "probe.truncated")
    foreach(name IN ITEMS header-only supergame-short)
      probe(${name})
      expect_keys(${name} 1 ${hash_keys})
      if(NOT err MATCHES "/${name}: the image ends before the size its header gives\n")
        message(FATAL_ERROR "--probe ${name}: standard error\n${err}")
      endif()
    endforeach()
  elseif(CASE STREQUAL "probe.oversized")
    probe(oversized)
    if(NOT rc STREQUAL "1" OR NOT out STREQUAL ""
        OR NOT err MATCHES "/oversized: larger than 1 MiB plus a 128-byte header\n")
      message(FATAL_ERROR "--probe oversized: expected exit 1 and no output, "
        "got exit ${rc}, output\n${out}standard error\n${err}")
    endif()
  elseif(CASE STREQUAL "probe.usage")
    # --probe is the whole command line: one file, no other option.
    set(image "${DIR}/linear-v3-16k")
    expect_exit(2 "--probe takes one file.*${usage_line}" --probe "${image}" --frames 1)
    expect_exit(2 "--probe takes one file.*${usage_line}" --frames 1 --probe "${image}")
    expect_exit(2 "--probe takes one file.*${usage_line}" --probe)
    expect_exit(2 "--probe takes one file.*${usage_line}" --probe "${image}" "${image}")
    expect_exit(2 "--probe takes one file.*${usage_line}" --probe --help)
  elseif(CASE STREQUAL "probe.unreadable")
    # Fault prevented: a missing file is reported without its name, or as
    # an image the core refused.
    expect_exit(1 "^muhtari-run: [^\n]*/no-such-image: cannot open\n$" --probe "${DIR}/no-such-image")
  elseif(CASE STREQUAL "probe.headerless_rule")
    # Fault prevented: the headerless hashes drop the header of a file that
    # is nothing but a header, where rcheevos hashes the whole file.
    probe(magic-only-128)
    expect_keys(magic-only-128 1 ${hash_keys})
    expect_hashes("" "${DIR}/magic-only-128")
    expect_hashes("_headerless" "${DIR}/magic-only-128")
  # Fault prevented for the sample cases: --probe passes on images the tests
  # write but misreads a real one, a 7800basic build made by the published
  # tools (tests/built/).
  elseif(CASE STREQUAL "probe.sample_a78")
    probe(simple.bas.a78)
    expect_keys(simple.bas.a78 0 ${identity_keys} ${header_keys} ${hash_keys})
    string(FIND "${out}" "sha1 " at)
    string(SUBSTRING "${out}" 0 ${at} fields)
    # The title is "My Game" padded with spaces to 32 bytes, and the magic
    # padded likewise: neither is a warning.
    set(want "kind 7800\nsource header\nboard linear\nboard_options none\nrom_size 32768\n"
      "ram_size 0\nsound none\ndevices none\nport1 proline\nport2 proline\nregion ntsc\n"
      "title My Game\na78_version 0x04\na78_rom_size 32768\na78_cart_type 0x0000\n"
      "a78_port1 0x01\na78_port2 0x01\na78_tv 0x00\na78_save 0x00\na78_xm 0x00\n"
      "a78_mapper 0x00\na78_mapper_options 0x00\na78_audio 0x0000\na78_irq 0x00\n")
    string(CONCAT want ${want})
    if(NOT fields STREQUAL want)
      message(FATAL_ERROR "--probe simple.bas.a78: expected\n${want}got\n${fields}")
    endif()
    expect_hashes("" "${DIR}/simple.bas.a78")
    # The .bin is the .a78 without its 128-byte header.
    expect_hashes("_headerless" "${DIR}/simple.bas.bin")
  elseif(CASE STREQUAL "probe.sample_headerless" OR CASE STREQUAL "probe.sample_hash_match")
    # The stripped sample's last 8 bytes: $FFF8 = $FF and $FFF9 = $F7, which
    # the footer test requires (docs/hardware/cartridge.md, "The footer
    # test"), then the NMI vector $F000, reset $F4AD and IRQ $F05F.
    file(READ "${DIR}/simple.bas.bin" footer OFFSET 32760 HEX)
    if(NOT footer STREQUAL "fff700f0adf45ff0")
      message(FATAL_ERROR "simple.bas.bin: footer ${footer}, not fff700f0adf45ff0")
    endif()
    probe(simple.bas.bin)
    expect_keys(simple.bas.bin 0 ${identity_keys} ${hash_keys})
    set(want_source footer)
    if(CASE STREQUAL "probe.sample_hash_match")
      # The test list's entry for this SHA-1: linear, NTSC.
      set(want_source hash)
    endif()
    foreach(pair IN ITEMS "source ${want_source}" "board linear" "region ntsc"
        "sha1 1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a" "md5 948e2565fefcbb635cd8b73ea9b29941"
        "crc32 55f0312b")
      string(REPLACE " " ";" pair "${pair}")
      list(GET pair 0 key)
      list(GET pair 1 want)
      probe_value(${key} got)
      if(NOT got STREQUAL want)
        message(FATAL_ERROR "--probe simple.bas.bin: ${key} is '${got}', not '${want}'\n${out}${err}")
      endif()
    endforeach()
    expect_hashes("" "${DIR}/simple.bas.bin")
    if(CASE STREQUAL "probe.sample_hash_match")
      set(DIR "${PROBE_DIR}")
      foreach(layout IN ITEMS om am)
        foreach(header IN ITEMS "" "-a78")
          set(name "activision-${layout}${header}")
          set(orientation FALSE)
          if(layout STREQUAL "am")
            set(orientation TRUE)
          endif()
          probe(${name})
          activision_probe_valid("${out}" "${DIR}/${name}" "${DIR}/activision-${layout}"
            hash ${orientation} FALSE valid)
          if(NOT rc STREQUAL "0" OR NOT err STREQUAL "" OR NOT valid)
            message(FATAL_ERROR "Activision raw orientation/identity mismatch: ${name}\n${out}${err}")
          endif()
          if(NOT header STREQUAL "")
            foreach(line IN ITEMS "a78_version 0x04" "a78_cart_type 0x0200"
                "a78_mapper 0x00" "a78_port1 0x05" "port1 joystick")
              if(NOT out MATCHES "(^|\n)${line}\n")
                message(FATAL_ERROR "Activision raw header metadata lost: ${line}\n${out}")
              endif()
            endforeach()
          endif()
        endforeach()
      endforeach()
      # Hashes of original AM/OM bodies must remain distinct.
      file(SHA256 "${DIR}/activision-am" am_digest)
      file(SHA256 "${DIR}/activision-om" om_digest)
      if(am_digest STREQUAL om_digest)
        message(FATAL_ERROR "AM and OM raw identities collapsed")
      endif()
      probe(activision-am-trailer-a78)
      activision_probe_valid("${out}" "${DIR}/activision-am-trailer-a78"
        "${DIR}/activision-am-trailer" hash TRUE FALSE valid)
      if(NOT rc STREQUAL "0" OR NOT valid OR
          NOT out MATCHES "\nwarning size_field_mismatch\n")
        message(FATAL_ERROR "Activision trailer identity or mapped-size mismatch\n${out}${err}")
      endif()
      set(pass "${out}")
      foreach(fault IN ITEMS board orientation hash)
        set(mutant "${pass}")
        if(fault STREQUAL "board")
          string(REPLACE "board activision\n" "board supergame\n" mutant "${mutant}")
        elseif(fault STREQUAL "orientation")
          string(REPLACE "flag activision_half_swapped\n" "" mutant "${mutant}")
        else()
          string(REGEX REPLACE "\nsha256_headerless [^\n]*\n" "\n" mutant "${mutant}")
        endif()
        activision_probe_valid("${mutant}" "${DIR}/activision-am-trailer-a78"
          "${DIR}/activision-am-trailer" hash TRUE FALSE valid)
        if(valid)
          message(FATAL_ERROR "Activision ${fault} probe fault was accepted")
        endif()
      endforeach()
      probe(activision)
      activision_probe_valid("${out}" "${DIR}/activision" "${DIR}/activision-unknown-body"
        header FALSE TRUE valid)
      if(NOT rc STREQUAL "0" OR NOT valid)
        message(FATAL_ERROR "Activision assumed layout was misreported\n${out}${err}")
      endif()
      string(REPLACE "warning activision_layout_assumed\n" "" mutant "${out}")
      activision_probe_valid("${mutant}" "${DIR}/activision" "${DIR}/activision-unknown-body"
        header FALSE TRUE valid)
      if(valid)
        message(FATAL_ERROR "Activision missing assumption warning was accepted")
      endif()
    endif()
  elseif(CASE STREQUAL "movie.v2_edges")
    # Fault prevented: a late fault reaches media, creates/truncates an output
    # or save, identity/conflict checks disappear, or typed EOF repeats motion.
    set(v2_dir "${OUT}/typed-effects")
    file(MAKE_DIRECTORY "${v2_dir}")
    set(v2 "${v2_dir}/movie.txt")
    set(header "muhtari-movie 2\nport1 driving\nport2 none\n")
    set(valid "|driving:0,0,0|none:|...BB|\n")
    set(save "${v2_dir}/input.sav")
    set(png "${v2_dir}/out.png")
    file(WRITE "${v2}" "${header}${valid}|driving:0,0,2|none:|...BB|\n")
    foreach(existing IN ITEMS yes no)
      file(REMOVE "${save}" "${png}")
      if(existing STREQUAL "yes")
        file(WRITE "${save}" "previous valid save bytes")
        file(WRITE "${png}" "previous output")
        file(SHA256 "${save}" save_before)
        file(SHA256 "${png}" png_before)
      endif()
      execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${v2}" --hsc on
        --save-dir "${v2_dir}" --frames 2 --hash-frame 1 --dump-frame "1:${png}"
        "${DIR}/input.a78" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
      if(NOT rc STREQUAL "1" OR NOT out STREQUAL "" OR NOT err MATCHES
          "movie.txt:5:14: decimal out of range, expected the device's range\n$")
        message(FATAL_ERROR "late input fault reached output or wrong diagnostic: ${rc} ${out}${err}")
      endif()
      if(existing STREQUAL "yes")
        file(SHA256 "${save}" save_after)
        file(SHA256 "${png}" png_after)
        if(NOT save_before STREQUAL save_after OR NOT png_before STREQUAL png_after)
          message(FATAL_ERROR "late fault truncated existing save/output")
        endif()
      elseif(EXISTS "${save}" OR EXISTS "${png}")
        message(FATAL_ERROR "late fault created save/output")
      endif()
    endforeach()
    # A nonexistent ROM is not opened before the last row is validated.
    expect_exit(1 "movie.txt:5:14: decimal out of range" --movie "${v2}"
      "${v2_dir}/missing.a78")
    foreach(pair IN ITEMS "driving;0,0,0" "lightgun;319,241,3" "paddle;65535,65535,3" "keypad;4095" "proline;......" "joystick;....."
        "none;" "savekey;" "atarivox;")
      list(GET pair 0 device)
      list(GET pair 1 payload)
      file(WRITE "${v2}" "muhtari-movie 2\nport1 ${device}\nport2 none\n|${device}:${payload}|none:|...BB|\n")
      expect_output("lag_frames 1\n" --movie "${v2}" --save-dir "${v2_dir}" "${DIR}/input.a78")
    endforeach()
    # Every typed device reaches the core rather than a joystick fallback.
    foreach(pair IN ITEMS "trakball;-2147483648,2147483647,1" "st-mouse;2147483647,-2147483648,3"
        "amiga-mouse;-2147483648,-2147483648,3"
        "snes2atari;4095" "mega7800;4095")
      list(GET pair 0 device)
      list(GET pair 1 payload)
      file(WRITE "${v2}" "muhtari-movie 2\nport1 ${device}\nport2 none\n|${device}:${payload}|none:|...BB|\n")
      expect_output("lag_frames 1\n" --movie "${v2}" "${DIR}/input.a78")
    endforeach()
    foreach(pair IN ITEMS "mouse:0;unknown device tag" "none:;device tag mismatch"
        "driving:+1,0,0;not a decimal" "driving:00,0,0;not a canonical decimal"
        "driving:-0,0,0;negative zero" "driving:2147483648,0,0;decimal out of range"
        "driving:-2147483649,0,0;decimal out of range" "driving:0,1,0;decimal out of range"
        "driving:0,0,2;decimal out of range" "driving:0,0;missing delimiter"
        "driving:0,0,0,0;missing delimiter")
      list(GET pair 0 field)
      list(GET pair 1 reason)
      file(WRITE "${v2}" "${header}|${field}|none:|...BB|\n")
      expect_exit(1 "movie.txt:4:[0-9]+: ${reason}" --movie "${v2}" "${DIR}/input.a78")
    endforeach()
    foreach(pair IN ITEMS "lightgun;320,0,2" "lightgun;0,242,2" "lightgun;0,0,4"
        "paddle;65536,0,0" "paddle;0,0,4" "keypad;4096" "snes2atari;4096"
        "mega7800;4096" "trakball;0,0,2" "st-mouse;0,0,4" "amiga-mouse;0,0,4")
      list(GET pair 0 device)
      list(GET pair 1 payload)
      file(WRITE "${v2}" "muhtari-movie 2\nport1 ${device}\nport2 none\n|${device}:${payload}|none:|...BB|\n")
      expect_exit(1 "movie.txt:4:[0-9]+: decimal out of range" --movie "${v2}"
        "${v2_dir}/missing.a78")
    endforeach()
    set(max_field "amiga-mouse:-2147483648,-2147483648,3")
    set(max_header "muhtari-movie 2\nport1 amiga-mouse\nport2 amiga-mouse\n")
    set(max_row "|${max_field}|${max_field}|...BB|")
    string(LENGTH "${max_row}" max_len)
    if(NOT max_len EQUAL 83)
      message(FATAL_ERROR "canonical maximum derivation changed: ${max_len}")
    endif()
    file(WRITE "${v2}" "${max_header}${max_row}\n")
    expect_output("lag_frames 1\n" --movie "${v2}" "${DIR}/input.a78")
    file(WRITE "${v2}" "${max_header}${max_row}x\n")
    expect_exit(1 "movie.txt:4:84: line over 83 bytes" --movie "${v2}" "${v2_dir}/missing.a78")
    file(WRITE "${v2}" "${header}${valid}")
    expect_exit(2 "--port1 joystick conflicts with the movie's port1 driving"
      --port1 joystick --movie "${v2}" --hsc on --save-dir "${v2_dir}" "${DIR}/input.a78")
    if(EXISTS "${save}" OR EXISTS "${png}")
      message(FATAL_ERROR "header conflict created save/output")
    endif()
    probe(input.a78)
    probe_value(sha256_headerless digest)
    string(REPEAT "0" 64 zeros)
    file(WRITE "${v2}" "${header}rom-sha256 ${zeros}\n${valid}")
    expect_exit(1 "rom-sha256 ${zeros}[^\n]* ${digest}" --movie "${v2}"
      --hsc on --save-dir "${v2_dir}" --dump-frame "1:${png}" "${DIR}/input.a78")
    if(EXISTS "${save}" OR EXISTS "${png}")
      message(FATAL_ERROR "identity mismatch created save/output")
    endif()
    file(WRITE "${v2}" "${header}rom-sha256 ${digest}\nbehaviour 999999999\n${valid}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${v2}" --port1 driving --port2 auto
      "${DIR}/input.a78" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL "lag_frames 1\n" OR NOT err MATCHES
        "behaviour 999999999, this core's is [1-9][0-9]*; replaying anyway")
      message(FATAL_ERROR "typed identity/warning path: ${rc} ${out}${err}")
    endif()
    file(WRITE "${v2}" "${header}")
    expect_exit(2 "the movie has no frames: give --frames N" --movie "${v2}" "${DIR}/input.a78")
    expect_output("lag_frames 1\n" --frames 1 --movie "${v2}" "${DIR}/input.a78")
    # EOF and explicit release must produce identical frame and pin evidence.
    set(held "|driving:4,0,1|none:|R..AB|\n")
    set(neutral "|driving:0,0,0|none:|...AB|\n")
    file(WRITE "${v2}" "${header}${valid}${held}")
    run_twice(short --movie "${v2}" --frames 5 --hash-frame 5 "${DIR}/input.a78")
    file(WRITE "${v2}" "${header}${valid}${held}${neutral}${neutral}${neutral}")
    run_twice(long --movie "${v2}" --frames 5 --hash-frame 5 "${DIR}/input.a78")
    if(NOT short STREQUAL long)
      message(FATAL_ERROR "typed EOF differs from explicit release: ${short} ${long}")
    endif()
    foreach(device IN ITEMS snes2atari mega7800)
      foreach(port IN ITEMS 1 2)
        if(port EQUAL 1)
          set(adapters "port1 ${device}\nport2 none\n")
          set(held "|${device}:4095|none:|RSPAB|\n")
          set(released "|${device}:0|none:|...AB|\n")
        else()
          set(adapters "port1 none\nport2 ${device}\n")
          set(held "|none:|${device}:4095|RSPAB|\n")
          set(released "|none:|${device}:0|...AB|\n")
        endif()
        file(WRITE "${v2}" "muhtari-movie 2\n${adapters}${held}")
        run_twice(short --movie "${v2}" --frames 5 --hash-frame 5 --hash-audio "${DIR}/input.a78")
        file(WRITE "${v2}" "muhtari-movie 2\n${adapters}${held}${released}${released}${released}${released}")
        run_twice(long --movie "${v2}" --frames 5 --hash-frame 5 --hash-audio "${DIR}/input.a78")
        if(NOT short STREQUAL long)
          message(FATAL_ERROR "${device} port${port} EOF differs from release: ${short} ${long}")
        endif()
      endforeach()
    endforeach()
  elseif(CASE STREQUAL "movie.lag")
    # Fault prevented: the runner miscounts or drops the frames in which the
    # game read no input. The input cart's even variant reads on even
    # frames only, so 60 frames hold 30 lag frames.
    write_movie(lag.txt "${prolines}" "60:${released}")
    expect_output("lag_frames 30\n" --frames 60 --movie "${movie}" "${DIR}/input-even.a78")
  elseif(CASE STREQUAL "movie.replay")
    # The additive grammar must traverse the real runner, not just parse.
    file(WRITE "${OUT}/typed.txt" "muhtari-movie 2\nport1 proline\nport2 proline\n|proline:U.....|proline:......|...AB|\n|proline:......|proline:......|...AB|\n")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${OUT}/typed.txt"
      --frames 4 --hash-frame 4 "${DIR}/input.a78"
      RESULT_VARIABLE typed_rc OUTPUT_VARIABLE typed ERROR_VARIABLE typed_err)
    if(NOT typed_rc STREQUAL "0" OR NOT typed MATCHES "frame 4 sha256 [0-9a-f]+\nlag_frames 1\n$")
      message(FATAL_ERROR "typed replay expected exit 0 and frame evidence: ${typed_rc}\n${typed}${typed_err}")
    endif()
    file(WRITE "${OUT}/driving.txt" "muhtari-movie 2\nport1 driving\nport2 driving\n|driving:0,0,0|driving:0,0,0|...BB|\n|driving:0,0,0|driving:0,0,0|...BB|\n|driving:4,0,1|driving:-4,0,0|...BB|\n|driving:-1,0,0|driving:1,0,1|...BB|\n|driving:0,0,0|driving:0,0,1|...BB|\n|driving:0,0,0|driving:0,0,0|...BB|\n")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${OUT}/driving.txt"
      --frames 12 --diagnostic "${CMAKE_CURRENT_LIST_DIR}/../diag/controller-motion.a78"
      RESULT_VARIABLE drive_rc OUTPUT_VARIABLE drive ERROR_VARIABLE drive_err)
    string(REGEX MATCH "diagnostic ([0-9a-f]+)\n" found "${drive}")
    set(mailbox "${CMAKE_MATCH_1}")
    # Connector tuples independently derived in docs/hardware/controllers.md.
    set(records "010100000201ff0003018000040180000501ff000601de000701cc000801ed000901ff000a0100000b0180000c01ff000d01ed000e0180000f0100001001ed00110100001201ed0013018000")
    if(NOT drive_rc STREQUAL "0" OR NOT mailbox MATCHES "^4d3744470100130e")
      message(FATAL_ERROR "driving replay mailbox absent: ${drive_rc} ${drive}${drive_err}")
    endif()
    string(SUBSTRING "${mailbox}" 32 152 observed_records)
    if(NOT observed_records STREQUAL records)
      message(FATAL_ERROR "driving replay pin records differ: ${observed_records}")
    endif()
    # Fault prevented: the runner drops or shifts a movie's input, or a
    # replay is not repeatable. The input cart paints what it read, so a
    # frame inside the held lines differs from an empty movie's, and a frame
    # after them, with nothing held, matches it. --frames defaults to the
    # movie's 12 lines.
    write_movie(empty.txt "${prolines}" "12:${released}")
    run_twice(empty --hash-frame 8 --hash-frame 12 --movie "${movie}" "${DIR}/input.a78")
    write_movie(up.txt "${prolines}" "4:${released}" "6:|U.....|......|...BB|" "2:${released}")
    run_twice(up --hash-frame 8 --hash-frame 12 --movie "${movie}" "${DIR}/input.a78")
    string(REGEX MATCHALL "frame [0-9]+ sha256 [0-9a-f]+" empty_hashes "${empty}")
    string(REGEX MATCHALL "frame [0-9]+ sha256 [0-9a-f]+" up_hashes "${up}")
    list(LENGTH up_hashes n)
    if(NOT n EQUAL 2 OR NOT up MATCHES "\nlag_frames 1\n$")
      message(FATAL_ERROR "expected two hash lines and lag_frames 1, got\n${up}")
    endif()
    list(GET empty_hashes 0 empty8)
    list(GET empty_hashes 1 empty12)
    list(GET up_hashes 0 up8)
    list(GET up_hashes 1 up12)
    if(up8 STREQUAL empty8 OR NOT up12 STREQUAL empty12)
      message(FATAL_ERROR "expected frame 8 to differ and frame 12 to match\n${empty}--\n${up}")
    endif()
  elseif(CASE STREQUAL "movie.ports")
    # Fault prevented: a header's device or a port's field reaches the wrong
    # port or the wrong pins. Up and button 1 on each port with each stick
    # give four pictures, none the empty movie's.
    held_hash(empty ports-empty.txt "${prolines}" "${released}")
    held_hash(p1_proline ports-1p.txt "${prolines}" "|....12|......|...BB|")
    held_hash(p1_joystick ports-1j.txt "port1 joystick\nport2 proline\n" "|U...1|......|...BB|"
      "|.....|......|...BB|")
    held_hash(p2_proline ports-2p.txt "${prolines}" "|......|....12|...BB|")
    held_hash(p2_joystick ports-2j.txt "port1 proline\nport2 joystick\n" "|......|U...1|...BB|"
      "|......|.....|...BB|")
    expect_distinct(ports "${empty}" "${p1_proline}" "${p1_joystick}" "${p2_proline}"
      "${p2_joystick}")
  elseif(CASE STREQUAL "movie.switches")
    # Fault prevented: a console switch in a movie's last field never
    # reaches SWCHB, or reaches the wrong bit.
    held_hash(empty sw-empty.txt "${prolines}" "${released}")
    set(hashes "${empty}")
    foreach(field IN ITEMS R..BB .S.BB ..PBB ...AB ...BA)
      held_hash(h sw-${field}.txt "${prolines}" "|......|......|${field}|")
      list(APPEND hashes "${h}")
    endforeach()
    expect_distinct(switches ${hashes})
  elseif(CASE STREQUAL "movie.past_end")
    # Fault prevented: past a movie's last line the runner keeps the last
    # buttons held, or resets the difficulty switches. Twenty frames of a
    # 10-line movie must run as the 20-line movie that spells out a release
    # with the difficulty kept.
    set(held "|U.....|......|...AB|")
    write_movie(short.txt "${prolines}" "4:${released}" "6:${held}")
    run_twice(short --frames 20 --hash-frame 10 --hash-frame 11 --hash-frame 20 --movie "${movie}"
      "${DIR}/input.a78")
    write_movie(long.txt "${prolines}" "4:${released}" "6:${held}" "10:|......|......|...AB|")
    run_twice(long --hash-frame 10 --hash-frame 11 --hash-frame 20 --movie "${movie}"
      "${DIR}/input.a78")
    string(REGEX MATCH "frame 10 sha256 [0-9a-f]+" f10 "${short}")
    string(REGEX MATCH "frame 11 sha256 [0-9a-f]+" f11 "${short}")
    string(REPLACE "frame 10" "frame 11" f10 "${f10}")
    if(NOT short STREQUAL long OR NOT short MATCHES "frame 20 sha256" OR f10 STREQUAL f11)
      message(FATAL_ERROR "past the end: expected the long movie's output, with frame 11 "
        "unlike frame 10, got\n${short}--\n${long}")
    endif()
  elseif(CASE STREQUAL "movie.malformed")
    # Fault prevented: a malformed movie runs, or is refused without the
    # place of its fault.
    write_movie(bad.txt "${prolines}" "1:|X.....|......|...BB|")
    expect_exit(1 "^muhtari-run: [^\n]*/bad.txt:4:2: letter out of place, expected U or [.]\n$"
      --movie "${movie}" "${DIR}/input.a78")
  elseif(CASE STREQUAL "movie.rom_mismatch")
    # Fault prevented: a movie recorded on another cartridge replays on this
    # one. The digest is the headerless SHA-256 that --probe prints.
    probe(input.a78)
    probe_value(sha256_headerless digest)
    string(REPEAT "0" 64 zeros)
    write_movie(other.txt "rom-sha256 ${zeros}\n${prolines}" "1:${released}")
    expect_exit(1 "rom-sha256 ${zeros}[^\n]* ${digest}\n$" --movie "${movie}" "${DIR}/input.a78")
    write_movie(same.txt "rom-sha256 ${digest}\n${prolines}" "1:${released}")
    expect_output("lag_frames 1\n" --movie "${movie}" "${DIR}/input.a78")
  elseif(CASE STREQUAL "movie.conflict")
    # Fault prevented: the command line silently overrides a movie's header,
    # so a replay runs with devices other than the recording's. The header
    # wins; an agreeing or auto option is no conflict.
    write_movie(conflict.txt "${prolines}region ntsc\n" "2:${released}")
    expect_exit(2 "--port1 joystick conflicts with the movie's port1 proline.*${usage_line}"
      --port1 joystick --movie "${movie}" "${DIR}/input.a78")
    expect_exit(2 "--port2 none conflicts with the movie's port2 proline.*${usage_line}"
      --port2 none --movie "${movie}" "${DIR}/input.a78")
    expect_output("lag_frames 1\n" --port1 proline --port2 auto --region ntsc --movie "${movie}"
      "${DIR}/input.a78")
  elseif(CASE STREQUAL "movie.behaviour_warning")
    # Fault prevented: a movie recorded on another behaviour revision
    # replays without notice, or is refused although it may still match.
    write_movie(rev9.txt "${prolines}behaviour 9\n" "2:${released}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${movie}" "${DIR}/input.a78"
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT out STREQUAL "lag_frames 1\n" OR NOT err MATCHES
        "^muhtari-run: [^\n]*/rev9.txt: behaviour 9, this core's is [1-9][0-9]*; replaying anyway\n$")
      message(FATAL_ERROR "expected exit 0, lag_frames 1 and one warning, got exit ${rc}\n"
        "${out}${err}")
    endif()
  elseif(CASE STREQUAL "movie.boot_bios")
    # Fault prevented: a movie recorded through the BIOS replays fast-booted,
    # or the BIOS file's digest goes unchecked. The header's boot bios and
    # the file's own digest boot simple through OpenBIOS with no --boot.
    file(SHA256 "${BIOS}" digest)
    write_movie(boot_bios.txt "${prolines}boot bios\nbios-sha256 ${digest}\n" "400:${released}")
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${movie}" --bios "${BIOS}" "${ROM}"
      OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT err STREQUAL "bios_sha256 ${digest}\n")
      message(FATAL_ERROR "expected exit 0 and the BIOS digest, got exit ${rc}\n${out}${err}")
    endif()
    expect_handoff("${out}")
  elseif(CASE STREQUAL "movie.bios_mismatch")
    # Fault prevented: a movie recorded with another BIOS replays, or runs
    # frames before the refusal. Both digests are named and no frame runs.
    file(SHA256 "${BIOS}" digest)
    string(REPEAT "0" 64 zeros)
    write_movie(bios_other.txt "${prolines}boot bios\nbios-sha256 ${zeros}\n" "2:${released}")
    expect_exit(1 "^bios_sha256 ${digest}\nmuhtari-run: [^\n]*bios_other.txt: bios-sha256 ${zeros} [^\n]*${digest}\n$"
      --hash-frame 1 --movie "${movie}" --bios "${BIOS}" "${ROM}")
  elseif(CASE STREQUAL "movie.boot_bios_no_file")
    # Fault prevented: a movie's boot bios runs fast-booted when no BIOS is
    # given.
    string(REPEAT "0" 64 zeros)
    write_movie(bios_nofile.txt "${prolines}boot bios\nbios-sha256 ${zeros}\n" "2:${released}")
    expect_exit(2 "the movie's boot bios needs --bios FILE.*${usage_line}"
      --movie "${movie}" "${ROM}")
  elseif(CASE STREQUAL "movie.boot_conflict")
    # Fault prevented: --boot silently overrides a movie's boot key, as the
    # port and region options may not.
    file(SHA256 "${BIOS}" digest)
    write_movie(boot_b.txt "${prolines}boot bios\nbios-sha256 ${digest}\n" "2:${released}")
    expect_exit(2 "--boot fast conflicts with the movie's boot bios.*${usage_line}"
      --boot fast --movie "${movie}" --bios "${BIOS}" "${ROM}")
    write_movie(boot_f.txt "${prolines}boot fast\n" "2:${released}")
    expect_exit(2 "--boot bios conflicts with the movie's boot fast.*${usage_line}"
      --boot bios --bios "${BIOS}" --movie "${movie}" "${ROM}")
  elseif(CASE STREQUAL "bios.rejected")
    # Fault prevented: a cartridge the BIOS sends down its 2600 path runs on
    # or exits as a failure. The input cart's $FFF8 is 0, which OpenBIOS's
    # region check refuses.
    expect_exit(77 "^bios_sha256 [0-9a-f]+\nmuhtari-run: [^\n]*input.a78: the BIOS rejected the cartridge\n$"
      --frames 60 --boot bios --bios "${BIOS}" "${DIR}/input.a78")
  elseif(CASE STREQUAL "bios.invalid")
    # Fault prevented: a BIOS file of the wrong size boots, or is refused
    # without naming the file.
    string(REPEAT "x" 1000 bytes)
    file(WRITE "${OUT}/bios1000.bin" "${bytes}")
    expect_exit(1 "bios1000.bin: not a 4 KiB or 16 KiB BIOS image\n$"
      --frames 1 --boot bios --bios "${OUT}/bios1000.bin" "${DIR}/input.a78")
  elseif(CASE STREQUAL "usage.boot_bios_no_file")
    expect_exit(2 "--boot bios needs --bios FILE.*${usage_line}" --frames 1 --boot bios)
    expect_exit(2 "--bios needs --boot bios.*${usage_line}" --frames 1 --bios x.bin)
    expect_exit(2 "--boot needs fast or bios.*${usage_line}" --frames 1 --boot slow)
    expect_output("" --frames 1 --boot fast)
  elseif(CASE STREQUAL "usage.port")
    expect_exit(2 "--port1 needs auto, proline, joystick, none, savekey or atarivox.*${usage_line}"
      --frames 1 --port1 mouse)
    expect_exit(2 "--port2 needs auto, proline, joystick, none, savekey or atarivox.*${usage_line}" --frames 1 --port2)
    expect_output("" --frames 1 --port1 joystick --port2 none)
  elseif(CASE STREQUAL "region.pal")
    # Fault prevented: explicit selection diverges from automatic PAL, metadata
    # describes the header instead of the session, or old output gains fields.
    set(pal_info "region pal\nwidth 320\nheight 292\nfps_num 3546894\nfps_den 71051\npar_num 1843750\npar_den 1773447\npalette warm_pal\n")
    set(ntsc_info "region ntsc\nwidth 320\nheight 242\nfps_num 39375000\nfps_den 656711\npar_num 6\npar_den 7\npalette warm_ntsc\n")
    expect_output("${pal_info}" --frames 1 --info "${DIR}/linear-v3-16k-pal")
    expect_output("${pal_info}" --info --frames 1 --region pal "${DIR}/linear-v3-16k")
    expect_output("${pal_info}" --region pal --frames 1 --info "${DIR}/linear-v3-16k-pal")
    expect_output("${ntsc_info}" --frames 1 --region ntsc --info "${DIR}/linear-v3-16k-pal")
    expect_output("${ntsc_info}" --frames 1 --region pal --info)
    expect_output("" --frames 1 --region pal "${DIR}/linear-v3-16k")
    expect_exit(2 "duplicate --info" --frames 1 --info --info)
    expect_exit(2 "--region needs auto, ntsc or pal" --frames 1 --region secam)
    expect_output("" --frames 1 --region ntsc)
  else()
    message(FATAL_ERROR "unknown CASE '${CASE}'")
  endif()
  return()
endif()

# Every script that starts muhtari-run gets the emulator, so an emulated lane
# never runs a host binary or fails to start a guest one.
string(REPLACE ";" "$<SEMICOLON>" runner_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
set(runner_check ${CMAKE_COMMAND} -DRUN=$<TARGET_FILE:muhtari-run> "-DEMULATOR=${runner_emulator}")
foreach(case IN ITEMS hash hash.repeat help usage.no_args usage.unknown usage.positional
    usage.frame_range usage.dump_syntax)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -DFRAMES=${MUHTARI_LONG_RUN} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 10)
endforeach()

muhtari_add_test(NAME runner.dump_png
  COMMAND ${runner_check} -DCASE=dump_png -DOUT=${PROJECT_BINARY_DIR}/tests/dump.png
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.exit.failure
  COMMAND ${runner_check} -DCASE=exit.failure -DOUT=${PROJECT_BINARY_DIR}/tests
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)
muhtari_add_test(NAME runner.version
  COMMAND ${runner_check} -DCASE=version -DVERSION_FILE=${PROJECT_SOURCE_DIR}/version.txt
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 10)

# Movies replayed on the input cart that muhtari-test-input writes into the
# build tree (tests/input), each case writing its own movies into OUT.
set(runner_movie_dir ${PROJECT_BINARY_DIR}/tests/runner-movies)
file(MAKE_DIRECTORY ${runner_movie_dir})
foreach(case IN ITEMS lag replay ports switches past_end malformed rom_mismatch conflict v2_edges
    behaviour_warning)
  muhtari_add_test(NAME runner.movie.${case}
    COMMAND ${runner_check} -DCASE=movie.${case} -DDIR=${input_cart_dir} -DOUT=${runner_movie_dir}
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 3) # Family: 2 x 1.05 s, Windows x64, 7 runs; provisional, run 39617761785
  set_tests_properties(runner.movie.${case} PROPERTIES FIXTURES_REQUIRED input_carts)
endforeach()

# Movies and runs that boot through OpenBIOS, which built.openbios
# assembles, with simple, which built.simple builds, or the input cart.
set(runner_bios ${PROJECT_BINARY_DIR}/built/openbios/openbios.bin)
set(runner_simple ${PROJECT_BINARY_DIR}/built/simple/simple.bas.a78)
foreach(case IN ITEMS movie.boot_bios movie.bios_mismatch movie.boot_bios_no_file
    movie.boot_conflict bios.rejected bios.invalid)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -DDIR=${input_cart_dir} -DOUT=${runner_movie_dir}
      -DBIOS=${runner_bios} -DROM=${runner_simple} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 3) # 2 x 1.10 s, macOS x64, slowest of 13 runs, run 39382016958
  set_tests_properties(runner.${case} PROPERTIES
    FIXTURES_REQUIRED "input_carts;built_openbios;built_simple")
endforeach()

# Boot to play: spacephysics, which built.spacephysics builds, played from
# the fast boot through muhtari-run (boot_to_play.cmake) and judged by
# muhtari-test-boot-to-play from the hashes and dumps it records. F is the
# frame after the ledger's title@120, H the frames held, and L the bound on
# the response: the game draws thrust one frame after the input, measured.
# Fault prevented: a game that boots and shows its title but does not
# respond to a held control, or responds late, or never moves.
add_executable(muhtari-test-boot-to-play
  ${CMAKE_CURRENT_LIST_DIR}/boot_to_play.c
  ${CMAKE_CURRENT_LIST_DIR}/png_read.c
  ${PROJECT_SOURCE_DIR}/runner/sha256.c)
target_link_libraries(muhtari-test-boot-to-play PRIVATE muhtari::muhtari)
# MSVC deprecates ISO C's fopen for its _s forms.
target_compile_definitions(muhtari-test-boot-to-play PRIVATE
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-boot-to-play PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-boot-to-play)
set(runner_play_dir ${PROJECT_BINARY_DIR}/tests/boot-to-play)
set(runner_play_f 121)
set(runner_play_h 60)
set(runner_play_l 1)
muhtari_add_test(NAME runner.boot_to_play
  COMMAND ${CMAKE_COMMAND} -DRUNNER=$<TARGET_FILE:muhtari-run>
    -DJUDGE=$<TARGET_FILE:muhtari-test-boot-to-play> "-DEMULATOR=${runner_emulator}"
    -DROM=${PROJECT_BINARY_DIR}/built/spacephysics/spacephysics.bas.a78 -DOUT=${runner_play_dir}
    -DF=${runner_play_f} -DH=${runner_play_h} -DL=${runner_play_l} -P ${CMAKE_CURRENT_LIST_DIR}/boot_to_play.cmake
  LABELS runner portable TIMEOUT 9) # 2 x 4.21 s, macOS x64, slowest of 9 runs, run 39382016958
set_tests_properties(runner.boot_to_play PROPERTIES
  FIXTURES_REQUIRED built_spacephysics FIXTURES_SETUP boot_to_play)
# The negative controls judge runner.boot_to_play's recorded runs, so each
# must fail with exit 1 and its exact mask or message. The two mutants
# transform the recorded output and never change the emulator. Fault
# prevented: a judge that passes a run with no response, the input on the
# wrong port, a direction whose mirror moves the same way, a picture that
# never moves, or a response one frame late.
function(runner_play_control name regex)
  set(cmd ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:muhtari-test-boot-to-play> ${ARGN})
  string(REPLACE ";" "$<SEMICOLON>" cmd "${cmd}")
  muhtari_add_test(NAME runner.boot_to_play.${name}
    COMMAND ${CMAKE_COMMAND} "-DCMD=${cmd}" -DCODE=1 "-DREGEX=${regex}"
      -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
    LABELS runner portable TIMEOUT 1) # 2 x 0.13 s, Windows arm64, slowest of 9 runs, run 39381705491
  set_tests_properties(runner.boot_to_play.${name} PROPERTIES FIXTURES_REQUIRED boot_to_play)
endfunction()
set(runner_play ${runner_play_dir} ${runner_play_f} ${runner_play_h})
runner_play_control(no_input "^mask 9:" play ${runner_play} ${runner_play_l} none none)
runner_play_control(port2 "^mask 9:" play ${runner_play} ${runner_play_l} none port2)
runner_play_control(mirrored "^sign did not flip:" mirrored ${runner_play} none input input up)
runner_play_control(static_mutant "^mask 2:" static ${runner_play} ${runner_play_l} none input)
runner_play_control(late_mutant "^mask 1:" late ${runner_play} ${runner_play_l} none input)

# The device, region and boot options.
foreach(case IN ITEMS usage.port region.pal usage.boot_bios_no_file)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -DDIR=${CMAKE_BINARY_DIR}/runner-probe
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 1) # 2 x 0.16 s, macOS x64, slowest of 17 runs, run 39377950148
endforeach()
set_tests_properties(runner.region.pal PROPERTIES FIXTURES_REQUIRED probe_images)

# --probe on the images muhtari-test-images writes into the build directory;
# no image bytes are tracked.
set(runner_probe_dir ${CMAKE_BINARY_DIR}/runner-probe)
file(MAKE_DIRECTORY ${runner_probe_dir})
muhtari_add_test(NAME runner.probe.images
  COMMAND muhtari-test-images ${runner_probe_dir}
  LABELS runner portable TIMEOUT 30)
set_tests_properties(runner.probe.images PROPERTIES FIXTURES_SETUP probe_images)
# A cartridge FILE loaded and run: its hash is stable, and a PAL one is
# refused.
foreach(case IN ITEMS hash pal)
  muhtari_add_test(NAME runner.load.${case}
    COMMAND ${runner_check} -DCASE=load.${case} -DDIR=${runner_probe_dir}
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 1) # 2 x 0.35 s, macOS x64, slowest of 31 runs, run 39377950148
  set_tests_properties(runner.load.${case} PROPERTIES FIXTURES_REQUIRED probe_images)
endforeach()
foreach(case IN ITEMS accepted 2600 unsupported_board malformed truncated oversized usage
    unreadable headerless_rule)
  muhtari_add_test(NAME runner.probe.${case}
    COMMAND ${runner_check} -DCASE=probe.${case} -DDIR=${runner_probe_dir}
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 10)
  if(NOT case MATCHES "^(usage|unreadable)$")
    set_tests_properties(runner.probe.${case} PROPERTIES FIXTURES_REQUIRED probe_images)
  endif()
endforeach()

# muhtari-run-testdb: the runner's own sources and the core's objects with
# tests/cart/test_cartdb.c in place of src/cartdb.c (link-time
# substitution), so --probe can show a hash match end to end while the
# shipped runner never holds a test entry. A test program: never installed
# or packaged.
get_target_property(runner_sources muhtari-run SOURCES)
add_executable(muhtari-run-testdb
  ${runner_sources}
  ${PROJECT_SOURCE_DIR}/tests/cart/test_cartdb.c
  $<TARGET_OBJECTS:muhtari_objs>
  $<TARGET_OBJECTS:muhtari_cpu>)
target_include_directories(muhtari-run-testdb PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(muhtari-run-testdb PRIVATE muhtari_host)
target_compile_definitions(muhtari-run-testdb PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-run-testdb PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-run-testdb)

# --probe on 7800basic's simple sample, which built.simple builds into the
# build tree (tests/built/): headered and stripped through the shipped
# runner, which finds the stripped one by its footer, and stripped through
# muhtari-run-testdb, which finds it in the test hash list.
set(runner_sample_dir ${PROJECT_BINARY_DIR}/built/simple)
foreach(case IN ITEMS sample_a78 sample_headerless sample_hash_match)
  set(runner_program $<TARGET_FILE:muhtari-run>)
  if(case STREQUAL "sample_hash_match")
    set(runner_program $<TARGET_FILE:muhtari-run-testdb>)
  endif()
  muhtari_add_test(NAME runner.probe.${case}
    COMMAND ${CMAKE_COMMAND} -DRUN=${runner_program} "-DEMULATOR=${runner_emulator}"
      -DCASE=probe.${case} -DDIR=${runner_sample_dir} -DPROBE_DIR=${runner_probe_dir}
      -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS built portable TIMEOUT 10)
  set_tests_properties(runner.probe.${case} PROPERTIES FIXTURES_REQUIRED built_samples)
  if(case STREQUAL "sample_hash_match")
    set_tests_properties(runner.probe.${case} PROPERTIES FIXTURES_REQUIRED "built_samples;probe_images")
  endif()
endforeach()

# hashes.txt, the file CI compares across lanes.
set(runner_hashes ${PROJECT_SOURCE_DIR}/tests/cmake/hashes.cmake)
muhtari_add_test(NAME runner.hashes_file
  COMMAND ${runner_check} -DCASE=hashes -DOUT=${PROJECT_BINARY_DIR} -DHASHES=${runner_hashes}
    -DDRIVER=$<TARGET_FILE:muhtari-test-diag> -DDIAG=${PROJECT_SOURCE_DIR}/tests/diag
    "-DSOUNDTEST=${PROJECT_BINARY_DIR}/built/soundtest/soundtest.bas.a78"
    "-DPOKEY=${PROJECT_BINARY_DIR}/built/pokey/pokey.bas.a78" -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner portable TIMEOUT 20)
set_tests_properties(runner.hashes_file PROPERTIES FIXTURES_REQUIRED "built_soundtest;built_pokey")
# The comparison's self tests: two files where three are required, and two
# files one byte apart.
set(runner_fixture ${PROJECT_BINARY_DIR}/tests/hashes)
file(WRITE ${runner_fixture}/a.txt "frame1 00\n")
file(WRITE ${runner_fixture}/b.txt "frame1 01\n")
# CMake wraps long messages, so the regexes allow a break between words.
foreach(case IN ITEMS "count;3;a;expected 3 files,[^/]*got 2" "byte;2;b;b.txt[^/]*differs")
  list(GET case 0 name)
  list(GET case 1 count)
  list(GET case 2 second)
  list(GET case 3 regex)
  set(runner_cmd ${CMAKE_COMMAND} "-DFILES=${runner_fixture}/a.txt\\;${runner_fixture}/${second}.txt"
    -DCOUNT=${count} -DMODE=compare -P ${runner_hashes})
  string(REPLACE ";" "$<SEMICOLON>" runner_cmd "${runner_cmd}")
  muhtari_add_test(NAME runner.hashes_compare.${name}.self_test
    COMMAND ${CMAKE_COMMAND} "-DCMD=${runner_cmd}" -DCODE=1 "-DREGEX=${regex}"
      -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
    LABELS runner TIMEOUT 10)
endforeach()

# The PNG writer and its checksums, checked by an independent parse.
add_executable(muhtari-test-png
  ${CMAKE_CURRENT_LIST_DIR}/test_png.c
  ${CMAKE_CURRENT_LIST_DIR}/png_read.c
  ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c)
target_link_libraries(muhtari-test-png PRIVATE muhtari::muhtari muhtari_host)
set_target_properties(muhtari-test-png PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-png)
muhtari_add_test(NAME runner.png
  COMMAND muhtari-test-png ${PROJECT_BINARY_DIR}/tests/png_test.png
  LABELS runner portable TIMEOUT 10)

# The bench statistics, the stop flag and MD5.
add_executable(muhtari-test-runner
  ${CMAKE_CURRENT_LIST_DIR}/test_runner.c
  ${PROJECT_SOURCE_DIR}/runner/bench.c
  ${PROJECT_SOURCE_DIR}/runner/md5.c
  ${PROJECT_SOURCE_DIR}/runner/clock.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c
  ${PROJECT_SOURCE_DIR}/runner/movie.c
  ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-runner PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-runner PRIVATE muhtari::muhtari muhtari_host)
target_compile_definitions(muhtari-test-runner PRIVATE _POSIX_C_SOURCE=200809L
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-runner PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-runner)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/movie-owner)
# Fault prevented: replay rereads mutable bytes, repeats relative fields,
# skips first Auto, leaks on allocation refusal or allocates a packet array.
muhtari_add_test(NAME runner.movie.owner COMMAND muhtari-test-runner movie
  ${PROJECT_BINARY_DIR}/tests/movie-owner ${input_cart_dir}/input.a78
  LABELS runner portable TIMEOUT 10)
set_tests_properties(runner.movie.owner PROPERTIES FIXTURES_REQUIRED input_carts)
muhtari_add_test(NAME runner.bench.percentile COMMAND muhtari-test-runner percentile
  LABELS runner bench portable TIMEOUT 10)
# Fault prevented: on a 32-bit size_t, a large --bench count wraps the
# sample buffer's size, and the bench writes past a too-small allocation.
muhtari_add_test(NAME runner.bench.bytes COMMAND muhtari-test-runner bytes
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.bench.format COMMAND muhtari-test-runner format
  LABELS runner bench portable TIMEOUT 10)
muhtari_add_test(NAME runner.stop COMMAND muhtari-test-runner stop
  LABELS runner portable TIMEOUT 10)
# Fault prevented: a wrong MD5 constant or byte order changes the
# RetroAchievements hash --probe prints, with nothing else to notice it.
muhtari_add_test(NAME runner.md5 COMMAND muhtari-test-runner md5
  LABELS runner portable TIMEOUT 10)
# The muhtari-movie 1 reader, linked alone: good movies and about thirty
# bad ones, each refused at its exact line and column.
add_executable(muhtari-test-movie
  ${CMAKE_CURRENT_LIST_DIR}/test_movie.c
  ${PROJECT_SOURCE_DIR}/runner/movie.c)
target_link_libraries(muhtari-test-movie PRIVATE muhtari::muhtari)
set_target_properties(muhtari-test-movie PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-movie)
# Fault prevented: a malformed or hostile movie runs part way, or is
# refused at the wrong place, before the whole file is checked.
muhtari_add_test(NAME runner.movie_parse COMMAND muhtari-test-movie
  LABELS runner portable TIMEOUT 3) # 2 x 1.10 s, macOS x64, slowest of 18 runs, run 39380651205

# Not portable: it reads docs/runner.md from the source tree.
muhtari_add_test(NAME runner.exit.table
  COMMAND ${runner_check} -DDOC=${PROJECT_SOURCE_DIR}/docs/runner.md
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/exit_table.cmake
  LABELS runner TIMEOUT 10)

# Fault prevented: hashes serialize different bytes from the public core.
muhtari_add_test(NAME runner.audio.contract
  COMMAND ${CMAKE_COMMAND} "-DRUN=$<TARGET_FILE:muhtari-run>"
    "-DREFERENCE=$<TARGET_FILE:muhtari-test-audio-contracts>"
    "-DCAPTURE=${PROJECT_BINARY_DIR}/tests/audio-reference"
    "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}" -DCASE=audio
    "-DROM=${PROJECT_BINARY_DIR}/built/soundtest/soundtest.bas.a78"
    -P ${CMAKE_CURRENT_LIST_FILE}
  LABELS runner audio portable TIMEOUT 30)
set_tests_properties(runner.audio.contract PROPERTIES FIXTURES_REQUIRED built_soundtest)

add_executable(muhtari-test-audio-runner ${CMAKE_CURRENT_LIST_DIR}/test_audio.c
  ${PROJECT_SOURCE_DIR}/runner/bench.c ${PROJECT_SOURCE_DIR}/runner/clock.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c ${PROJECT_SOURCE_DIR}/runner/md5.c
  ${PROJECT_SOURCE_DIR}/runner/movie.c ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-audio-runner PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-audio-runner PRIVATE muhtari::muhtari muhtari_host)
target_compile_definitions(muhtari-test-audio-runner PRIVATE _POSIX_C_SOURCE=200809L
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-audio-runner PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-audio-runner)
muhtari_add_test(NAME runner.audio.stop_failure COMMAND muhtari-test-audio-runner
  ${PROJECT_BINARY_DIR}/tests/audio-stop.txt LABELS runner audio portable TIMEOUT 10)

add_executable(muhtari-test-save-runner ${CMAKE_CURRENT_LIST_DIR}/test_save.c
  ${PROJECT_SOURCE_DIR}/runner/bench.c ${PROJECT_SOURCE_DIR}/runner/clock.c
  ${PROJECT_SOURCE_DIR}/runner/crc32.c ${PROJECT_SOURCE_DIR}/runner/md5.c
  ${PROJECT_SOURCE_DIR}/runner/movie.c ${PROJECT_SOURCE_DIR}/runner/png.c
  ${PROJECT_SOURCE_DIR}/runner/sha256.c)
target_include_directories(muhtari-test-save-runner PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-save-runner PRIVATE muhtari::muhtari muhtari_host)
target_compile_definitions(muhtari-test-save-runner PRIVATE _POSIX_C_SOURCE=200809L
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-save-runner PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-save-runner)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-roundtrip)
muhtari_add_test(NAME runner.save.roundtrip COMMAND ${CMAKE_COMMAND}
  "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}" "-DSAVE_TEST=$<TARGET_FILE:muhtari-test-save-runner>"
  "-DRUN=$<TARGET_FILE:muhtari-run>" "-DDIR=${PROJECT_BINARY_DIR}/tests/save-roundtrip"
  -DCASE=save_roundtrip -P ${CMAKE_CURRENT_LIST_FILE} LABELS runner portable TIMEOUT 10)
foreach(case IN ITEMS import faults options cadence)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-${case})
  muhtari_add_test(NAME runner.save.${case} COMMAND muhtari-test-save-runner
    ${PROJECT_BINARY_DIR}/tests/save-${case} ${case} LABELS runner portable TIMEOUT 10)
endforeach()
if(NOT CMAKE_SYSTEM_NAME STREQUAL "WASI")
  add_executable(muhtari-test-control ${CMAKE_CURRENT_LIST_DIR}/test_control.c)
  target_compile_definitions(muhtari-test-control PRIVATE _POSIX_C_SOURCE=200809L
    MUHTARI_TIMEOUT_SCALE=${MUHTARI_TIMEOUT_SCALE}
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  set_target_properties(muhtari-test-control PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-control)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-controls)
  # Fault prevented: an in-process callback passes while actual OS delivery cannot save/stop.
  muhtari_add_test(NAME runner.save.controls COMMAND muhtari-test-control
    ${PROJECT_BINARY_DIR}/tests/save-controls ${PROJECT_SOURCE_DIR}/tests/diag/battery.a78
    ${CMAKE_CROSSCOMPILING_EMULATOR}
    $<TARGET_FILE:muhtari-run> LABELS runner TIMEOUT 30)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-nonregular)
  muhtari_add_test(NAME runner.save.nonregular COMMAND muhtari-test-save-runner
    ${PROJECT_BINARY_DIR}/tests/save-nonregular nonregular LABELS runner TIMEOUT 10)
endif()
