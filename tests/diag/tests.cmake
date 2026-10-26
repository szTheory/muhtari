# One admitted template, independent numeric judgment, then real host equality.
if(DIAG_BANKSET_RUN)
  file(MAKE_DIRECTORY "${OUT}")
  function(bankset_exec)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" ${ARGV}
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "bankset ${ARGV}: ${rc}: ${out}${err}")
    endif()
  endfunction()
  set(cases 32n)
  if(BANKSET_EXPANDED)
    set(cases 32n 32s 32h 48n 52n 128n 128s 128h 256n 256s 256h 512n 512s 512h 32h-pal 512s-pal)
  endif()
  set(hashes "")
  foreach(frame RANGE 1 12)
    list(APPEND hashes --hash-frame ${frame})
  endforeach()
  foreach(case IN LISTS cases)
    string(REGEX MATCH "^([0-9]+)([nsh])" parsed "${case}")
    set(kib "${CMAKE_MATCH_1}")
    set(mode 0)
    if(CMAKE_MATCH_2 STREQUAL s)
      set(mode 1)
    elseif(CMAKE_MATCH_2 STREQUAL h)
      set(mode 2)
    endif()
    set(tag "bankset${parsed}")
    set(pal 0)
    if(case MATCHES "-pal$")
      set(pal 1)
    endif()
    set(dir "${OUT}/${case}")
    file(MAKE_DIRECTORY "${dir}")
    set(image "${dir}/bankset${case}.a78")
    bankset_exec(--bankset-image "${ROOT}/tests/diag/bankset.a78" "${image}" ${kib} ${mode} ${pal} 0)
    if(case STREQUAL "32n")
      execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files
        "${ROOT}/tests/diag/bankset.a78" "${image}" COMMAND_ERROR_IS_FATAL ANY)
    endif()
    bankset_exec(--bankset-case "${image}" "${dir}" "${EXPECTED}" "${tag}" "${MODULE}")
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --movie "${dir}/input.txt"
      --frames 12 ${hashes} --hash-audio --dump-frame "5:${dir}/pixels.png" --diagnostic "${image}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE actual ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "bankset actual runner ${case}: ${rc} ${err}")
    endif()
    string(REGEX MATCH "diagnostic [0-9a-f]+\n" mailbox "${actual}")
    file(CONFIGURE OUTPUT "${dir}/report.txt" CONTENT "${mailbox}" @ONLY NEWLINE_STYLE UNIX)
    bankset_exec(--mailbox "${dir}/report.txt" "${tag}" "${EXPECTED}")
    bankset_exec(--bankset-png "${dir}/pixels.png" ${mode} ${pal} "${dir}/filter-fault.png")
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --bankset-png
      "${dir}/filter-fault.png" ${mode} ${pal}
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 10)
    if(NOT rc STREQUAL "1" OR NOT err MATCHES "PNG row filter is not zero")
      message(FATAL_ERROR "bankset accepted CRC-valid PNG filter fault: ${rc} ${err}")
    endif()
    # All actual hosts have now passed numeric judgment. Compare actual taps.
    file(STRINGS "${dir}/reference.txt" evidence)
    foreach(line IN LISTS evidence)
      string(FIND "${actual}" "${line}\n" found)
      if(found EQUAL -1)
        message(FATAL_ERROR "bankset runner native/raw/PCM/count mismatch ${case}: ${line}")
      endif()
    endforeach()
    foreach(fault RANGE 0 11)
      execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${dir}/fault-${fault}.txt"
        "${tag}" "${EXPECTED}" RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT 10)
      if(NOT rc STREQUAL "1")
        message(FATAL_ERROR "bankset accepted numeric report fault ${case}/${fault}: ${rc}")
      endif()
    endforeach()
    foreach(fault missing truncated extra)
      set(path "${dir}/${fault}.txt")
      if(fault STREQUAL truncated)
        string(SUBSTRING "${mailbox}" 0 50 bad)
        file(WRITE "${path}" "${bad}")
      elseif(fault STREQUAL extra)
        file(WRITE "${path}" "${mailbox}extra")
      endif()
      execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${path}"
        "${tag}" "${EXPECTED}" RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT 10)
      if(NOT rc STREQUAL "1")
        message(FATAL_ERROR "bankset accepted ${fault} report")
      endif()
    endforeach()
    message("bankset ${case}: seventeen records, native pixels, real hosts and all output equality pass")
  endforeach()
  # Equal/wrong ROM-set graphics must fail the actual native judge.
  bankset_exec(--bankset-image "${ROOT}/tests/diag/bankset.a78" "${OUT}/wrong.a78" 32 0 0 1)
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --bankset-case "${OUT}/wrong.a78" "${OUT}"
    "${EXPECTED}" bankset32n "" RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "1" OR NOT err MATCHES "wrong native graphics sentinel")
    message(FATAL_ERROR "bankset accepted CPU-set graphics: ${rc} ${err}")
  endif()
  execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 5
    --dump-frame "5:${OUT}/wrong.png" "${OUT}/wrong.a78"
    RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "bankset wrong-view runner control did not execute: ${rc} ${err}")
  endif()
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --bankset-png "${OUT}/wrong.png" 0 0
    RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT 10)
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "bankset accepted actual runner wrong-view pixels")
  endif()
  if(BANKSET_EXPANDED)
    foreach(kib 48 52)
      foreach(mode 1 2)
        bankset_exec(--bankset-image "${ROOT}/tests/diag/bankset.a78" "${OUT}/refused.a78" ${kib} ${mode} 0 0)
        bankset_exec(--bankset-refuse "${OUT}/refused.a78" invalid "${MODULE}")
        execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 1 "${OUT}/refused.a78"
          RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 10)
        if(NOT rc STREQUAL "1" OR NOT err MATCHES "not a 7800 or 2600 cartridge image")
          message(FATAL_ERROR "bankset runner accepted forbidden ${kib}/${mode}: ${rc}")
        endif()
      endforeach()
    endforeach()
    foreach(fault 2 3)
      bankset_exec(--bankset-image "${ROOT}/tests/diag/bankset.a78" "${OUT}/refused.a78" 32 0 0 ${fault})
      bankset_exec(--bankset-refuse "${OUT}/refused.a78" truncated "${MODULE}")
      execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 1 "${OUT}/refused.a78"
        RESULT_VARIABLE rc OUTPUT_QUIET ERROR_VARIABLE err TIMEOUT 10)
      if(NOT rc STREQUAL "1" OR NOT err MATCHES "(not a 7800 or 2600 cartridge image|the image ends before the size its header gives)")
        message(FATAL_ERROR "bankset runner accepted malformed geometry ${fault}: ${rc} ${err}")
      endif()
    endforeach()
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --bankset-image "${OUT}/wrong.a78"
      "${OUT}/forged.a78" 32 0 0 0 RESULT_VARIABLE rc TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "bankset accepted unrelated or altered template identity")
    endif()
  endif()
  return()
