# Hygiene tests, registered only by the hygiene preset (MUHTARI_HYGIENE).

# No personal data, unlisted binary, game image or GPL text in any file git
# would track.
muhtari_add_test(NAME hygiene.tree
  COMMAND ${MUHTARI_SH} ${CMAKE_SOURCE_DIR}/scripts/hygiene.sh --tree
  LABELS hygiene TIMEOUT 60)
