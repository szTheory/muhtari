# The hashes every lane prints, and their comparison across lanes, run as
#   cmake -DMODE=write -DRUN=<muhtari-run> -DDRIVER=<muhtari-test-diag>
#     -DDIAG=<tests/diag> [-DEMULATOR=<list>] -DOUT=<dir> -P hashes.cmake
#   cmake -DMODE=compare -DFILES=<list> -DCOUNT=<n> -P hashes.cmake

# Fault prevented: a board hash without its actual numeric mailbox, or a
# partial frame/audio report, silently joining cross-platform equality.
function(hashes_board name result)
  set(rom "${DIAG}/${name}.a78")
  if(NOT EXISTS "${rom}" OR NOT EXISTS "${DIAG}/expected.csv")
    message(FATAL_ERROR "hashes: missing ${name} diagnostic fixture or expectations")
  endif()
  execute_process(COMMAND ${EMULATOR} "${RUN}" --probe "${rom}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE probe ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "0" OR NOT probe MATCHES "\nboard ${name}\n")
    message(FATAL_ERROR "hashes: ${name} diagnostic board mismatch: ${rc} ${probe} ${err}")
  endif()
  execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 600 --hash-frame 1 --hash-frame 600
      --hash-audio --diagnostic "${rom}"
    RESULT_VARIABLE rc OUTPUT_VARIABLE observed ERROR_VARIABLE err TIMEOUT 30)
  string(REGEX MATCHALL "[^\n]+" lines "${observed}")
  list(LENGTH lines count)
  if(NOT rc STREQUAL "0" OR NOT count EQUAL 10 OR NOT observed MATCHES "\n$"
      OR observed MATCHES "\r")
    message(FATAL_ERROR "hashes: ${name} expected complete ten-field diagnostic report: ${rc} ${err}")
  endif()
  set(patterns "^frame 1 sha256 ([0-9a-f]+)$" "^frame 600 sha256 ([0-9a-f]+)$"
    "^audio_transitions_sha256 ([0-9a-f]+)$" "^audio_pcm_sha256 ([0-9a-f]+)$"
    "^audio_frames 600$" "^audio_samples [1-9][0-9]*$" "^audio_rate 48000/1$"
    "^audio_behaviour_revision 3$" "^audio_synthesis exact-q23-16x64-dc20$"
    "^diagnostic ([0-9a-f]+)$")
  set(content "")
  foreach(i RANGE 0 9)
    list(GET lines ${i} line)
    list(GET patterns ${i} pattern)
    if(NOT line MATCHES "${pattern}")
      message(FATAL_ERROR "hashes: ${name} malformed diagnostic field ${i}: ${line}")
    endif()
    if(i LESS 4 OR i EQUAL 9)
      string(LENGTH "${CMAKE_MATCH_1}" length)
      set(want 64)
      if(i EQUAL 9)
        set(want 512)
      endif()
      if(NOT length EQUAL want)
        message(FATAL_ERROR "hashes: ${name} diagnostic field ${i} length ${length}")
      endif()
    endif()
    string(REGEX REPLACE "^frame (1|600) sha256 " "frame\\1 " row "${line}")
    string(APPEND content "${name}.${row}\n")
  endforeach()
  file(MAKE_DIRECTORY "${OUT}")
  set(report "${OUT}/hashes-${name}-mailbox.txt")
  file(WRITE "${report}" "${line}\n")
  execute_process(COMMAND ${EMULATOR} "${DRIVER}" --mailbox "${report}" "${name}"
      "${DIAG}/expected.csv"
    RESULT_VARIABLE rc OUTPUT_VARIABLE verdict ERROR_VARIABLE err TIMEOUT 30)
  if(NOT rc STREQUAL "0")
    message(FATAL_ERROR "hashes: ${name} numeric diagnostic failed: ${rc} ${verdict} ${err}")
  endif()
  set(${result} "${content}" PARENT_SCOPE)
endfunction()
if(HASHES_FUNCTIONS_ONLY)
  return()
