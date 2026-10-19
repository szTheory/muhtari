# The committed sample against the full set, run as the test
# cpu.vectors-full.sample_match:
#   cmake -DVECCONV=<muhtari-vecconv> -DDIR=<vectors dir> -DSAMPLE=<6502-sample.n65v>
#         -DWORK=<dir> [-DWRITE=ON] -P vectors_sample_match.cmake
# Fault prevented: the per-pull-request sample drifting from the pinned set,
# so every pull request tests vectors that are not upstream's. The first 100
# tests of each DIR/6502/v1/xx.json are converted, the 256 chunks joined in
# order from 00 to ff, and the result must equal SAMPLE byte for byte; the
# first difference fails naming its opcode's chunk. WRITE=ON copies the
# joined file over SAMPLE instead, for a pin bump.
#
# Seeded from szTheory/nesturbator @9f9386c661f6
# tests/cmake/vectors_sample_match.cmake (same author, MIT).
cmake_minimum_required(VERSION 3.25)

foreach(var VECCONV DIR SAMPLE WORK)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "vectors_sample_match: -D${var} is required")
  endif()
endforeach()

file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}")
set(chunks "")
foreach(h 0 1 2 3 4 5 6 7 8 9 a b c d e f)
  foreach(l 0 1 2 3 4 5 6 7 8 9 a b c d e f)
    set(xx ${h}${l})
    set(chunk "${WORK}/${xx}.n65v")
    execute_process(COMMAND "${VECCONV}" "${DIR}/6502/v1/${xx}.json" "${chunk}" ${xx} --first 100
      OUTPUT_VARIABLE out ERROR_VARIABLE out RESULT_VARIABLE rc)
    if(NOT rc EQUAL 0)
      message(FATAL_ERROR "sample_match: vecconv ${xx} exited ${rc}\n${out}")
    endif()
    list(APPEND chunks "${chunk}")
  endforeach()
endforeach()

set(joined "${WORK}/joined.n65v")
execute_process(COMMAND "${CMAKE_COMMAND}" -E cat ${chunks} OUTPUT_FILE "${joined}" RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "sample_match: cmake -E cat exited ${rc}")
endif()

if(WRITE)
  file(COPY_FILE "${joined}" "${SAMPLE}")
  file(SIZE "${SAMPLE}" size)
  message(STATUS "sample_match: wrote ${SAMPLE}, ${size} bytes from 256 chunks")
  return()
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${joined}" "${SAMPLE}" RESULT_VARIABLE rc)
if(rc EQUAL 0)
  file(SIZE "${SAMPLE}" size)
  message(STATUS "sample_match: 256 chunks equal ${SAMPLE}, ${size} bytes")
  return()
endif()

# Name the first chunk that differs, by its offset in SAMPLE.
file(SIZE "${SAMPLE}" sample_size)
set(o 0)
foreach(chunk IN LISTS chunks)
  file(SIZE "${chunk}" n)
  math(EXPR end "${o} + ${n}")
  if(end GREATER sample_size)
    message(FATAL_ERROR "sample_match: ${chunk} runs past the end of ${SAMPLE} (${sample_size} bytes) at offset ${o}")
  endif()
  file(READ "${chunk}" got HEX)
  file(READ "${SAMPLE}" want OFFSET ${o} LIMIT ${n} HEX)
  if(NOT got STREQUAL want)
    message(FATAL_ERROR "sample_match: ${chunk} differs from ${SAMPLE} at offset ${o}")
  endif()
  set(o ${end})
endforeach()
message(FATAL_ERROR "sample_match: the 256 chunks end at offset ${o}; ${SAMPLE} holds ${sample_size} bytes")
