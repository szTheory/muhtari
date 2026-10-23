# Rebuild a committed diagnostic from its source and compare its bytes, run as
#   cmake -DWASMTIME=<wasmtime> -DPKG=<7800basic package> -DSRC=<rom>.s
#         [-DCFG=<rom>.cfg] -DOUT=<dir> -DMANIFEST=<manifest.txt>
#         [-DPATH=<repo path>] [-DVARIANT=65c02] [-DKEEP=<dest>] -P assemble.cmake
# Fault prevented: a source edit or a tool change leaving the committed bytes
# stale. The committed file's SHA-256 is field 5 of its manifest row.
#
# The stages are tests/built/build.cmake's, under wasmtime with the package
# mapped to /b: dasm.wasm <rom>.s -f3, and with a CFG 7800sign.wasm -w and
# 7800header.wasm -o -f <rom>.cfg, which writes <rom>.a78. Without a CFG the
# result is dasm's plain output, <rom>.bin. Every .s beside SRC is copied in,
# for its includes. PATH, the row to compare with, defaults to SRC's path from
# the repository root with the result's extension. VARIANT 65c02 builds
# tests/cpu/decimal/clark.s's 65C02 control: the copy calls A65C02 and S65C02
# for A6502 and S6502, as the file's header says. KEEP copies the result to
# KEEP and prints its digest instead of comparing, to create or update a
# fixture before its manifest row is written.
cmake_minimum_required(VERSION 3.25)
foreach(var WASMTIME PKG SRC OUT MANIFEST)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "assemble: -D${var} is required")
  endif()
endforeach()

# Paths given relative, as docs/emulation-tests.md writes them, are taken from
# the working directory.
foreach(var WASMTIME PKG SRC CFG OUT MANIFEST KEEP)
  if(NOT "${${var}}" STREQUAL "")
    get_filename_component(${var} "${${var}}" ABSOLUTE)
  endif()
endforeach()
file(TO_CMAKE_PATH "${PKG}" pkg)
get_filename_component(src_dir "${SRC}" DIRECTORY)
get_filename_component(name "${SRC}" NAME_WE)
if(CFG)
  set(result ${name}.a78)
else()
  set(result ${name}.bin)
endif()
if("${PATH}" STREQUAL "")
  get_filename_component(root "${MANIFEST}/../../.." ABSOLUTE)
  file(RELATIVE_PATH PATH "${root}" "${src_dir}/${result}")
endif()

set(dir "${OUT}/${name}")
file(REMOVE_RECURSE "${dir}")
file(MAKE_DIRECTORY "${dir}")
file(GLOB sources "${src_dir}/*.s")
file(COPY ${sources} ${CFG} DESTINATION "${dir}")
if(VARIANT STREQUAL "65c02")
  file(READ "${dir}/${name}.s" text)
  string(REGEX REPLACE "JSR ([AS])6502" "JSR \\165C02" text "${text}")
  file(WRITE "${dir}/${name}.s" "${text}")
elseif(NOT "${VARIANT}" STREQUAL "")
  message(FATAL_ERROR "assemble: unknown -DVARIANT=${VARIANT}")
endif()

# Share only compiled pinned modules with the sample recipe. Sources, signing
# and headers still execute from scratch; each output/name owns its config.
get_filename_component(cache "${pkg}/../wasmtime-cache" ABSOLUTE)
file(TO_CMAKE_PATH "${cache}" cache)
string(REPLACE "\"" "\\\"" cache_toml "${cache}")
file(MAKE_DIRECTORY "${cache}")
set(cache_config "${OUT}/.wasmtime-${name}.toml")
file(CONFIGURE OUTPUT "${cache_config}" CONTENT "[cache]\ndirectory = \"${cache_toml}\"\n" @ONLY)
set(run "${WASMTIME}" run -C cache=y "-Ccache-config=${cache_config}" --dir=. "--dir=${pkg}::/b")

# stage(<name> COMMAND ...): fails naming the stage and its status.
function(stage stage_name)
  execute_process(${ARGN} WORKING_DIRECTORY "${dir}"
    RESULTS_VARIABLE statuses OUTPUT_VARIABLE out ERROR_VARIABLE err)
  foreach(status IN LISTS statuses)
    if(NOT status STREQUAL "0")
      message(FATAL_ERROR "${PATH}: stage ${stage_name} exited ${statuses}\n${out}${err}")
    endif()
  endforeach()
endfunction()

stage(assemble COMMAND ${run} ${pkg}/dasm.wasm ${name}.s -f3 -o${name}.bin)
if(CFG)
  get_filename_component(cfg_name "${CFG}" NAME)
  stage(sign COMMAND ${run} ${pkg}/7800sign.wasm -w ${name}.bin)
  stage(header COMMAND ${run} ${pkg}/7800header.wasm -o -f ${cfg_name} ${name}.bin)
endif()
if(NOT EXISTS "${dir}/${result}")
  message(FATAL_ERROR "${PATH}: the build wrote no ${result}")
endif()
file(SHA256 "${dir}/${result}" got)

if(KEEP)
  file(COPY_FILE "${dir}/${result}" "${KEEP}")
  message(STATUS "${PATH}: kept as ${KEEP}, SHA-256 ${got}")
  return()
endif()
file(STRINGS "${MANIFEST}" rows REGEX "^${PATH}\t")
list(LENGTH rows count)
if(NOT count EQUAL 1)
  message(FATAL_ERROR "${PATH}: ${MANIFEST} has ${count} rows for it, not 1")
endif()
string(REPLACE "\t" ";" row "${rows}")
list(GET row 4 want)
if(NOT got STREQUAL want)
  message(FATAL_ERROR "${PATH}: SHA-256 drift: the manifest has ${want}, the build gave ${got}")
endif()
message(STATUS "${PATH}: SHA-256 ${got} as the manifest has it")
