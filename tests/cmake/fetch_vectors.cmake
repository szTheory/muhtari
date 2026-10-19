# The full SingleStepTests 6502 set at its pin, run as the test
# cpu.vectors-full.fetch:
#   cmake -DDIR=<dir> -DSHA=<commit> -DURL=<repository> -DLIST=<6502-v1.sha256>
#         -DSOURCE_DIR=<source dir> -DGIT=<git> -P fetch_vectors.cmake
# Fault prevented: the nightly run testing vectors from another commit, or a
# damaged or partial download, while it reports green. Every one of the 256
# files DIR/6502/v1/xx.json must match its SHA-256 in LIST; a missing, extra
# or altered file fails with its path, and so does no network.
#
# When every file already verifies nothing is fetched. Otherwise DIR is
# replaced by a cone sparse, blobless, depth-1 checkout of 6502/v1 at SHA, its
# .git is deleted and every file is checked. No command that reads blobs
# (size, log, diff) runs in the blobless tree: each would fetch blobs one at a
# time. DIR is deleted only when it holds the marker file this script writes,
# and a DIR inside the source tree outside build/ is refused.
#
# Seeded from szTheory/nesturbator @9f9386c661f6 tests/cmake/fetch_vectors.cmake
# (same author, MIT): the in-source guard and the marker-file deletion rule.
cmake_minimum_required(VERSION 3.25)

foreach(var DIR SHA URL LIST SOURCE_DIR GIT)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "fetch_vectors: -D${var} is required")
  endif()
endforeach()
string(LENGTH "${SHA}" sha_len)
if(NOT SHA MATCHES "^[0-9a-f]+$" OR NOT sha_len EQUAL 40)
  message(FATAL_ERROR "fetch_vectors: SHA '${SHA}' is not 40 lowercase hex digits")
endif()
set(sub "6502/v1")

# DIR must not land in the source tree, except under build/. cmake_path
# compares text only, so the paths are compared again after REAL_PATH, which
# resolves symbolic links and the case stored on disk. REAL_PATH resolves only
# a path that exists, so the deepest existing ancestor of DIR is resolved and
# the rest appended. Lower-casing on macOS and Windows, whose file systems
# ignore case by default, covers the part that does not exist yet.
function(check_in_source dir_path src_path)
  cmake_path(IS_PREFIX src_path "${dir_path}" NORMALIZE in_source)
  cmake_path(APPEND src_path "build" OUTPUT_VARIABLE build_path)
  cmake_path(IS_PREFIX build_path "${dir_path}" NORMALIZE in_build)
  if(in_source AND NOT in_build)
    message(FATAL_ERROR "fetch_vectors: DIR ${dir_path} is inside the source tree but not under build/")
  endif()
endfunction()
cmake_path(ABSOLUTE_PATH DIR NORMALIZE OUTPUT_VARIABLE dir_abs)
cmake_path(ABSOLUTE_PATH SOURCE_DIR NORMALIZE OUTPUT_VARIABLE src_abs)
check_in_source("${dir_abs}" "${src_abs}")
set(probe "${dir_abs}")
set(rest "")
while(NOT EXISTS "${probe}")
  cmake_path(GET probe FILENAME leaf)
  if(rest STREQUAL "")
    set(rest "${leaf}")
  else()
    set(rest "${leaf}/${rest}")
  endif()
  cmake_path(GET probe PARENT_PATH probe)
endwhile()
file(REAL_PATH "${probe}" dir_real)
if(NOT rest STREQUAL "")
  cmake_path(APPEND dir_real "${rest}")
endif()
file(REAL_PATH "${src_abs}" src_real)
if(CMAKE_HOST_WIN32 OR CMAKE_HOST_APPLE)
  string(TOLOWER "${dir_real}" dir_real)
  string(TOLOWER "${src_real}" src_real)
endif()
check_in_source("${dir_real}" "${src_real}")

# The expected digest of each file, as sha_<xx>. The list is in path order,
# 00 to ff; cpu.vectors.pin checks it against the pin.
set(names "")
foreach(h 0 1 2 3 4 5 6 7 8 9 a b c d e f)
  foreach(l 0 1 2 3 4 5 6 7 8 9 a b c d e f)
    list(APPEND names ${h}${l})
  endforeach()
endforeach()
file(STRINGS "${LIST}" lines)
foreach(line IN LISTS lines)
  if(NOT line MATCHES "^([0-9a-f]+)  ${sub}/([0-9a-f][0-9a-f])\\.json$")
    message(FATAL_ERROR "fetch_vectors: ${LIST}: bad line: ${line}")
  endif()
  set(sha_${CMAKE_MATCH_2} "${CMAKE_MATCH_1}")
