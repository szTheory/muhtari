# Runs the README's RetroArch install line for Apple Silicon, as
#   cmake -DREADME=<README.md> -DBASE_URL=<url ending in />
#         [-DHOME_DIR=<empty scratch dir>] -P install_line.cmake
# Fault prevented: the README's install line no longer installs the core.
# The one README line beginning "mkdir -p ~/Library/Application" runs
# verbatim under sh with HOME set to a scratch directory; only its release
# download base is replaced by BASE_URL, a file:// directory URL for a local
# build or the release URL after publishing. The core and its information
# file must then exist under the RetroArch directory. macOS only: the line
# relies on macOS's tar, which reads a zip on standard input.
foreach(var README BASE_URL)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "install_line: -D${var}=... is required")
  endif()
endforeach()
if(NOT CMAKE_HOST_APPLE)
  message(FATAL_ERROR "install_line: runs on macOS only, whose tar reads a zip on stdin")
endif()
if(NOT DEFINED HOME_DIR)
  set(HOME_DIR "$ENV{TMPDIR}")
  if(HOME_DIR STREQUAL "")
    set(HOME_DIR /tmp)
  endif()
  set(HOME_DIR "${HOME_DIR}/muhtari-install-line")
endif()

file(STRINGS "${README}" lines REGEX "^mkdir -p ~/Library/Application")
list(LENGTH lines count)
if(NOT count EQUAL 1)
  message(FATAL_ERROR "install_line: ${count} lines in ${README} begin "
    "'mkdir -p ~/Library/Application', expected 1")
endif()
set(release_base "https://github.com/szTheory/muhtari/releases/latest/download/")
string(FIND "${lines}" "${release_base}" at)
if(at EQUAL -1)
  message(FATAL_ERROR "install_line: the line does not download from ${release_base}:\n${lines}")
endif()
string(REPLACE "${release_base}" "${BASE_URL}" line "${lines}")

file(REMOVE_RECURSE "${HOME_DIR}")
file(MAKE_DIRECTORY "${HOME_DIR}")
execute_process(COMMAND ${CMAKE_COMMAND} -E env "HOME=${HOME_DIR}" sh -c "${line}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE out)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "install_line: exit ${rc} from\n  ${line}\n${out}")
endif()
foreach(want cores/muhtari_libretro.dylib info/muhtari_libretro.info)
  set(path "Library/Application Support/RetroArch/${want}")
  if(NOT EXISTS "${HOME_DIR}/${path}")
    message(FATAL_ERROR "install_line: the line did not install ~/${path}")
  endif()
  message(STATUS "install_line: installed ~/${path}")
endforeach()
file(REMOVE_RECURSE "${HOME_DIR}")