endif()
if(MODE STREQUAL "write")
  # Fault prevented: a lane's output silently not compared. Writes
  # <OUT>/hashes.txt with the frame 1 and frame 600 hashes and the SHA-256 of
  # the frame 1 PNG, the file CI collects from every lane.
  set(png "${OUT}/hashes-card.png")
  file(REMOVE "${png}")
  execute_process(COMMAND ${EMULATOR} "${RUN}" --frames 600 --hash-frame 1 --hash-frame 600
      --dump-frame "1:${png}"
    OUTPUT_VARIABLE out ERROR_VARIABLE err RESULT_VARIABLE rc)
  if(NOT rc STREQUAL "0"
      OR NOT out MATCHES "^frame 1 sha256 ([0-9a-f]+)\nframe 600 sha256 ([0-9a-f]+)\n$")
    message(FATAL_ERROR "muhtari-run: expected exit 0 and two hash lines, got exit ${rc}\n${out}${err}")
  endif()
  set(frame1 ${CMAKE_MATCH_1})
  set(frame600 ${CMAKE_MATCH_2})
  file(SHA256 "${png}" png_sha256)
  set(content "frame1 ${frame1}\nframe600 ${frame600}\npng ${png_sha256}\n")
  string(REPEAT "|....12|......|...BB|\n" 60 input)
  string(REPEAT "|......|......|...BB|\n" 540 released)
  file(WRITE "${OUT}/hashes-audio.movie" "muhtari-movie 1\nport1 proline\nport2 proline\n${input}${released}")
  foreach(name soundtest pokey)
    string(TOUPPER "${name}" key)
    if(NOT EXISTS "${${key}}")
      message(FATAL_ERROR "hashes: missing ${name} built audio fixture")
    endif()
    execute_process(COMMAND ${EMULATOR} "${RUN}" --movie "${OUT}/hashes-audio.movie"
        --hash-audio "${${key}}"
      OUTPUT_VARIABLE audio ERROR_VARIABLE err RESULT_VARIABLE rc)
    if(NOT rc STREQUAL "0" OR NOT audio MATCHES "audio_frames 600\n"
        OR audio MATCHES "audio_transitions_sha256 e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
      message(FATAL_ERROR "hashes: ${name} audio failed: ${rc} ${audio} ${err}")
    endif()
    string(REGEX MATCHALL "audio_[^\n]+" lines "${audio}")
    list(LENGTH lines audio_fields)
    if(NOT audio_fields EQUAL 7)
      message(FATAL_ERROR "hashes: ${name} expected seven audio fields")
    endif()
    foreach(line IN LISTS lines)
      string(APPEND content "${name}.${line}\n")
    endforeach()
  endforeach()
  foreach(name activision absolute fractalus)
    hashes_board("${name}" board)
    string(APPEND content "${board}")
  endforeach()
  # Unix newlines on every host: file(WRITE) writes CRLF on Windows.
  file(CONFIGURE OUTPUT "${OUT}/hashes.txt" NEWLINE_STYLE UNIX
    CONTENT "${content}")
elseif(MODE STREQUAL "compare")
  # Fault prevented: lanes that disagree, or a lane that stops uploading and
  # silently drops out. Passes only for exactly COUNT existing files that are
  # byte-identical.
  list(LENGTH FILES n)
  if(NOT n EQUAL COUNT)
    message(FATAL_ERROR "hashes: expected ${COUNT} files, got ${n}: ${FILES}")
  endif()
  list(GET FILES 0 first)
  foreach(file IN LISTS FILES)
    if(NOT EXISTS "${file}")
      message(FATAL_ERROR "hashes: ${file} does not exist")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files "${first}" "${file}"
      RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
    if(NOT rc EQUAL 0)
      file(READ "${first}" want)
      file(READ "${file}" got)
      message(FATAL_ERROR "hashes: ${file} differs from ${first}\n${first}:\n${want}${file}:\n${got}")
    endif()
  endforeach()
  message(STATUS "hashes: ${n} identical files")
else()
  message(FATAL_ERROR "hashes: -DMODE=write or -DMODE=compare is required")
endif()
