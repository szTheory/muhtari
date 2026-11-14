# The Emscripten inventory tests the emitted production loader directly.
# Native fixture and observation evidence are required inputs from build/ci.
find_program(MUHTARI_EMSCRIPTEN_NODE node REQUIRED)
if(NOT EXISTS "${MUHTARI_EMSCRIPTEN_NODE}" OR IS_DIRECTORY "${MUHTARI_EMSCRIPTEN_NODE}")
  message(FATAL_ERROR "required bundled Node executable is missing: ${MUHTARI_EMSCRIPTEN_NODE}")
endif()
set(MUHTARI_EMSCRIPTEN_NATIVE_JSON
  "${PROJECT_SOURCE_DIR}/build/ci/tests/emscripten/native.json")
set(MUHTARI_EMSCRIPTEN_FIXTURE
  "${PROJECT_SOURCE_DIR}/build/ci/tests/input-carts/input.a78")
set(MUHTARI_EMSCRIPTEN_MODULE
  "${PROJECT_BINARY_DIR}/libretro/muhtari_libretro.mjs")
set(MUHTARI_EMSCRIPTEN_WASM
  "${PROJECT_BINARY_DIR}/libretro/muhtari_libretro.wasm")
set(MUHTARI_EMSCRIPTEN_SOUPER_NATIVE_JSON
  "${PROJECT_SOURCE_DIR}/build/ci/tests/emscripten/souper-native.json")
set(MUHTARI_EMSCRIPTEN_SOUPER_FIXTURE
  "${PROJECT_SOURCE_DIR}/build/ci/tests/diag/souper/souper.a78")
foreach(required IN ITEMS
    "${MUHTARI_EMSCRIPTEN_NATIVE_JSON}" "${MUHTARI_EMSCRIPTEN_FIXTURE}")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "required native Emscripten evidence is missing: ${required}")
  endif()
endforeach()
foreach(required IN ITEMS "${MUHTARI_EMSCRIPTEN_SOUPER_NATIVE_JSON}"
    "${MUHTARI_EMSCRIPTEN_SOUPER_FIXTURE}")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "required native Souper evidence is missing: ${required}")
  endif()
endforeach()

if(NOT TARGET muhtari_libretro)
  message(FATAL_ERROR "Emscripten inventory requires the production muhtari_libretro target")
endif()

if(DEFINED ENV{MUHTARI_EXPECTED_SHA} AND NOT "$ENV{MUHTARI_EXPECTED_SHA}" STREQUAL "")
  set(MUHTARI_EMSCRIPTEN_EXPECTED_SHA "$ENV{MUHTARI_EXPECTED_SHA}")
else()
  execute_process(COMMAND git -C "${PROJECT_SOURCE_DIR}" rev-parse --verify HEAD
    RESULT_VARIABLE git_rc OUTPUT_VARIABLE MUHTARI_EMSCRIPTEN_EXPECTED_SHA
    ERROR_VARIABLE git_error OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT git_rc STREQUAL "0")
    message(FATAL_ERROR "Emscripten package tests cannot resolve the checkout commit: ${git_error}")
  endif()
endif()
string(LENGTH "${MUHTARI_EMSCRIPTEN_EXPECTED_SHA}" commit_length)
if(NOT commit_length EQUAL 40 OR NOT MUHTARI_EMSCRIPTEN_EXPECTED_SHA MATCHES "^[0-9a-f]+$")
  message(FATAL_ERROR "Emscripten package tests require the full lowercase checkout commit")
endif()
find_program(MUHTARI_EMSCRIPTEN_EMCC emcc REQUIRED)
get_filename_component(MUHTARI_EMSCRIPTEN_SDK_ROOT "${MUHTARI_EMSCRIPTEN_EMCC}" DIRECTORY)
set(host "${PROJECT_SOURCE_DIR}/tests/emscripten/host.mjs")
muhtari_add_test(NAME emscripten.artifact
  COMMAND "${MUHTARI_EMSCRIPTEN_NODE}" "${host}"
    --module "${MUHTARI_EMSCRIPTEN_MODULE}"
    --fixture "${MUHTARI_EMSCRIPTEN_FIXTURE}"
    --native "${MUHTARI_EMSCRIPTEN_NATIVE_JSON}"
    --mode verify
  LABELS emscripten portable TIMEOUT 30)
