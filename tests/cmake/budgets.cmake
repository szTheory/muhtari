# Document size budgets, run as
#   cmake -DROOT=<source dir> -P budgets.cmake
# Fault prevented: the README, the public header or a page in docs/ or
# docs/hardware/ growing
# past its budget, as manuals and design prose drift into them. Fails naming
# each file over budget with its size and limit, in bytes.
if(NOT IS_DIRECTORY "${ROOT}")
  message(FATAL_ERROR "budgets: -DROOT=<source dir> is required")
endif()

# Resolve relative roots from the caller's working directory once, so the
# document glob and all size checks inspect the same source tree.
get_filename_component(root "${ROOT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(checks "README.md=12288" "include/muhtari.h=24576")
file(GLOB docs RELATIVE "${root}" "${root}/docs/*.md" "${root}/docs/hardware/*.md")
foreach(doc IN LISTS docs)
  list(APPEND checks "${doc}=40960")
endforeach()

set(over "")
foreach(check IN LISTS checks)
  string(REGEX MATCH "^(.*)=([0-9]+)$" unused "${check}")
  set(file "${CMAKE_MATCH_1}")
  set(limit "${CMAKE_MATCH_2}")
  file(SIZE "${root}/${file}" size)
  if(size GREATER limit)
    string(APPEND over "\n  ${file}: ${size} bytes, budget ${limit}")
  endif()
endforeach()
if(over)
  message(FATAL_ERROR "budgets: over budget:${over}")
endif()
list(LENGTH checks count)
message(STATUS "budgets: ${count} files within budget")
