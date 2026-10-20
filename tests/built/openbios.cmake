# Build the 7800 OpenBIOS image from its pinned CC0 source, run as
#   cmake -DWASMTIME=<wasmtime> -DPKG=<7800basic package> -DASM=<7800openbios.asm>
#         -DOUT=<dir>/openbios.bin -DFILL=FF|00 -P openbios.cmake
# Fault prevented: a BIOS image that differs between legs or from the pinned
# source. The BIOS-path tests boot this image, so every platform must build
# the same bytes.
#
# The source includes two binaries of an in-BIOS game that is not CC0. They are
# never fetched: each incbin line is replaced by the same number of FILL bytes,
# and the build fails, naming the line, unless each occurs exactly once. The
# package's dasm assembles the result under wasmtime in a fresh directory, as
# build.cmake does. The image stays in the build tree.
cmake_minimum_required(VERSION 3.25)
foreach(var WASMTIME PKG ASM OUT FILL)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "openbios: -D${var} is required")
  endif()
endforeach()

# SHA-256 of the 16 KiB image for each fill. Both are rows of
# tests/release/firmware.txt, so a release archive can carry neither.
set(want_FF 46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2)
set(want_00 1ee4c780397c923447b245fcd75e3a276956a0fd130049d0a85041d058c3ad4f)
if(NOT DEFINED want_${FILL})
  message(FATAL_ERROR "openbios: -DFILL must be FF or 00, not ${FILL}")
endif()

file(TO_CMAKE_PATH "${PKG}" pkg)
file(TO_CMAKE_PATH "${OUT}" out)
get_filename_component(dir "${out}" DIRECTORY)
get_filename_component(bin "${out}" NAME)
file(REMOVE_RECURSE "${dir}")
file(MAKE_DIRECTORY "${dir}")

# Each binary and the fill that takes its place: 8 KiB of graphics at $C000
# and 896 bytes of game code.
file(READ "${ASM}" text)
foreach(pair "kiloparsec.bl1=8192" "kiloparsec.bl2=896")
  string(REPLACE "=" ";" pair "${pair}")
  list(GET pair 0 name)
  list(GET pair 1 size)
  set(line "incbin \"${name}\"")
  string(REPLACE "." "[.]" name_re "${name}")
  string(REGEX MATCHALL "incbin[ \t]+\"${name_re}\"" found "${text}")
  list(LENGTH found count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "openbios: '${line}' occurs ${count} times in ${ASM}, not once")
  endif()
  string(REGEX REPLACE "incbin[ \t]+\"${name_re}\"" "ds.b ${size},$${FILL}" text "${text}")
endforeach()
file(WRITE "${dir}/7800openbios.asm" "${text}")

# stage(<name> [OUTPUT_FILE f] COMMAND ...), as in build.cmake: fails naming
# the stage and its status unless it is 0.
function(stage name)
  execute_process(${ARGN} WORKING_DIRECTORY "${dir}"
    RESULTS_VARIABLE statuses ERROR_VARIABLE err)
  foreach(status IN LISTS statuses)
    if(NOT status STREQUAL "0")
      message(FATAL_ERROR "openbios: stage ${name} exited ${statuses} (${dir}/${name}.log)\n${err}")
    endif()
  endforeach()
endfunction()

stage(assemble OUTPUT_FILE assemble.log
  COMMAND "${WASMTIME}" run --dir=. ${pkg}/dasm.wasm 7800openbios.asm -f3 -o${bin})

if(NOT EXISTS "${out}")
  message(FATAL_ERROR "openbios: the build wrote no ${bin}")
endif()
file(SHA256 "${out}" got)
if(NOT got STREQUAL want_${FILL})
  message(FATAL_ERROR "openbios: SHA-256 drift for the ${FILL} fill: expected ${want_${FILL}}, "
    "the build gave ${got}")
endif()
message(STATUS "openbios: ${bin} ${got} as expected")
