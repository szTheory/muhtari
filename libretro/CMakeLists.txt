# The libretro core: a module RetroArch loads at run time. Hidden visibility
# here and in the two static libraries it links leaves only the retro_*
# functions, which libretro.h marks with default visibility or dllexport,
# in the export table.
function(muhtari_libretro_module name)
  # The runner's SHA-256 names the BIOS file a user supplied in the log.
  if(EMSCRIPTEN AND "${name}" STREQUAL "muhtari_libretro")
    # A browser/Node consumer imports the generated loader, not an Emscripten
    # side module. Keep this branch on the installed production target only.
    add_executable(${name} libretro.c ${PROJECT_SOURCE_DIR}/runner/sha256.c)
    target_link_options(${name} PRIVATE
      "-sMODULARIZE=1"
      "-sEXPORT_ES6=1"
      "-sENVIRONMENT=web,node"
      "-Wl,--no-entry"
      "-sINCOMING_MODULE_JS_API=['wasmBinary']"
      "-sALLOW_TABLE_GROWTH=1"
      "-sALLOW_MEMORY_GROWTH=1"
      "-sEXPORTED_FUNCTIONS=['_retro_api_version','_retro_get_system_info','_retro_get_system_av_info','_retro_set_environment','_retro_set_video_refresh','_retro_set_audio_sample','_retro_set_audio_sample_batch','_retro_set_input_poll','_retro_set_input_state','_retro_init','_retro_deinit','_retro_run','_retro_reset','_retro_load_game','_retro_unload_game','_retro_serialize_size','_retro_serialize','_retro_unserialize','_retro_get_memory_data','_retro_get_memory_size','_retro_get_region','_retro_set_controller_port_device','_malloc','_free']"
      "-sEXPORTED_RUNTIME_METHODS=['addFunction','removeFunction','HEAPU8','HEAPU32']")
    set_target_properties(${name} PROPERTIES SUFFIX ".mjs")
  else()
    add_library(${name} MODULE libretro.c ${PROJECT_SOURCE_DIR}/runner/sha256.c)
  endif()
  target_include_directories(${name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/generated ${PROJECT_SOURCE_DIR}/runner)
  target_link_libraries(${name} PRIVATE muhtari::muhtari muhtari_host)
  # MSVC deprecates ISO C's fopen, which reads the BIOS file, for its _s forms.
  target_compile_definitions(${name} PRIVATE $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
  # No "lib" prefix: the frontend and the release name it muhtari_libretro.
  # CMake gives a MODULE the suffix .so on macOS; RetroArch expects .dylib.
  set_target_properties(${name} PROPERTIES
    PREFIX ""
    C_VISIBILITY_PRESET hidden)
  if(APPLE)
    set_target_properties(${name} PROPERTIES SUFFIX ".dylib")
  endif()
  muhtari_warnings(${name})
endfunction()

# Fingerprint every project source and header that feeds the library, host
# helpers or libretro adapter. Build-owned observers use this to refuse
# cross-build evidence from a different implementation snapshot.
file(GLOB _muhtari_core_sources CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.c")
file(GLOB _muhtari_core_headers CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.h")
file(GLOB _muhtari_host_sources CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/host/*.c")
file(GLOB _muhtari_host_headers CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/host/*.h")
set(_muhtari_source_files
  ${_muhtari_core_sources}
  ${_muhtari_core_headers}
  ${_muhtari_host_sources}
  ${_muhtari_host_headers}
  "${PROJECT_SOURCE_DIR}/include/muhtari.h"
  "${PROJECT_SOURCE_DIR}/libretro/libretro.c"
  "${PROJECT_SOURCE_DIR}/libretro/libretro.h"
  "${PROJECT_SOURCE_DIR}/runner/sha256.c"
  "${PROJECT_SOURCE_DIR}/runner/sha256.h"
  "${PROJECT_SOURCE_DIR}/version.txt"
  "${PROJECT_SOURCE_DIR}/cmake/version.h.in"
  "${PROJECT_BINARY_DIR}/generated/muhtari_version.h")
list(SORT _muhtari_source_files)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${_muhtari_source_files})
set(_muhtari_source_material "")
foreach(_source IN LISTS _muhtari_source_files)
  file(SHA256 "${_source}" _source_hash)
  if(_source STREQUAL "${PROJECT_BINARY_DIR}/generated/muhtari_version.h")
    # Build-tree paths differ between native and Emscripten builds. Hash the
    # generated compile input under one stable source identity name.
    set(_source_relative "generated/muhtari_version.h")
  else()
    file(RELATIVE_PATH _source_relative "${PROJECT_SOURCE_DIR}" "${_source}")
  endif()
  string(APPEND _muhtari_source_material "${_source_relative}:${_source_hash}\n")
endforeach()
string(SHA256 MUHTARI_CORE_SOURCE_ID "${_muhtari_source_material}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/muhtari-source-id.txt" "${MUHTARI_CORE_SOURCE_ID}\n")

muhtari_libretro_module(muhtari_libretro)
# Test only, never installed or packaged: the same adapter in RGB565, so the
# in-process host proves the 16-bit path and the single-sample audio path.
if(NOT EMSCRIPTEN)
  muhtari_libretro_module(muhtari_libretro_rgb565)
  target_compile_definitions(muhtari_libretro_rgb565 PRIVATE USE_RGB565)
endif()

# Core information for RetroArch's info directory, with display_version
# taken from version.txt.
configure_file(muhtari_libretro.info.in
  ${CMAKE_CURRENT_BINARY_DIR}/muhtari_libretro.info @ONLY)

# RetroArch's layout: the module in cores/, its information file in info/.
# RUNTIME covers the Windows DLL.
install(TARGETS muhtari_libretro
  LIBRARY DESTINATION cores COMPONENT libretro
  RUNTIME DESTINATION cores COMPONENT libretro)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/muhtari_libretro.info
  DESTINATION info COMPONENT libretro)
