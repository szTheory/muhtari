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
foreach(required IN ITEMS
    "${MUHTARI_EMSCRIPTEN_NATIVE_JSON}" "${MUHTARI_EMSCRIPTEN_FIXTURE}")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "required native Emscripten evidence is missing: ${required}")
  endif()
endforeach()

if(NOT TARGET muhtari_libretro)
  message(FATAL_ERROR "Emscripten inventory requires the production muhtari_libretro target")
endif()
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
