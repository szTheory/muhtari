# Every subsystem's tests. Each file is include()d, never add_subdirectory()d,
# so the top-level directory's TESTS property holds every name and the
# inventory check in cmake/MuhtariTests.cmake sees them all. The hygiene lane
# registers only its own tests, and the nofp lane only the abi tests.
if(MUHTARI_HYGIENE)
  include(${CMAKE_CURRENT_LIST_DIR}/hygiene/tests.cmake)
  return()
endif()
if(MUHTARI_NOFP)
  include(${CMAKE_CURRENT_LIST_DIR}/abi/tests.cmake)
  return()
endif()
include(${CMAKE_CURRENT_LIST_DIR}/abi/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/bus/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/core/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cpu/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/host/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/install/tests.cmake)
# Without the libretro core (wasm32: WASI has no dlopen) none of its tests
# applies.
if(MUHTARI_BUILD_LIBRETRO)
  include(${CMAKE_CURRENT_LIST_DIR}/libretro/tests.cmake)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/perf/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/policy/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/release/tests.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/runner/tests.cmake)
