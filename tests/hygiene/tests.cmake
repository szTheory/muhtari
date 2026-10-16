# Hygiene tests, registered only by the hygiene preset (MUHTARI_HYGIENE).

# No personal data, unlisted binary, game image or GPL text in any file git
# would track.
muhtari_add_test(NAME hygiene.tree
  COMMAND ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/hygiene.sh --tree
  LABELS hygiene TIMEOUT 60)

# Every tracked C file but the vendored libretro.h is formatted as
# .clang-format says.
muhtari_add_test(NAME hygiene.format
  COMMAND ${MUHTARI_SH} -c "cd \"$1\" && ${GIT_EXECUTABLE} ls-files -z '*.c' '*.h' \
':!:libretro/libretro.h' | xargs -0 \"$2\" --dry-run --Werror" sh
    ${CMAKE_SOURCE_DIR} ${MUHTARI_CLANG_FORMAT}
  LABELS hygiene TIMEOUT 60)

# No floating point in the core's text. The self test scans a fixture with
# six float lines beside two lines that only look like them.
set(hygiene_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)
muhtari_add_test(NAME hygiene.no_float
  COMMAND ${CMAKE_COMMAND} "-DROOTS=${CMAKE_SOURCE_DIR}/src$<SEMICOLON>${CMAKE_SOURCE_DIR}/include"
    -P ${hygiene_scripts}/no_float.cmake
  LABELS hygiene TIMEOUT 30)
set(hygiene_fixture ${CMAKE_BINARY_DIR}/hygiene/no_float_fixture)
file(WRITE ${hygiene_fixture}/sample.c "int y = 0x1E5;\nint double_free = 2;\nx = 1.5;\n"
  "z = 0x1p3;\nw = 1e3;\ndouble d;\n#include <math.h>\n#include <float.h>\n")
set(hygiene_cmd ${CMAKE_COMMAND} -DROOTS=${hygiene_fixture} -P ${hygiene_scripts}/no_float.cmake)
string(REPLACE ";" "$<SEMICOLON>" hygiene_cmd "${hygiene_cmd}")
muhtari_add_test(NAME hygiene.no_float.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${hygiene_cmd}" -DCODE=1
    "-DREGEX=sample.c:3: x = 1.5.*sample.c:8: #include <float.h>.*no_float: 6 lines"
    -P ${hygiene_scripts}/expect_fail.cmake
  LABELS hygiene TIMEOUT 30)

# The clang static analyzer finds nothing in the core.
muhtari_add_test(NAME hygiene.analyzer
  COMMAND ${MUHTARI_SH} -c "cd \"$1\" && \"$2\" -p \"$3\" --quiet \
-checks='-*,clang-analyzer-*' --warnings-as-errors='*' src/*.c" sh
    ${CMAKE_SOURCE_DIR} ${MUHTARI_CLANG_TIDY} ${CMAKE_BINARY_DIR}
  LABELS hygiene TIMEOUT 120)