endif()

# Sustained acceptance is never shortened by MUHTARI_SHORT_RUNS. Numeric
# startup/full-frame judgment runs before any native/raw/PCM equality row.
if(DIAG_PAL_RUN)
  file(MAKE_DIRECTORY "${OUT}")
  set(pal_modules "")
  if(MODULE)
    set(pal_modules "${MODULE};${MODULE565}")
  else()
    set(pal_modules "library")
  endif()
  foreach(module IN LISTS pal_modules)
    set(selected "")
    if(NOT module STREQUAL library)
      set(selected "${module}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --pal-case
      "${ROOT}/tests/diag/pal.a78" "${EXPECTED}" "${OUT}" ${selected}
      RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT ${PAL_TIMEOUT})
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "pal numeric/native/raw/PCM host: ${rc} ${report}${err}")
    endif()
  endforeach()
  set(pal_hashes "")
  foreach(frame RANGE 1 601)
    list(APPEND pal_hashes --hash-frame ${frame})
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 601 ${pal_hashes}
    --hash-audio --diagnostic "${ROOT}/tests/diag/pal.a78"
    RESULT_VARIABLE rc OUTPUT_VARIABLE actual ERROR_VARIABLE err TIMEOUT ${PAL_TIMEOUT})
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "pal actual runner: ${rc} ${err}")
  endif()
  file(CONFIGURE OUTPUT "${OUT}/runner.txt" CONTENT "${actual}" @ONLY NEWLINE_STYLE UNIX)
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --pal-report "${OUT}/runner.txt" "${EXPECTED}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT ${PAL_TIMEOUT})
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "pal actual runner numeric judgment: ${rc} ${report}${err}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files "${OUT}/runner.txt" "${OUT}/reference.txt"
    RESULT_VARIABLE rc)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "pal actual runner601 native frames/raw/PCM differs from judged library")
  endif()
  foreach(fault RANGE 0 10)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/fault-${fault}.txt" pal "${EXPECTED}"
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT ${PAL_TIMEOUT})
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "pal accepted numeric/protocol fault ${fault}: ${rc}")
    endif()
  endforeach()
  # A real shortened session has only599 full hardware intervals. It must
  # fail even if the same emulator produces internally consistent hashes.
  set(short_hashes "")
  foreach(frame RANGE 1 600)
    list(APPEND short_hashes --hash-frame ${frame})
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 600 ${short_hashes}
    --hash-audio --diagnostic "${ROOT}/tests/diag/pal.a78"
    RESULT_VARIABLE rc OUTPUT_VARIABLE short_report ERROR_VARIABLE err TIMEOUT ${PAL_TIMEOUT})
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "pal shortened-session control failed to execute: ${rc} ${err}")
  endif()
  file(CONFIGURE OUTPUT "${OUT}/short-session.txt" CONTENT "${short_report}" @ONLY NEWLINE_STYLE UNIX)
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --pal-report "${OUT}/short-session.txt" "${EXPECTED}"
    RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT ${PAL_TIMEOUT})
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "pal accepted actual599-full-frame session: ${rc}")
  endif()
  foreach(fault IN ITEMS samples calls shortened truncated trailing missing)
    set(bad "${actual}")
    if(fault STREQUAL samples)
      string(REPLACE "audio_samples 577879" "audio_samples 577878" bad "${bad}")
    elseif(fault STREQUAL calls)
      string(REPLACE "audio_frames 601" "audio_frames 600" bad "${bad}")
    elseif(fault STREQUAL shortened)
      string(REGEX REPLACE "frame 600 sha256 [0-9a-f]+\n" "" bad "${bad}")
    elseif(fault STREQUAL truncated)
      string(REGEX REPLACE "\n$" "" bad "${bad}")
    elseif(fault STREQUAL trailing)
      string(APPEND bad "extra\n")
    else()
      set(bad "")
    endif()
    if(fault STREQUAL truncated OR fault STREQUAL missing)
      # CONFIGURE appends a final newline, which would repair this fault.
      file(WRITE "${OUT}/report-${fault}.txt" "${bad}")
    else()
      file(CONFIGURE OUTPUT "${OUT}/report-${fault}.txt" CONTENT "${bad}" @ONLY NEWLINE_STYLE UNIX)
    endif()
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --pal-report "${OUT}/report-${fault}.txt" "${EXPECTED}"
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT ${PAL_TIMEOUT})
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "pal accepted incomplete numeric report ${fault}: ${rc}")
    endif()
  endforeach()
  set(pal_writer_template [=[
set(HASHES_FUNCTIONS_ONLY TRUE)
include([==[@ROOT@/tests/cmake/hashes.cmake]==])
set(DRIVER [==[@DRIVER@]==])
set(DIAG [==[@ROOT@/tests/diag]==])
set(EMULATOR [==[@EMULATOR@]==])
hashes_pal_report([==[@pal_report_path@]==] rows)
]=])
  foreach(fault IN ITEMS valid samples calls shortened truncated trailing missing short-session)
    set(pal_report_path "${OUT}/report-${fault}.txt")
    if(fault STREQUAL valid)
      set(pal_report_path "${OUT}/runner.txt")
    elseif(fault STREQUAL short-session)
      set(pal_report_path "${OUT}/short-session.txt")
    endif()
    string(CONFIGURE "${pal_writer_template}" writer @ONLY)
    file(CONFIGURE OUTPUT "${OUT}/writer-${fault}.cmake" CONTENT "${writer}" @ONLY NEWLINE_STYLE UNIX)
    execute_process(COMMAND ${CMAKE_COMMAND} -P "${OUT}/writer-${fault}.cmake"
      RESULT_VARIABLE rc OUTPUT_VARIABLE verdict ERROR_VARIABLE err TIMEOUT ${PAL_TIMEOUT})
    if(fault STREQUAL valid)
      if(NOT rc STREQUAL "0")
        message(FATAL_ERROR "PAL equality writer rejected valid numbers: ${rc} ${verdict}${err}")
      endif()
    elseif(NOT rc STREQUAL "1" OR NOT err MATCHES "hashes: PAL numeric report failed")
      message(FATAL_ERROR "PAL equality writer accepted ${fault}: ${rc} ${verdict}${err}")
    endif()
  endforeach()
  if(DEFINED HASHES_PAL_ROWS)
    hashes_pal_report("${OUT}/runner.txt" rows)
    set(HASHES_PAL_ROWS "${rows}")
  endif()
  message("PAL: separately judged startup plus600 full frames; actual library/runner/modules native/raw/PCM equality and report fault controls pass")
  return()
