# Preserve the failed launch's evidence even when a later pixel judge fails.
cmake_minimum_required(VERSION 3.25)
if("${BUILD}" STREQUAL "")
  message(FATAL_ERROR "retroarch report: -DBUILD is required")
endif()
file(GLOB evidence "${BUILD}/ra-*/retroarch.log" "${BUILD}/ra-*/test.cfg"
  "${BUILD}/frames/input.ratst")
if(NOT evidence)
  message(STATUS "No RetroArch launch evidence recorded")
endif()
foreach(path IN LISTS evidence)
  file(READ "${path}" contents)
  message("${path}:\n${contents}")
endforeach()
