# Build a 7800basic sample from the pinned tools and check its bytes, run as
#   cmake -DWASMTIME=<wasmtime> -DPKG=<7800basic package> -DOUT=<dir>
#         -DEXPECTED=<expected.txt> -DNAME=<sample> -P build.cmake
# Fault prevented: a tool or host difference changing the sample's bytes
# unnoticed. Every platform must build byte-identical files (ASSET_POLICY.md
# section 3), and the probe tests read them as fixtures.
#
# Our own recipe, taken from each tool's usage output; 7800basic's own build
# script is GPL-2 and is never read. Each stage runs under wasmtime in a fresh
# OUT/NAME with the package mapped to /b, so no host path reaches the guest and
# one recipe serves every OS. A reused directory would hand postprocess a
# stale 7800.asm. The built files stay in the build tree.
cmake_minimum_required(VERSION 3.25)
foreach(var WASMTIME PKG OUT EXPECTED NAME)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "built: -D${var} is required")
  endif()
endforeach()

# The row: name, tool pins, source, .a78 SHA-256, .bin SHA-256, frame, audio.
file(STRINGS "${EXPECTED}" rows REGEX "^${NAME}[ \t]")
list(LENGTH rows count)
if(NOT count EQUAL 1)
  message(FATAL_ERROR "${NAME}: ${EXPECTED} has ${count} rows for it, not 1")
endif()
string(REGEX REPLACE "[ \t]+" ";" row "${rows}")
list(GET row 2 source)
list(GET row 3 want_a78)
list(GET row 4 want_bin)

file(TO_CMAKE_PATH "${PKG}" pkg)
get_filename_component(source_dir "${pkg}/${source}" DIRECTORY)
get_filename_component(bas "${source}" NAME)
set(dir "${OUT}/${NAME}")
file(REMOVE_RECURSE "${dir}")
file(MAKE_DIRECTORY "${dir}")
file(GLOB inputs "${source_dir}/*")
file(COPY ${inputs} DESTINATION "${dir}")

# Reuse compiled pinned modules across sample/diagnostic processes, not their
# outputs: every stage still runs in its fresh directory and checks its bytes.
# Each recipe/name has its own config, so parallel fixtures never rewrite one.
get_filename_component(cache "${pkg}/../wasmtime-cache" ABSOLUTE)
file(TO_CMAKE_PATH "${cache}" cache)
string(REPLACE "\"" "\\\"" cache_toml "${cache}")
file(MAKE_DIRECTORY "${cache}")
set(cache_config "${OUT}/.wasmtime-${NAME}.toml")
file(CONFIGURE OUTPUT "${cache_config}" CONTENT "[cache]\ndirectory = \"${cache_toml}\"\n" @ONLY)
get_filename_component(cache_config "${cache_config}" ABSOLUTE)
set(run "${WASMTIME}" run -C cache=y "-Ccache-config=${cache_config}" --dir=. "--dir=${pkg}::/b")

# stage(<name> [INPUT_FILE f] [OUTPUT_FILE f] COMMAND ... [COMMAND ...])
# One execute_process; a pipe is several COMMANDs. Fails naming the stage and
# every status unless all are 0.
function(stage name)
  execute_process(${ARGN} WORKING_DIRECTORY "${dir}"
    RESULTS_VARIABLE statuses ERROR_VARIABLE err)
  foreach(status IN LISTS statuses)
    if(NOT status STREQUAL "0")
      message(FATAL_ERROR "${NAME}: stage ${name} exited ${statuses}\n${err}")
    endif()
  endforeach()
endfunction()

stage(preprocess OUTPUT_FILE pre.bas
  COMMAND ${run} ${pkg}/7800preprocess.wasm ${bas})
stage(compile INPUT_FILE pre.bas OUTPUT_FILE compile.log
  COMMAND ${run} ${pkg}/7800basic.wasm -i /b)
stage(postprocess OUTPUT_FILE ${bas}.asm
  COMMAND ${run} ${pkg}/7800postprocess.wasm -i /b
  COMMAND ${run} ${pkg}/7800optimize.wasm)
stage(assemble OUTPUT_FILE assemble.log
  COMMAND ${run} ${pkg}/dasm.wasm ${bas}.asm -I/b/includes -f3 -p20 -o${bas}.bin
  COMMAND ${run} ${pkg}/7800filter.wasm)
stage(sign OUTPUT_FILE sign.log
  COMMAND ${run} ${pkg}/7800sign.wasm -w ${bas}.bin)
stage(header OUTPUT_FILE header.log
  COMMAND ${run} ${pkg}/7800header.wasm -o -f a78info.cfg ${bas}.bin)

foreach(kind a78 bin)
  set(file "${dir}/${bas}.${kind}")
  if(NOT EXISTS "${file}")
    message(FATAL_ERROR "${NAME}: the build wrote no ${bas}.${kind}")
  endif()
  file(SHA256 "${file}" got)
  if(NOT got STREQUAL want_${kind})
    message(FATAL_ERROR "${NAME}: .${kind} SHA-256 drift: ${EXPECTED} has ${want_${kind}}, "
      "the build gave ${got}")
  endif()
endforeach()
message(STATUS "${NAME}: .a78 ${want_a78} and .bin ${want_bin} as expected")