endif()

# Concrete fault: agreeing hosts masking wrong controller numbers or omitted native/audio evidence.
if(DIAG_CONTROLLERS_RUN)
  set(root_out "${OUT}")
  foreach(device IN ITEMS driving lightgun paddle keypad trakball st-mouse amiga-mouse snes2atari mega7800)
  set(last_group 0)
  set(adapter FALSE)
  if(device STREQUAL "snes2atari" OR device STREQUAL "mega7800")
    set(last_group 2)
    set(adapter TRUE)
  endif()
  foreach(group RANGE 0 ${last_group})
  set(OUT "${root_out}/${device}-${group}")
  set(port_arg both)
  if(adapter)
    set(port_arg ${group})
    set(fixture controller-motion)
    set(selected_module "${MODULE}")
    string(SUBSTRING "abc" ${group} 1 suffix)
    if(device STREQUAL "snes2atari")
      set(scenario motion4${suffix})
    else()
      set(scenario motion5${suffix})
    endif()
  elseif(device STREQUAL "trakball" OR device STREQUAL "st-mouse" OR device STREQUAL "amiga-mouse")
    set(fixture controller-motion)
    if(device STREQUAL "trakball")
      set(scenario motion1)
    elseif(device STREQUAL "st-mouse")
      set(scenario motion2)
    else()
      set(scenario motion3)
    endif()
    set(selected_module "${MODULE}")
  elseif(device STREQUAL "keypad")
    set(fixture controller-analog)
    set(scenario analog1)
    set(selected_module "${MODULE}")
  elseif(device STREQUAL "paddle")
    set(fixture controller-analog)
    set(scenario analog0)
    set(selected_module "${MODULE}")
  elseif(device STREQUAL "lightgun")
    set(fixture controller-light)
    set(scenario light0)
    set(selected_module "${MODULE}")
  else()
    set(fixture controller-motion)
    set(scenario motion0)
    set(selected_module "${MODULE}")
  endif()
  file(MAKE_DIRECTORY "${OUT}")
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --controller-case ${device} ${port_arg}
    "${ROOT}/tests/diag/${fixture}.a78" "${EXPECTED}" "${OUT}" ${selected_module}
    RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "controller numeric/equality: ${rc}\n${report}${err}")
  endif()
  set(hashes "")
  foreach(frame RANGE 1 12)
    list(APPEND hashes --hash-frame ${frame})
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${RUNNER}" --movie "${OUT}/input.txt"
    --frames 12 ${hashes} --hash-audio --diagnostic "${ROOT}/tests/diag/${fixture}.a78"
    RESULT_VARIABLE rc OUTPUT_VARIABLE actual ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "controller actual runner: ${rc}\n${actual}${err}")
  endif()
  string(REGEX MATCH "diagnostic [0-9a-f]+\n" mailbox "${actual}")
  file(CONFIGURE OUTPUT "${OUT}/report.txt" CONTENT "${mailbox}" @ONLY NEWLINE_STYLE UNIX)
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/report.txt" ${scenario}
    "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "controller actual runner numeric judgment: ${rc}")
  endif()
  file(STRINGS "${OUT}/reference.txt" evidence)
  foreach(line IN LISTS evidence)
    string(FIND "${actual}" "${line}\n" found)
    if(found EQUAL -1)
      message(FATAL_ERROR "controller actual runner differs: ${line}")
    endif()
  endforeach()
  if(DEFINED HASHES_CONTROLLER_ROWS)
    string(REGEX REPLACE "^controller-" "" namespace "${fixture}")
    set(prefix "controller.${namespace}.${device}")
    if(adapter)
      string(APPEND prefix ".group${group}")
    endif()
    string(APPEND prefix ".both")
    file(CONFIGURE OUTPUT "${OUT}/hashes-report.txt" CONTENT "${actual}" @ONLY NEWLINE_STYLE UNIX)
    hashes_controller_report("${prefix}" "${scenario}" "${actual}" rows)
    string(APPEND HASHES_CONTROLLER_ROWS "${rows}")
  endif()
  set(last_fault 4)
  if(adapter)
    set(last_fault 7)
  elseif(device STREQUAL "keypad")
    set(last_fault 6)
  elseif(device STREQUAL "trakball" OR device STREQUAL "st-mouse" OR device STREQUAL "amiga-mouse")
    set(last_fault 8)
  endif()
  foreach(fault RANGE 0 ${last_fault})
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/fault-${fault}.txt"
      ${scenario} "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "controller report accepted numeric/protocol fault ${fault}: ${rc}")
    endif()
  endforeach()
  string(LENGTH "${mailbox}" length)
  math(EXPR length "${length}-1")
  string(SUBSTRING "${mailbox}" 0 ${length} truncated)
  file(WRITE "${OUT}/truncated.txt" "${truncated}")
  file(REMOVE "${OUT}/missing.txt")
  foreach(fault IN ITEMS truncated missing)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/${fault}.txt"
      ${scenario} "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "controller report accepted ${fault}: ${rc}")
    endif()
  endforeach()
  if(selected_module)
    message("${device}: independent library/runner/module numbers, native/public pixels, raw transitions and exact PCM/counts pass")
  else()
    message("${device}: independent library/runner numbers and native/raw/PCM/counts pass; module unavailable on this configuration")
  endif()
  endforeach()
  endforeach()
  return()
