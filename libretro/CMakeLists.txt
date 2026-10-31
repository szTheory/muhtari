# The libretro core: a module RetroArch loads at run time. Hidden visibility
# here and in the two static libraries it links leaves only the retro_*
# functions, which libretro.h marks with default visibility or dllexport,
# in the export table.
function(muhtari_libretro_module name)
  # The runner's SHA-256 names the BIOS file a user supplied in the log.
  add_library(${name} MODULE libretro.c ${PROJECT_SOURCE_DIR}/runner/sha256.c)
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

muhtari_libretro_module(muhtari_libretro)
# Test only, never installed or packaged: the same adapter in RGB565, so the
# in-process host proves the 16-bit path and the single-sample audio path.
muhtari_libretro_module(muhtari_libretro_rgb565)
target_compile_definitions(muhtari_libretro_rgb565 PRIVATE USE_RGB565)

# POSIX local diagnostic only; no install/package rule and no platform input API.
if(MUHTARI_TESTS AND UNIX)
  muhtari_libretro_module(muhtari_libretro_input_trace)
  target_sources(muhtari_libretro_input_trace PRIVATE
    ${PROJECT_SOURCE_DIR}/tests/libretro/input_trace.c)
  target_include_directories(muhtari_libretro_input_trace PRIVATE
    ${PROJECT_SOURCE_DIR}/tests/libretro)
  target_compile_definitions(muhtari_libretro_input_trace PRIVATE MUHTARI_INPUT_TRACE)
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
