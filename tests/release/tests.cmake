# Release tests, label release: they read the archives the package step
# writes, so the ci workflows run them in a second test step after packaging
# and the first test step excludes them.
set(release_scripts ${CMAKE_CURRENT_LIST_DIR}/../cmake)
set(release_suffix ${MUHTARI_OS}-${MUHTARI_ARCH})

muhtari_add_test(NAME release.archive_contents
  COMMAND ${CMAKE_COMMAND} -DDIR=${PROJECT_BINARY_DIR} -DSUFFIX=${release_suffix}
    -DSOURCE_DIR=${PROJECT_SOURCE_DIR} -DBINARY_DIR=${PROJECT_BINARY_DIR}
    -P ${release_scripts}/archive_contents.cmake
  LABELS release TIMEOUT 60)

# The self test checks a libretro zip built without THIRD-PARTY-NOTICES.md
# and expects that file named.
set(release_fixture ${PROJECT_BINARY_DIR}/release/self_test)
set(release_core muhtari_libretro.so)
if(MUHTARI_OS STREQUAL "macos")
  set(release_core muhtari_libretro.dylib)
elseif(MUHTARI_OS STREQUAL "windows")
  set(release_core muhtari_libretro.dll)
endif()
file(REMOVE_RECURSE ${release_fixture})
file(COPY ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION ${release_fixture}/stage)
file(WRITE ${release_fixture}/stage/cores/${release_core} "core\n")
file(WRITE ${release_fixture}/stage/info/muhtari_libretro.info "info\n")
file(MAKE_DIRECTORY ${release_fixture}/zips)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar cf
    ${release_fixture}/zips/muhtari-libretro-${release_suffix}.zip --format=zip
    LICENSE cores info
  WORKING_DIRECTORY ${release_fixture}/stage
  COMMAND_ERROR_IS_FATAL ANY)
set(release_cmd ${CMAKE_COMMAND} -DDIR=${release_fixture}/zips -DSUFFIX=${release_suffix}
  -DSOURCE_DIR=${PROJECT_SOURCE_DIR} -DBINARY_DIR=${release_fixture} -DCOMPONENTS=libretro
  -P ${release_scripts}/archive_contents.cmake)
string(REPLACE ";" "$<SEMICOLON>" release_cmd "${release_cmd}")
muhtari_add_test(NAME release.archive_contents.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${release_cmd}" -DCODE=1
    "-DREGEX=lacks: THIRD-PARTY-NOTICES.md" -P ${release_scripts}/expect_fail.cmake
  LABELS release TIMEOUT 10)

# The floors each binary reads as, from its own extraction of the zips. The
# self test passes an impossible floor for this leg's system and expects it
# named.
set(release_floor_cmd ${CMAKE_COMMAND} -DDIR=${PROJECT_BINARY_DIR} -DSUFFIX=${release_suffix})
muhtari_add_test(NAME release.floor
  COMMAND ${release_floor_cmd} -DWORK=${PROJECT_BINARY_DIR}/release/floor
    -P ${release_scripts}/floor.cmake
  LABELS release TIMEOUT 60)
set(release_floor 9.0)
set(release_regex "minimum OS [0-9.]+ is above the floor 9.0")
if(MUHTARI_OS STREQUAL "linux")
  set(release_floor 2.0)
  set(release_regex "GLIBC_[0-9.]+ is above the floor 2.0")
elseif(MUHTARI_OS STREQUAL "windows")
  set(release_floor "")
  set(release_regex "imports KERNEL32.dll, outside the allowlist")
endif()
set(release_cmd ${release_floor_cmd} -DWORK=${PROJECT_BINARY_DIR}/release/floor_self_test
  -DFLOOR_OVERRIDE=${release_floor} -P ${release_scripts}/floor.cmake)
string(REPLACE ";" "$<SEMICOLON>" release_cmd "${release_cmd}")
muhtari_add_test(NAME release.floor.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${release_cmd}" -DCODE=1
    "-DREGEX=${release_regex}" -P ${release_scripts}/expect_fail.cmake
  LABELS release TIMEOUT 60)

# No member of this leg's zips is firmware, a built sample or a known ROM.
# The self test zips a synthetic 256-byte A78 file and lists the SHA-1 of its
# last 128 bytes, which must be found once the header is stripped.
set(release_scan_lists -DEXPECTED=${PROJECT_SOURCE_DIR}/tests/built/expected.txt
  -DCARTDB=${PROJECT_SOURCE_DIR}/src/cartdb.c)
muhtari_add_test(NAME release.firmware_scan
  COMMAND ${CMAKE_COMMAND} "-DZIPS=${PROJECT_BINARY_DIR}/muhtari-*-${release_suffix}.zip"
    -DFIRMWARE=${PROJECT_SOURCE_DIR}/tests/release/firmware.txt ${release_scan_lists}
    -P ${release_scripts}/firmware_scan.cmake
  LABELS release TIMEOUT 60)  # provisional
set(release_scan ${PROJECT_BINARY_DIR}/release/firmware_scan_self_test)
file(REMOVE_RECURSE ${release_scan})
string(REPEAT "h" 118 release_pad)
string(REPEAT "synthetic payload, not a ROM. " 5 release_body)
string(SUBSTRING "${release_body}" 0 128 release_body)
file(WRITE ${release_scan}/stage/synthetic.a78 "xATARI7800${release_pad}${release_body}")
string(SHA1 release_sha1 "${release_body}")
file(WRITE ${release_scan}/firmware.txt "sha1 ${release_sha1} synthetic\n")
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar cf ${release_scan}/synthetic.zip --format=zip synthetic.a78
  WORKING_DIRECTORY ${release_scan}/stage
  COMMAND_ERROR_IS_FATAL ANY)
set(release_cmd ${CMAKE_COMMAND} -DZIPS=${release_scan}/synthetic.zip ${release_scan_lists}
  -DFIRMWARE=${release_scan}/firmware.txt -P ${release_scripts}/firmware_scan.cmake)
string(REPLACE ";" "$<SEMICOLON>" release_cmd "${release_cmd}")
muhtari_add_test(NAME release.firmware_scan.self_test
  COMMAND ${CMAKE_COMMAND} "-DCMD=${release_cmd}" -DCODE=1
    "-DREGEX=synthetic.zip/synthetic.a78: matches synthetic \\(header stripped\\)"
    -P ${release_scripts}/expect_fail.cmake
  LABELS release TIMEOUT 30)  # provisional