endif()

# Detailed cross-host battery observations reuse the numeric/media owner.
# Fault prevented: agreeing hosts hiding unchanged single-media bytes or a bad mailbox.
if(DIAG_BATTERY_RUN)
  file(REMOVE_RECURSE "${OUT}")
  file(MAKE_DIRECTORY "${OUT}/runner")
  function(battery_exec)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" ${ARGV}
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag battery ${ARGV}: ${rc}\n${out}${err}")
    endif()
    message("${out}")
  endfunction()
  foreach(count 1 2)
    set(import "")
    if(count EQUAL 2)
      set(import --battery-import "${OUT}/library-1.sav")
    endif()
    battery_exec(--battery-count ${count} ${import} --battery-export "${OUT}/library-${count}.sav"
      "${ROOT}/tests/diag/battery.a78" "${EXPECTED}")
    if(count EQUAL 2)
      set(import --battery-import "${OUT}/module-1.srm")
    endif()
    battery_exec(--battery-count ${count} ${import} --battery-export "${OUT}/module-${count}.srm"
      --libretro "${MODULE}" "${ROOT}/tests/diag/battery.a78" "${EXPECTED}")
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 120 --diagnostic --save-dir "${OUT}/runner"
      "${ROOT}/tests/diag/battery.a78" RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag battery fresh runner ${count}: ${rc} ${err}")
    endif()
    file(COPY_FILE "${OUT}/runner/battery.sav" "${OUT}/runner-${count}.sav")
    file(CONFIGURE OUTPUT "${OUT}/report-${count}.txt" CONTENT "${report}" @ONLY NEWLINE_STYLE UNIX)
    battery_exec(--battery-count ${count} --mailbox "${OUT}/report-${count}.txt" battery "${EXPECTED}")
    foreach(host library module runner)
      set(ext sav)
      if(host STREQUAL "module")
        set(ext srm)
      endif()
      battery_exec(--battery-media "${OUT}/${host}-${count}.${ext}" ${count})
      execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files
        "${OUT}/${host}-${count}.${ext}" "${OUT}/runner-${count}.sav" COMMAND_ERROR_IS_FATAL ANY)
    endforeach()
  endforeach()
  battery_exec(--battery-controls "${OUT}/runner-2.sav" 2)
  battery_exec(--battery-bmp "${OUT}/pixels.bmp" "${ROOT}/tests/diag/battery.a78" "${EXPECTED}")
  execute_process(COMMAND ${EMULATOR} "${COMPARE}" --battery-controls 1 255
    "${OUT}/pixels.bmp" "${OUT}/pixels.bmp" COMMAND_ERROR_IS_FATAL ANY)
  foreach(fault missing short oversized crc unchanged)
    set(path "${OUT}/bad-report.txt")
    if(fault STREQUAL "missing")
      set(path "${OUT}/missing.txt")
    else()
      set(bad "${report}")
      if(fault STREQUAL "short")
        string(SUBSTRING "${bad}" 0 50 bad)
      elseif(fault STREQUAL "oversized")
        string(APPEND bad extra)
      elseif(fault STREQUAL "crc")
        string(SUBSTRING "${bad}" 0 27 prefix)
        string(SUBSTRING "${bad}" 28 -1 suffix)
        string(SUBSTRING "${bad}" 27 1 digit)
        if(digit STREQUAL "0")
          set(digit 1)
        else()
          set(digit 0)
        endif()
        set(bad "${prefix}${digit}${suffix}")
      else()
        file(READ "${OUT}/report-1.txt" bad)
      endif()
      file(WRITE "${path}" "${bad}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --battery-count 2 --mailbox "${path}" battery "${EXPECTED}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "diag battery accepted ${fault} report: ${rc} ${out}${err}")
    endif()
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --battery-media "${OUT}/missing.sav" 2
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 10)
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "diag battery accepted missing media: ${rc}")
  endif()
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --battery-timeout
    "${ROOT}/tests/diag/battery.a78" "${EXPECTED}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "1" OR NOT out MATCHES "hang: no status .00 within 600 frames")
    message(FATAL_ERROR "diag battery timeout control: ${rc} ${out}${err}")
  endif()
  return()
