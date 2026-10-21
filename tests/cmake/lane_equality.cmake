# Compare the two runtime hash artifacts using the existing byte comparator.
set(FILES "${NATIVE}/hashes.txt" "${BE}/hashes.txt")
set(COUNT 2)
set(MODE compare)
include("${CMAKE_CURRENT_LIST_DIR}/hashes.cmake")
