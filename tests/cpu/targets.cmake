# The CPU test programs. Each links the shipped CPU object with the flat test
# bus, which defines the two bus functions, and never links muhtari: the
# object under test is the object that ships, and the linker rejects any
# reference cpu.c makes beyond the bus and the instance's fields.
set(cpu_vectors_dir ${CMAKE_SOURCE_DIR}/tests/vectors)
add_executable(muhtari-test-vectors
  ${CMAKE_CURRENT_LIST_DIR}/test_vectors.c
  ${CMAKE_CURRENT_LIST_DIR}/vector_bus.c
  ${cpu_vectors_dir}/n65v.c
  ${cpu_vectors_dir}/sst_json.c
  $<TARGET_OBJECTS:muhtari_cpu>)
target_include_directories(muhtari-test-vectors PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}
  ${cpu_vectors_dir})
set_target_properties(muhtari-test-vectors PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-vectors)

# The converter from upstream JSON to N65V, which builds the committed
# sample. A test tool: never installed.
add_executable(muhtari-vecconv
  ${cpu_vectors_dir}/vecconv.c
  ${cpu_vectors_dir}/n65v.c
  ${cpu_vectors_dir}/sst_json.c)
target_include_directories(muhtari-vecconv PRIVATE ${cpu_vectors_dir})
set_target_properties(muhtari-vecconv PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-vecconv)

# CPU rules on the flat test bus, one case per argv.
add_executable(muhtari-test-cpu
  ${CMAKE_CURRENT_LIST_DIR}/test_cpu.c
  ${CMAKE_CURRENT_LIST_DIR}/vector_bus.c
  $<TARGET_OBJECTS:muhtari_cpu>)
target_include_directories(muhtari-test-cpu PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}
  ${cpu_vectors_dir})
set_target_properties(muhtari-test-cpu PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
muhtari_warnings(muhtari-test-cpu)