endif()

# The generated retail batch reuses this file as a CMake script. Faults:
# absent/corrupt runner reports, incomplete mailbox protocols, and a missing
# admitted fixture must never produce a passing three-deliverable observation.
if(DIAG_RETAIL_RUN)
  function(diag_exec)
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" ${ARGV}
      RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag retail ${ARGV}: ${rc}\n${out}\n${err}")
    endif()
    message("${out}")
  endfunction()
  function(diag_observe path name smoke)
    if(smoke)
      diag_exec(--smoke "${path}")
    else()
      diag_exec("${path}" "${EXPECTED}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --probe "${path}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE probe TIMEOUT 10)
    if(name STREQUAL "smoke")
      set(board absolute)
    else()
      set(board "${name}")
    endif()
    if(NOT rc STREQUAL "0" OR NOT probe MATCHES "\nboard ${board}\n")
      message(FATAL_ERROR "diag retail wrong board: ${rc} ${probe}")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" --frames 600 --diagnostic "${path}"
      RESULT_VARIABLE rc OUTPUT_VARIABLE report ERROR_VARIABLE err TIMEOUT 30)
    if(NOT rc STREQUAL "0")
      message(FATAL_ERROR "diag retail runner: ${rc} ${err}")
    endif()
    file(CONFIGURE OUTPUT "${OUT}/report.txt" CONTENT "${report}" @ONLY NEWLINE_STYLE UNIX)
    diag_exec(--mailbox "${OUT}/report.txt" "${name}" "${EXPECTED}")
    diag_exec(--libretro "${MODULE}" "${path}" "${EXPECTED}")
    message("${name}: library, runner and libretro numeric observations passed")
  endfunction()
  file(MAKE_DIRECTORY "${OUT}")
  diag_observe("${OUT}/smoke.a78" smoke TRUE)
  file(READ "${OUT}/report.txt" good_report)
  foreach(bad IN ITEMS "" "${good_report}extra" "diagnostic xyz\n")
    file(WRITE "${OUT}/bad-report.txt" "${bad}")
    execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/bad-report.txt" smoke
      "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
    if(NOT rc STREQUAL "1")
      message(FATAL_ERROR "diag retail accepted malformed/absent report: ${rc}")
    endif()
  endforeach()
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${OUT}/absent-report" smoke
    "${EXPECTED}" RESULT_VARIABLE rc TIMEOUT 10)
  if(NOT rc STREQUAL "1")
    message(FATAL_ERROR "diag retail accepted missing report")
  endif()
  foreach(args IN ITEMS "--diagnostic" "--frames;1;--diagnostic"
      "--bench;1;--diagnostic;${OUT}/smoke.a78"
      "--frames;1;--diagnostic;--diagnostic;${OUT}/smoke.a78")
    execute_process(COMMAND ${EMULATOR} "${RUNNER}" ${args}
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET TIMEOUT 10)
    if(NOT rc STREQUAL "2")
      message(FATAL_ERROR "diag retail usage fault ${args}: ${rc}")
    endif()
  endforeach()
  file(STRINGS "${MANIFEST}" admitted REGEX "^tests/diag/(activision|absolute|fractalus)[.]a78\t")
  foreach(row IN LISTS admitted)
    string(REPLACE "\t" ";" fields "${row}")
    list(GET fields 0 relative)
    list(GET fields 1 source)
    list(GET fields 3 license)
    if(NOT source STREQUAL "this-repo" OR NOT license STREQUAL "MIT" OR
        NOT EXISTS "${ROOT}/${relative}")
      message(FATAL_ERROR "diag retail missing or unowned admitted fixture: ${relative}")
    endif()
    get_filename_component(name "${relative}" NAME_WE)
    diag_observe("${ROOT}/${relative}" "${name}" FALSE)
  endforeach()
  return()
