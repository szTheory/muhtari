# Test-list keys against the bundled tables, run as
#   cmake -DTEST_LIST=<tests/cart/test_cartdb.c> -DBUNDLED=<src/cartdb.c> -P no_test_entries.cmake
# Fault prevented: a key from the test list shipping in the core, where it
# would change what the probe reports for a real image. Every byte row in
# either file ({0x..,...}) is read as hex; a test row whose full SHA-1 or
# whose first 8 bytes appear among the bundled rows or their first 8 bytes
# fails, naming the key.
foreach(var TEST_LIST BUNDLED)
  if(NOT DEFINED ${var} OR NOT EXISTS "${${var}}")
    message(FATAL_ERROR "no_test_entries: -D${var} must name an existing file")
  endif()
endforeach()

set(byte "0x[0-9a-fA-F][0-9a-fA-F]")
function(read_rows file out)
  file(READ "${file}" text)
  string(REGEX MATCHALL "{ *${byte}( *, *${byte})* *}" rows "${text}")
  set(hex "")
  foreach(row IN LISTS rows)
    string(REGEX REPLACE "[{} ,]|0x" "" row "${row}")
    string(TOLOWER "${row}" row)
    list(APPEND hex "${row}")
  endforeach()
  set(${out} "${hex}" PARENT_SCOPE)
endfunction()

read_rows("${TEST_LIST}" test_rows)
read_rows("${BUNDLED}" bundled_rows)
list(LENGTH test_rows count)
if(count EQUAL 0)
  message(FATAL_ERROR "no_test_entries: ${TEST_LIST} holds no key rows")
endif()
set(bundled_prefixes "")
foreach(row IN LISTS bundled_rows)
  string(SUBSTRING "${row}" 0 16 prefix)
  list(APPEND bundled_prefixes "${prefix}")
endforeach()

set(leaks "")
foreach(row IN LISTS test_rows)
  string(SUBSTRING "${row}" 0 16 prefix)
  if(row IN_LIST bundled_rows OR prefix IN_LIST bundled_prefixes)
    string(APPEND leaks "\n  ${BUNDLED} holds the test-list key ${row}")
  endif()
endforeach()
if(leaks)
  message(FATAL_ERROR "no_test_entries:${leaks}")
endif()
message(STATUS "no_test_entries: none of ${count} test-list keys is bundled")
