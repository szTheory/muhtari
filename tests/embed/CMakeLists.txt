# A C-only project that embeds muhtari with add_subdirectory, as a host using
# FetchContent would. The test embed.subdirectory builds it and runs it.
# Configuring fails if embedding enabled C++, defined any target besides the
# library and its alias, or registered muhtari's tests.
cmake_minimum_required(VERSION 3.25)
project(embed C)

if(NOT DEFINED MUHTARI_SOURCE_DIR)
  message(FATAL_ERROR "embed: set MUHTARI_SOURCE_DIR")
endif()
add_subdirectory(${MUHTARI_SOURCE_DIR} muhtari)

get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if("CXX" IN_LIST languages)
  message(FATAL_ERROR "embed: muhtari enabled C++ in an embedding project")
endif()
# Besides the library, only the object libraries it is built from, which
# compile nothing an embedder does not already get.
get_property(targets DIRECTORY ${MUHTARI_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
set(others ${targets})
list(REMOVE_ITEM others muhtari)
foreach(target IN LISTS others)
  get_target_property(type ${target} TYPE)
  if(NOT type STREQUAL "OBJECT_LIBRARY" OR NOT target MATCHES "^muhtari_")
    set(others_bad TRUE)
  endif()
endforeach()
if(NOT "muhtari" IN_LIST targets OR others_bad)
  message(FATAL_ERROR "embed: muhtari added the targets '${targets}', not 'muhtari' and its "
    "muhtari_* object libraries")
endif()
if(NOT TARGET muhtari::muhtari)
  message(FATAL_ERROR "embed: muhtari did not add the alias muhtari::muhtari")
endif()
get_property(tests DIRECTORY ${MUHTARI_SOURCE_DIR} PROPERTY TESTS)
if(tests)
  message(FATAL_ERROR "embed: muhtari registered tests in an embedding project: ${tests}")
endif()

add_executable(embed ${CMAKE_CURRENT_SOURCE_DIR}/../consumer/main.c)
target_link_libraries(embed PRIVATE muhtari::muhtari)