endif()

# Our own diagnostic ROMs (docs/emulation-tests.md, diag). Each tests/diag/*.s
# is MIT code of this repository, assembled into the committed .a78 beside it
# by assemble.cmake. muhtari-test-diag runs one and judges its mailbox against
# expected.csv, whose values docs/hardware/maria.md derives before a ROM
# first runs.
set(diag_dir ${CMAKE_CURRENT_LIST_DIR})
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${diag_dir}/expected.csv
  ${diag_dir}/stats.csv)

# muhtari-test-diag: links muhtari and includes src/internal.h, to read the
# mailbox through muhtari__peek and the core's per-line statistics, which
# stats.csv gives for each DMA test, so each cost is recorded twice.
add_executable(muhtari-test-diag ${diag_dir}/driver.c ${PROJECT_SOURCE_DIR}/runner/sha256.c
  ${PROJECT_SOURCE_DIR}/tests/runner/png_read.c)
target_include_directories(muhtari-test-diag PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(muhtari-test-diag PRIVATE muhtari::muhtari muhtari_host)
add_executable(muhtari-test-retroarch-compare ${PROJECT_SOURCE_DIR}/tests/retroarch/compare.c
  ${PROJECT_SOURCE_DIR}/tests/retroarch/bmp.c)
target_compile_definitions(muhtari-test-retroarch-compare PRIVATE
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
set_target_properties(muhtari-test-retroarch-compare PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-retroarch-compare)
if(TARGET muhtari_libretro)
  target_include_directories(muhtari-test-diag PRIVATE ${PROJECT_SOURCE_DIR}/libretro)
  target_compile_definitions(muhtari-test-diag PRIVATE DIAG_LIBRETRO
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  target_link_libraries(muhtari-test-diag PRIVATE ${CMAKE_DL_LIBS})
  set(diag_retail ${PROJECT_BINARY_DIR}/tests/diag/retail.cmake)
  set(diag_battery ${PROJECT_BINARY_DIR}/tests/diag/battery.cmake)
  file(GENERATE OUTPUT ${diag_battery} CONTENT
"set(DIAG_BATTERY_RUN TRUE)
set(COMPARE [==[$<TARGET_FILE:muhtari-test-retroarch-compare>]==])
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/battery-roundtrip]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[$<TARGET_FILE:muhtari_libretro>]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
  muhtari_add_test(NAME diag.battery COMMAND ${CMAKE_COMMAND} -P ${diag_battery}
    LABELS diag portable TIMEOUT 30)
  file(GENERATE OUTPUT ${diag_retail} CONTENT
"set(DIAG_RETAIL_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/retail]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[$<TARGET_FILE:muhtari_libretro>]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
set(MANIFEST [==[${PROJECT_SOURCE_DIR}/tests/roms/manifest.txt]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
  muhtari_add_test(NAME diag.retail COMMAND ${CMAKE_COMMAND} -P ${diag_retail}
    LABELS diag portable TIMEOUT 120)
endif()
set_target_properties(muhtari-test-diag PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-diag)

set(diag_controllers ${PROJECT_BINARY_DIR}/tests/diag/controllers.cmake)
set(diag_controller_module "")
if(TARGET muhtari_libretro)
  set(diag_controller_module "$<TARGET_FILE:muhtari_libretro_testdb>")
endif()
file(GENERATE OUTPUT ${diag_controllers} CONTENT
"set(DIAG_CONTROLLERS_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/controllers]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[${diag_controller_module}]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
muhtari_add_test(NAME diag.controllers COMMAND ${CMAKE_COMMAND} -P ${diag_controllers}
  LABELS diag portable TIMEOUT 30)

set(diag_bankset ${PROJECT_BINARY_DIR}/tests/diag/bankset.cmake)
file(GENERATE OUTPUT ${diag_bankset} CONTENT
"set(DIAG_BANKSET_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/bankset]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[${diag_controller_module}]==])
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
muhtari_add_test(NAME diag.bankset_tracer COMMAND ${CMAKE_COMMAND} -P ${diag_bankset}
  LABELS diag portable TIMEOUT 30)
set(diag_bankset_matrix ${PROJECT_BINARY_DIR}/tests/diag/bankset-matrix.cmake)
file(GENERATE OUTPUT ${diag_bankset_matrix} CONTENT
"set(BANKSET_EXPANDED TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
include([==[${diag_bankset}]==])
")
muhtari_add_test(NAME diag.bankset COMMAND ${CMAKE_COMMAND} -P ${diag_bankset_matrix}
  LABELS diag portable TIMEOUT 4) # Provisional ceil(2 x 1.85s), complete local matrix; N/A-local.

set(diag_pal ${PROJECT_BINARY_DIR}/tests/diag/pal.cmake)
set(diag_pal565 "")
if(TARGET muhtari_libretro)
  set(diag_pal565 "$<TARGET_FILE:muhtari_libretro_testdb565>")
endif()
math(EXPR diag_pal_timeout "53 * ${MUHTARI_TIMEOUT_SCALE}")
file(GENERATE OUTPUT ${diag_pal} CONTENT
"set(DIAG_PAL_RUN TRUE)
set(ROOT [==[${PROJECT_SOURCE_DIR}]==])
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/pal]==])
set(DRIVER [==[$<TARGET_FILE:muhtari-test-diag>]==])
set(RUNNER [==[$<TARGET_FILE:muhtari-run>]==])
set(MODULE [==[${diag_controller_module}]==])
set(MODULE565 [==[${diag_pal565}]==])
set(PAL_TIMEOUT ${diag_pal_timeout})
set(EMULATOR [==[${CMAKE_CROSSCOMPILING_EMULATOR}]==])
set(EXPECTED [==[${diag_dir}/expected.csv]==])
include([==[${CMAKE_CURRENT_LIST_FILE}]==])
")
# Provisional: ceil(2 * 52.64s / ASan scale 2), complete workload,
# run 40238824962/job 121307057493; existing lane scales and workload remain.
muhtari_add_test(NAME diag.pal COMMAND ${CMAKE_COMMAND} -P ${diag_pal}
  LABELS diag portable TIMEOUT 53 COST 18)


muhtari_add_test(NAME diag.dma
  COMMAND muhtari-test-diag ${diag_dir}/dma.a78 ${diag_dir}/expected.csv ${diag_dir}/stats.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.10 s, macOS x64, slowest of 27 runs, run 39358378619
muhtari_add_test(NAME diag.cpu
  COMMAND muhtari-test-diag ${diag_dir}/cpu.a78 ${diag_dir}/expected.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.05 s, Windows x64, slowest of 24 runs, run 39358378619
muhtari_add_test(NAME diag.dli
  COMMAND muhtari-test-diag ${diag_dir}/dli.a78 ${diag_dir}/expected.csv ${diag_dir}/stats.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.09 s, macOS x64, slowest of 23 runs, run 39362291672
muhtari_add_test(NAME diag.modes
  COMMAND muhtari-test-diag ${diag_dir}/modes.a78 ${diag_dir}/expected.csv
  LABELS diag portable TIMEOUT 1)  # 2 x 0.04 s, Windows arm64, slowest of 19 runs, run 39382343202
# The board ROMs: the bytes each board maps (docs/hardware/cartridge.md,
# The board ROMs).
foreach(diag_board IN ITEMS linear16 bank6 sg144)
  muhtari_add_test(NAME diag.${diag_board}
    COMMAND muhtari-test-diag ${diag_dir}/${diag_board}.a78 ${diag_dir}/expected.csv
    LABELS diag portable TIMEOUT 1)  # 2 x 0.03 s, Windows arm64, slowest of 21 runs, run 39382343202
endforeach()

# diag.rebuild: every tests/diag/*.s that has a .cfg, and Bruce Clark's
# decimal test as printed and as its 65C02 control, assembled by the pinned
# tools and compared with the manifest (assemble.cmake). One generated script
# runs them in turn; a failure names the file. The tools come from
# tests/built/tests.cmake.
set(diag_out ${PROJECT_BINARY_DIR}/tests/diag)
set(diag_manifest ${CMAKE_SOURCE_DIR}/tests/roms/manifest.txt)
set(diag_decimal ${CMAKE_SOURCE_DIR}/tests/cpu/decimal)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${diag_manifest})
file(GLOB diag_sources CONFIGURE_DEPENDS ${diag_dir}/*.s)
set(diag_rebuild "")
foreach(diag_src IN LISTS diag_sources)
  string(REGEX REPLACE "\\.s$" ".cfg" diag_cfg "${diag_src}")
  if(EXISTS ${diag_cfg})
    string(APPEND diag_rebuild "set(SRC [==[${diag_src}]==])\nset(CFG [==[${diag_cfg}]==])\n"
      "set(PATH \"\")\nset(VARIANT \"\")\ninclude([==[${diag_dir}/assemble.cmake]==])\n")
  endif()
endforeach()
foreach(diag_variant IN ITEMS 6502 65c02)
  set(diag_v "")
  if(diag_variant STREQUAL "65c02")
    set(diag_v 65c02)
  endif()
  string(APPEND diag_rebuild "set(SRC [==[${diag_decimal}/clark.s]==])\nset(CFG \"\")\n"
    "set(PATH tests/cpu/decimal/clark-${diag_variant}.bin)\nset(VARIANT \"${diag_v}\")\n"
    "include([==[${diag_dir}/assemble.cmake]==])\n")
endforeach()
set(diag_tools "set(WASMTIME [==[${MUHTARI_WASMTIME}]==])\nset(PKG [==[${MUHTARI_7800BASIC}]==])\n")
set(diag_rebuild_script ${diag_out}/rebuild.cmake)
file(WRITE ${diag_rebuild_script} "${diag_tools}set(OUT [==[${diag_out}/rebuild]==])\n"
  "set(MANIFEST [==[${diag_manifest}]==])\n${diag_rebuild}")
muhtari_add_test(NAME diag.rebuild COMMAND ${CMAKE_COMMAND} -P ${diag_rebuild_script}
  LABELS built portable TIMEOUT 22)  # 2 x 10.66 s, macOS x64, slowest of 26 runs, run 39362291672

# Negative control: one digit of dma.a78's digest changed in a copy of the
# manifest must fail by name.
file(READ ${diag_manifest} diag_text)
string(REGEX MATCH "tests/diag/dma\\.a78\t[^\t]*\t[^\t]*\t[^\t]*\t([0-9a-f]+)" diag_row "${diag_text}")
set(diag_digest ${CMAKE_MATCH_1})
string(SUBSTRING ${diag_digest} 0 63 diag_head)
string(SUBSTRING ${diag_digest} 63 1 diag_tail)
if(diag_tail STREQUAL "0")
  set(diag_tail 1)
else()
  set(diag_tail 0)
endif()
string(REPLACE "${diag_digest}" "${diag_head}${diag_tail}" diag_text "${diag_text}")
file(WRITE ${diag_out}/manifest_wrong.txt "${diag_text}")
set(diag_wrong_script ${diag_out}/rebuild_wrong.cmake)
file(WRITE ${diag_wrong_script} "${diag_tools}set(OUT [==[${diag_out}/rebuild_wrong]==])\n"
  "set(MANIFEST [==[${diag_out}/manifest_wrong.txt]==])\n"
  "set(SRC [==[${diag_dir}/dma.s]==])\nset(CFG [==[${diag_dir}/dma.cfg]==])\n"
  "set(PATH tests/diag/dma.a78)\ninclude([==[${diag_dir}/assemble.cmake]==])\n")
muhtari_add_test(NAME diag.rebuild.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${CMAKE_COMMAND}$<SEMICOLON>-P$<SEMICOLON>${diag_wrong_script}"
    -DCODE=1 "-DREGEX=tests/diag/dma\\.a78: SHA-256 drift"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS built portable TIMEOUT 3)  # 2 x 1.09 s, Windows arm64, slowest of 26 runs, run 39363678798

# Negative control: a copy of expected.csv with the first gating value
# changed must fail on that record. The driver runs from a generated script,
# so the emulator list needs no second level of escaping.
file(READ ${diag_dir}/expected.csv diag_text)
string(REPLACE "\ndma,null,241,hw\n" "\ndma,null,242,hw\n" diag_text "${diag_text}")
file(WRITE ${diag_out}/expected_wrong.csv "${diag_text}")
set(diag_expected_script ${diag_out}/expected_self_test.cmake)
file(GENERATE OUTPUT ${diag_expected_script} CONTENT
"execute_process(COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} [==[$<TARGET_FILE:muhtari-test-diag>]==]
  [==[${diag_dir}/dma.a78]==] [==[${diag_out}/expected_wrong.csv]==]
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
message(\"\${out}\")
if(NOT rc STREQUAL \"0\")
  message(FATAL_ERROR \"muhtari-test-diag exited \${rc}\")
endif()
")
muhtari_add_test(NAME diag.expected.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${CMAKE_COMMAND}$<SEMICOLON>-P$<SEMICOLON>${diag_expected_script}"
    -DCODE=1 "-DREGEX=dma null: 241 expected 242 .hw. id 1 flags 01 FAIL.*exited 1"
    -P ${PROJECT_SOURCE_DIR}/tests/cmake/expect_fail.cmake
  LABELS diag portable TIMEOUT 1)  # 2 x 0.26 s, macOS x64, slowest of 26 runs, run 39354563718
