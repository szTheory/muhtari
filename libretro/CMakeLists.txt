# The libretro core: a module RetroArch loads at run time. Hidden visibility
# here and in the two static libraries it links leaves only the retro_*
# functions, which libretro.h marks with default visibility or dllexport,
# in the export table.
function(muhtari_libretro_module name)
  add_library(${name} MODULE libretro.c)
  target_include_directories(${name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/generated)
  target_link_libraries(${name} PRIVATE muhtari::muhtari muhtari_host)
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
