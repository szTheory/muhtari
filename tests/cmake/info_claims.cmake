# libretro .info claims check, run as
#   cmake -DINFO=<file.info> -DINVENTORY=<inventory.txt> -P info_claims.cmake
# Fault prevented: the .info promises a capability no test exercises.
# RetroArch turns features on from these keys, so every key set to "true"
# must map to a test, and that test must be in the inventory. A description
# over 512 characters, the core-info template's limit, fails too.
foreach(var INFO INVENTORY)
  if(NOT DEFINED ${var} OR NOT EXISTS "${${var}}")
    message(FATAL_ERROR "info_claims: -D${var} must name an existing file")
  endif()
endforeach()

# Claim to the test that proves it. is_experimental is a warning label, not
# a capability, so it needs no test.
set(claim_supports_no_game libretro.host.xrgb8888)
set(claim_memory_descriptors libretro.host.memory_map)
set(claim_input_descriptors libretro.host.devices)
set(claim_core_options libretro.host.options)
set(claim_firmware0_opt libretro.host.bios)
set(claim_firmware1_opt libretro.host.bios)
set(claim_firmware2_opt libretro.host.hsc_firmware)
set(claim_libretro_saves libretro.host.battery)
set(claim_savestate libretro.host.state)
set(exempt is_experimental)

file(STRINGS "${INVENTORY}" inventory)
file(STRINGS "${INFO}" lines REGEX "^[a-z_0-9]+ = \"true\"$")
foreach(line IN LISTS lines)
  string(REGEX REPLACE " = .*" "" key "${line}")
  if(key IN_LIST exempt)
    continue()
  endif()
  if(NOT DEFINED claim_${key})
    message(FATAL_ERROR "info_claims: ${key} = \"true\" maps to no test")
  endif()
  string(REPLACE "." "\\." name "${claim_${key}}")
  set(found ${inventory})
  list(FILTER found INCLUDE REGEX "^${name}\t")
  if(NOT found)
    message(FATAL_ERROR "info_claims: ${key} needs ${claim_${key}}, which is not in ${INVENTORY}")
  endif()
endforeach()

# RetroArch accepts this non-boolean feature level as a promise about saved
# state continuation. Keep it tied to the loaded-core equality proof, repeated
# continuation property, and this phase's observed host-control sequence.
file(STRINGS "${INFO}" feature_lines REGEX "^[ \t]*savestate_features[ \t]*=")
list(LENGTH feature_lines feature_count)
if(feature_count GREATER 1)
  message(FATAL_ERROR "info_claims: savestate_features must appear exactly once")
elseif(feature_count EQUAL 1)
  list(GET feature_lines 0 feature_line)
  if(NOT feature_line STREQUAL "savestate_features = \"deterministic\"")
    message(FATAL_ERROR "info_claims: savestate_features must be \"deterministic\"")
  endif()
  foreach(test IN ITEMS libretro.host.state prop.state_continuation retroarch.states.controls)
    string(REPLACE "." "\\." name "${test}")
    set(found ${inventory})
    list(FILTER found INCLUDE REGEX "^${name}\t")
    if(NOT found)
      message(FATAL_ERROR "info_claims: savestate_features needs ${test}, which is not in ${INVENTORY}")
    endif()
  endforeach()
endif()

# A file without a description line is not this check's concern.
file(READ "${INFO}" info_text)
if(info_text MATCHES "(^|\n)description = \"([^\"\n]*)\"")
  string(LENGTH "${CMAKE_MATCH_2}" description_length)
  if(description_length GREATER 512)
    message(FATAL_ERROR "info_claims: description is ${description_length} characters, over 512")
  endif()
endif()
list(LENGTH lines count)
message(STATUS "info_claims: ${count} \"true\" line(s), each backed by a test or exempt")