muhtari_add_test(NAME emscripten.controls
  COMMAND "${MUHTARI_EMSCRIPTEN_NODE}" "${host}"
    --module "${MUHTARI_EMSCRIPTEN_MODULE}"
    --fixture "${MUHTARI_EMSCRIPTEN_FIXTURE}"
    --native "${MUHTARI_EMSCRIPTEN_NATIVE_JSON}"
    --mode controls
  LABELS emscripten portable TIMEOUT 30)

set(bundle_script "${PROJECT_SOURCE_DIR}/tests/emscripten/bundle.cmake")
set(bundle_archive "${PROJECT_BINARY_DIR}/muhtari-libretro-emscripten.zip")
muhtari_add_test(NAME emscripten.bundle
  COMMAND "${CMAKE_COMMAND}"
    "-DROOT=${PROJECT_SOURCE_DIR}"
    "-DBUILD=${PROJECT_BINARY_DIR}"
    "-DWORK=${PROJECT_BINARY_DIR}/bundle-create"
    "-DMODE=create"
    "-DEXPECTED_SHA=${MUHTARI_EMSCRIPTEN_EXPECTED_SHA}"
    "-DSDK_ROOT=${MUHTARI_EMSCRIPTEN_SDK_ROOT}"
    "-DNATIVE=${MUHTARI_EMSCRIPTEN_NATIVE_JSON}"
    "-DFIXTURE=${MUHTARI_EMSCRIPTEN_FIXTURE}"
    "-DNODE=${MUHTARI_EMSCRIPTEN_NODE}"
    -P "${bundle_script}"
  LABELS emscripten portable TIMEOUT 30)
muhtari_add_test(NAME emscripten.bundle.controls
  COMMAND "${CMAKE_COMMAND}"
    "-DROOT=${PROJECT_SOURCE_DIR}"
    "-DWORK=${PROJECT_BINARY_DIR}/bundle-controls"
    "-DMODE=controls"
    "-DBUILD=${PROJECT_BINARY_DIR}"
    "-DEXPECTED_SHA=${MUHTARI_EMSCRIPTEN_EXPECTED_SHA}"
    "-DARCHIVE=${bundle_archive}"
    "-DNATIVE=${MUHTARI_EMSCRIPTEN_NATIVE_JSON}"
    "-DFIXTURE=${MUHTARI_EMSCRIPTEN_FIXTURE}"
    "-DNODE=${MUHTARI_EMSCRIPTEN_NODE}"
    -P "${bundle_script}"
  LABELS emscripten portable TIMEOUT 30)
set_tests_properties(emscripten.bundle.controls PROPERTIES DEPENDS emscripten.bundle)

foreach(mode IN ITEMS verify controls)
  set(name emscripten.ym)
  if(mode STREQUAL controls)
    string(APPEND name .controls)
  endif()
  muhtari_add_test(NAME ${name}
    COMMAND "${CMAKE_COMMAND}"
      "-DNODE=${MUHTARI_EMSCRIPTEN_NODE}"
      "-DHOST=${host}"
      "-DMODULE=${MUHTARI_EMSCRIPTEN_MODULE}"
      "-DFIXTURES=${PROJECT_SOURCE_DIR}/build/ci/tests/ym2151"
      "-DNATIVE_DIR=${PROJECT_SOURCE_DIR}/build/ci/tests/emscripten"
      "-DMODE=${mode}"
      -P "${PROJECT_SOURCE_DIR}/tests/emscripten/ym.cmake"
    LABELS emscripten portable TIMEOUT 30)
endforeach()

muhtari_add_test(NAME emscripten.souper
  COMMAND "${CMAKE_COMMAND}"
    "-DNODE=${MUHTARI_EMSCRIPTEN_NODE}"
    "-DHOST=${host}"
    "-DMODULE=${MUHTARI_EMSCRIPTEN_MODULE}"
    "-DNATIVE=${MUHTARI_EMSCRIPTEN_SOUPER_NATIVE_JSON}"
    "-DFIXTURE=${MUHTARI_EMSCRIPTEN_SOUPER_FIXTURE}"
    -P "${PROJECT_SOURCE_DIR}/tests/emscripten/souper.cmake"
  LABELS emscripten portable TIMEOUT 30)