endforeach()
foreach(xx IN LISTS names)
  if(NOT DEFINED sha_${xx})
    message(FATAL_ERROR "fetch_vectors: ${LIST} has no line for ${sub}/${xx}.json")
  endif()
endforeach()

set(files "${dir_abs}/${sub}")

# Sets OUT to one message per file that is missing, extra or differs from LIST.
function(verify_files out)
  set(found "")
  file(GLOB present RELATIVE "${files}" "${files}/*")
  foreach(name IN LISTS present)
    string(REGEX REPLACE "\\.json$" "" xx "${name}")
    if(NOT xx IN_LIST names OR name STREQUAL xx)
      list(APPEND found "${sub}/${name}: not in ${LIST}")
    endif()
  endforeach()
  foreach(xx IN LISTS names)
    set(path "${files}/${xx}.json")
    if(NOT EXISTS "${path}")
      list(APPEND found "${sub}/${xx}.json: missing")
      continue()
    endif()
    file(SHA256 "${path}" sha)
    if(NOT sha STREQUAL sha_${xx})
      list(APPEND found "${sub}/${xx}.json: expected sha256 ${sha_${xx}}, actual ${sha}")
    endif()
  endforeach()
  set(${out} "${found}" PARENT_SCOPE)
endfunction()

verify_files(found)
if(NOT found)
  message(STATUS "vectors-full: 256 files verify at ${SHA}; nothing fetched")
  return()
endif()
list(LENGTH found n)
message(STATUS "vectors-full: ${n} of 256 files do not verify; fetching ${SHA}")

# The script deletes only a DIR it created, which holds its marker file.
set(marker "${dir_abs}/.muhtari-vectors")
if(EXISTS "${dir_abs}" AND NOT EXISTS "${marker}")
  file(GLOB any "${dir_abs}/*" "${dir_abs}/.*")
  if(any)
    message(FATAL_ERROR "fetch_vectors: ${dir_abs} exists and was not created by this script; refusing to delete it")
  endif()
endif()
file(REMOVE_RECURSE "${dir_abs}")
file(MAKE_DIRECTORY "${dir_abs}")
file(TOUCH "${marker}")

# A stalled transfer fails after GIT_HTTP_LOW_SPEED_TIME seconds below 1000
# B/s, and git never prompts. The fetch reads trees only (measured 1 s), so 60
# s. The checkout downloads the blobs lazily, and the server sends nothing
# while it packs them: measured 75 to 80 s silent, then 180 MiB in 15 s. The
# checkout's limit is therefore 240 s, three times that silence; with 60 s it
# failed with "Operation too slow" every time.
set(ENV{GIT_HTTP_LOW_SPEED_LIMIT} 1000)
set(ENV{GIT_HTTP_LOW_SPEED_TIME} 60)
set(ENV{GIT_TERMINAL_PROMPT} 0)

function(run_git)
  execute_process(COMMAND "${GIT}" ${ARGN}
    WORKING_DIRECTORY "${dir_abs}"
    OUTPUT_VARIABLE git_out ERROR_VARIABLE git_err RESULT_VARIABLE git_rc)
  if(NOT git_rc EQUAL 0)
    message(FATAL_ERROR "fetch_vectors: git ${ARGN} exited ${git_rc}\n${git_out}${git_err}")
  endif()
  set(git_out "${git_out}" PARENT_SCOPE)
endfunction()

string(TIMESTAMP start "%s" UTC)
run_git(-c init.defaultBranch=main init -q)
run_git(remote add origin "${URL}")
run_git(sparse-checkout set --cone "${sub}")
run_git(-c gc.auto=0 fetch -q --depth 1 --filter=blob:none origin "${SHA}")
set(ENV{GIT_HTTP_LOW_SPEED_LIMIT} 1000)
set(ENV{GIT_HTTP_LOW_SPEED_TIME} 240)
run_git(-c advice.detachedHead=false checkout -q FETCH_HEAD)
run_git(rev-parse HEAD)
string(STRIP "${git_out}" head)
if(NOT head STREQUAL SHA)
  message(FATAL_ERROR "fetch_vectors: HEAD is ${head}, expected ${SHA}")
endif()
file(REMOVE_RECURSE "${dir_abs}/.git")
string(TIMESTAMP end "%s" UTC)
math(EXPR took "${end} - ${start}")

verify_files(found)
if(found)
  list(JOIN found "\n  " found)
  message(FATAL_ERROR "fetch_vectors: the files fetched at ${SHA} do not match ${LIST}:\n  ${found}")
endif()
message(STATUS "vectors-full: fetched ${SHA} in ${took} s; 256 files verify")
