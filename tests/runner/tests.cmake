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
  if(CASE STREQUAL "hashes")
    execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=write "-DRUN=${RUN}"
      "-DDRIVER=${DRIVER}" "-DDIAG=${DIAG}" "-DOUT=${OUT}"
      "-DEMULATOR=${EMULATOR}" "-DSOUNDTEST=${SOUNDTEST}" "-DPOKEY=${POKEY}"
      -P "${HASHES}" COMMAND_ERROR_IS_FATAL ANY)
    file(STRINGS "${OUT}/hashes.txt" rows)
    list(LENGTH rows count)
    if(NOT count EQUAL 69)
      message(FATAL_ERROR "hashes: expected 69 complete observation rows, got ${count}")
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
    foreach(fault missing_old duplicate_old missing_new duplicate_new)
      set(prefix frame1)
      if(fault MATCHES "_new$")
        set(prefix "battery[.]second[.]save_sha256")
      endif()
      string(REGEX MATCH "${prefix} [^\n]+\n" row "${complete}")
      if(fault MATCHES "^missing_")
        string(REPLACE "${row}" "" bad "${complete}")
      else()
        set(bad "${complete}${row}")
      endif()
      file(WRITE "${OUT}/hashes-battery/${fault}-a.txt" "${bad}")
      file(WRITE "${OUT}/hashes-battery/${fault}-b.txt" "${bad}")
      execute_process(COMMAND "${CMAKE_COMMAND}" -DMODE=compare -DCOUNT=2
        "-DFILES=${OUT}/hashes-battery/${fault}-a.txt;${OUT}/hashes-battery/${fault}-b.txt"
        -P "${HASHES}" RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
      if(NOT rc STREQUAL "1" OR NOT err MATCHES "hashes: missing duplicate or reordered")
        message(FATAL_ERROR "hashes: accepted ${fault} equality: ${rc} ${out}${err}")
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
    foreach(fault valid missing duplicate revision mailbox)
      set(bad "${report}")
      if(fault STREQUAL "missing")
        string(REGEX REPLACE "frame 120 [^\n]+\n" "" bad "${bad}")
      elseif(fault STREQUAL "duplicate")
        string(APPEND bad "${report}")
      elseif(fault STREQUAL "revision")
        string(REPLACE "audio_behaviour_revision 4" "audio_behaviour_revision 3" bad "${bad}")
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
  if(CASE STREQUAL "hash")
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
  elseif(CASE STREQUAL "load.pal_refused")
    # Fault prevented: a PAL cartridge runs on the NTSC timing, so its
    # pinned hashes would change silently when PAL lands.
    expect_exit(77 "linear-v3-16k-pal: unsupported region" --frames 1 "${DIR}/linear-v3-16k-pal")
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
  elseif(CASE STREQUAL "movie.lag")
    # Fault prevented: the runner miscounts or drops the frames in which the
    # game read no input. The input cart's even variant reads on even
    # frames only, so 60 frames hold 30 lag frames.
    write_movie(lag.txt "${prolines}" "60:${released}")
    expect_output("lag_frames 30\n" --frames 60 --movie "${movie}" "${DIR}/input-even.a78")
  elseif(CASE STREQUAL "movie.replay")
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
      --frames 1 --port1 paddle)
    expect_exit(2 "--port2 needs auto, proline, joystick, none, savekey or atarivox.*${usage_line}" --frames 1 --port2)
    expect_output("" --frames 1 --port1 joystick --port2 none)
  elseif(CASE STREQUAL "usage.region_pal")
    expect_exit(2 "--region pal is not supported: give auto or ntsc.*${usage_line}"
      --frames 1 --region pal)
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
foreach(case IN ITEMS hash hash.repeat usage.no_args usage.unknown usage.positional
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
foreach(case IN ITEMS lag replay ports switches past_end malformed rom_mismatch conflict
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
foreach(case IN ITEMS usage.port usage.region_pal usage.boot_bios_no_file)
  muhtari_add_test(NAME runner.${case}
    COMMAND ${runner_check} -DCASE=${case} -P ${CMAKE_CURRENT_LIST_FILE}
    LABELS runner portable TIMEOUT 1) # 2 x 0.16 s, macOS x64, slowest of 17 runs, run 39377950148
endforeach()

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
foreach(case IN ITEMS hash pal_refused)
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
  ${PROJECT_SOURCE_DIR}/runner/md5.c)
target_compile_definitions(muhtari-test-runner PRIVATE _POSIX_C_SOURCE=200809L)
set_target_properties(muhtari-test-runner PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-runner)
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
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  set_target_properties(muhtari-test-control PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  muhtari_warnings(muhtari-test-control)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-controls)
  # Fault prevented: an in-process callback passes while actual OS delivery cannot save/stop.
  muhtari_add_test(NAME runner.save.controls COMMAND muhtari-test-control
    ${PROJECT_BINARY_DIR}/tests/save-controls ${CMAKE_CROSSCOMPILING_EMULATOR}
    $<TARGET_FILE:muhtari-run> LABELS runner TIMEOUT 30)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests/save-nonregular)
  muhtari_add_test(NAME runner.save.nonregular COMMAND muhtari-test-save-runner
    ${PROJECT_BINARY_DIR}/tests/save-nonregular nonregular LABELS runner TIMEOUT 10)
endif()
