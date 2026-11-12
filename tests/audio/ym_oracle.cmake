cmake_minimum_required(VERSION 3.21)

foreach(required ROOT SEED WORK)
  if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
    message(FATAL_ERROR "ym_oracle.cmake requires -D${required}=...")
  endif()
endforeach()

get_filename_component(root_dir "${ROOT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
if(NOT EXISTS "${root_dir}/AGENTS.md")
  message(FATAL_ERROR "ROOT does not resolve to the repository: ${root_dir}")
endif()
if(IS_ABSOLUTE "${SEED}")
  set(seed_dir "${SEED}")
else()
  set(seed_dir "${root_dir}/${SEED}")
endif()
if(IS_ABSOLUTE "${WORK}")
  set(work_dir "${WORK}")
else()
  set(work_dir "${root_dir}/${WORK}")
endif()

set(receipt_path "${seed_dir}/source-receipt.json")
set(object_path "${seed_dir}/ymfm_opm.o")
set(library_path "${root_dir}/build/ci/libmuhtari.a")
if(NOT EXISTS "${receipt_path}" OR NOT EXISTS "${object_path}" OR NOT EXISTS "${library_path}")
  message(FATAL_ERROR "prepared receipt, exact seed object, and built C library are all required")
endif()

file(READ "${receipt_path}" receipt)
string(JSON pin ERROR_VARIABLE json_error GET "${receipt}" pin)
if(json_error OR NOT pin STREQUAL "81aec25ccbb98f4873a255f7551ac4dadac59b4a")
  message(FATAL_ERROR "the prepared seed receipt has an unexpected source pin")
endif()
string(JSON input_count ERROR_VARIABLE json_error LENGTH "${receipt}" files)
if(json_error OR NOT input_count EQUAL 6)
  message(FATAL_ERROR "the receipt must name exactly the six approved seed inputs")
endif()

set(approved_inputs
  LICENSE
  src/ymfm_opm.cpp
  src/ymfm_opm.h
  src/ymfm_fm.ipp
  src/ymfm_fm.h
  src/ymfm.h)
foreach(relative IN LISTS approved_inputs)
  string(JSON expected_hash ERROR_VARIABLE json_error GET "${receipt}" files "${relative}")
  if(json_error OR NOT EXISTS "${seed_dir}/${relative}")
    message(FATAL_ERROR "approved seed input is missing from receipt or filesystem: ${relative}")
  endif()
  file(SHA256 "${seed_dir}/${relative}" actual_hash)
  if(NOT actual_hash STREQUAL expected_hash)
    message(FATAL_ERROR "prepared seed input hash mismatch: ${relative}")
  endif()
endforeach()

string(JSON expected_object ERROR_VARIABLE json_error GET "${receipt}" opm_object_sha256)
file(SHA256 "${object_path}" actual_object)
if(json_error OR NOT actual_object STREQUAL expected_object)
  message(FATAL_ERROR "prepared OPM object hash differs from its receipt")
endif()

find_program(CXX_COMPILER NAMES c++ clang++ g++)
if(NOT CXX_COMPILER)
  message(FATAL_ERROR "a local C++17 compiler is required for the test-only pinned oracle")
endif()
file(MAKE_DIRECTORY "${work_dir}")
set(oracle "${work_dir}/ym_oracle")
execute_process(
  COMMAND "${CXX_COMPILER}" -std=c++17 -O2 -Wall -Wextra -Werror
    -I "${root_dir}/src" -I "${root_dir}/include" -isystem "${seed_dir}/src"
    "${root_dir}/tests/audio/ym_oracle.cpp" "${library_path}" "${object_path}"
    -o "${oracle}"
  WORKING_DIRECTORY "${root_dir}"
  RESULT_VARIABLE compile_rc OUTPUT_VARIABLE compile_out ERROR_VARIABLE compile_err)
if(NOT compile_rc STREQUAL "0")
  message(FATAL_ERROR "oracle harness compile/link failed: ${compile_rc}\n${compile_out}\n${compile_err}")
endif()
message(STATUS "oracle harness compiled against the already-built receipt-matched object")

execute_process(
  COMMAND "${oracle}"
  WORKING_DIRECTORY "${root_dir}"
  RESULT_VARIABLE run_rc OUTPUT_VARIABLE run_out ERROR_VARIABLE run_err)
message(STATUS "${run_out}")
if(NOT run_rc STREQUAL "0")
  message(FATAL_ERROR "actual C-vs-pinned-OPM comparison failed: ${run_rc}\n${run_err}")
endif()
