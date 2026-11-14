cmake_minimum_required(VERSION 3.25)

foreach(required OBSERVER FIXTURE OUT EMULATOR SOURCE_ID_FILE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "souper observer: -D${required} is required")
  endif()
endforeach()
file(MAKE_DIRECTORY "${OUT}")
set(receipt "${OUT}/souper-native.json")
execute_process(COMMAND ${EMULATOR} "${OBSERVER}" --souper-fixture "${FIXTURE}" --output "${receipt}"
  RESULT_VARIABLE rc OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 30)
if(NOT rc STREQUAL "0")
  message(FATAL_ERROR "Souper native libretro observer: ${rc}: ${output}${error}")
endif()
file(READ "${receipt}" json)
file(SHA256 "${FIXTURE}" expected_fixture_hash)
file(READ "${SOURCE_ID_FILE}" expected_source_identity)
string(STRIP "${expected_source_identity}" expected_source_identity)
string(JSON schema ERROR_VARIABLE schema_error GET "${json}" schema)
string(JSON workload ERROR_VARIABLE workload_error GET "${json}" workload)
string(JSON fixture_hash ERROR_VARIABLE fixture_error GET "${json}" fixture_sha256)
string(JSON source_identity ERROR_VARIABLE source_error GET "${json}" source_identity)
string(JSON width ERROR_VARIABLE width_error GET "${json}" numeric_oracles width)
string(JSON height ERROR_VARIABLE height_error GET "${json}" numeric_oracles height)
string(JSON state_format ERROR_VARIABLE state_error GET "${json}" state format)
string(JSON state_behaviour ERROR_VARIABLE state_behaviour_error GET "${json}" state behaviour)
string(JSON state_bytes ERROR_VARIABLE state_bytes_error GET "${json}" state bytes)
string(JSON library_state_hash ERROR_VARIABLE library_state_error GET "${json}" state library_sha256)
string(JSON adapter_state_hash ERROR_VARIABLE adapter_state_error GET "${json}" state adapter_sha256)
string(JSON library_continuation_hash ERROR_VARIABLE library_continuation_error GET "${json}" state library_continuation_sha256)
string(JSON adapter_continuation_hash ERROR_VARIABLE adapter_continuation_error GET "${json}" state adapter_continuation_sha256)
string(JSON replay_count ERROR_VARIABLE replay_count_error GET "${json}" state replays)
string(JSON refused_states ERROR_VARIABLE refused_states_error GET "${json}" state refused_null_short_long_corrupt)
string(JSON preserved_refusal ERROR_VARIABLE preserved_refusal_error GET "${json}" state preserved_after_refusal)
string(JSON failed_reload_unloaded ERROR_VARIABLE failed_reload_error GET "${json}" state malformed_reload_unloaded)
string(JSON frame_count ERROR_VARIABLE frames_error LENGTH "${json}" frames)
string(LENGTH "${fixture_hash}" fixture_hash_length)
string(LENGTH "${source_identity}" source_identity_length)
if(schema_error OR workload_error OR fixture_error OR source_error OR width_error OR height_error OR
    state_error OR state_behaviour_error OR state_bytes_error OR library_state_error OR adapter_state_error OR
    library_continuation_error OR adapter_continuation_error OR replay_count_error OR refused_states_error OR
    preserved_refusal_error OR failed_reload_error OR
    frames_error OR NOT schema EQUAL 4 OR NOT workload STREQUAL "souper" OR
    NOT fixture_hash MATCHES "^[0-9a-f]+$" OR NOT fixture_hash_length EQUAL 64 OR
    NOT fixture_hash STREQUAL expected_fixture_hash OR
    NOT source_identity MATCHES "^[0-9a-f]+$" OR NOT source_identity_length EQUAL 64 OR
    NOT source_identity STREQUAL expected_source_identity OR
    NOT width EQUAL 320 OR NOT height EQUAL 242 OR NOT state_format EQUAL 3 OR
    NOT state_behaviour EQUAL 8 OR NOT state_bytes GREATER 32768 OR
    NOT library_state_hash MATCHES "^[0-9a-f]+$" OR NOT adapter_state_hash MATCHES "^[0-9a-f]+$" OR
    NOT library_continuation_hash MATCHES "^[0-9a-f]+$" OR
    NOT adapter_continuation_hash MATCHES "^[0-9a-f]+$" OR
    NOT replay_count EQUAL 2 OR
    NOT refused_states OR NOT preserved_refusal OR NOT failed_reload_unloaded OR
    NOT frame_count EQUAL 12)
  message(FATAL_ERROR "Souper native receipt is malformed or has the wrong source/image identity")
endif()
string(JSON mailbox_count ERROR_VARIABLE mailbox_error LENGTH "${json}" numeric_oracles mailbox_values)
if(mailbox_error OR NOT mailbox_count EQUAL 20)
  message(FATAL_ERROR "Souper native receipt lacks the independent mailbox oracle")
endif()
set(expected_mailbox_values 0 0 64 64 224 160 161 162 162 163 163 166 166 167 167 160 161 94 94 224)
foreach(i RANGE 0 19)
  string(JSON mailbox_value ERROR_VARIABLE mailbox_value_error GET "${json}" numeric_oracles mailbox_values ${i})
  list(GET expected_mailbox_values ${i} expected_mailbox_value)
  if(mailbox_value_error OR NOT mailbox_value EQUAL expected_mailbox_value)
    message(FATAL_ERROR "Souper native mailbox oracle value ${i} changed")
  endif()
endforeach()
foreach(i RANGE 0 11)
  string(JSON index ERROR_VARIABLE index_error GET "${json}" frames ${i} index)
  string(JSON audio_count ERROR_VARIABLE count_error GET "${json}" frames ${i} audio_count)
  string(JSON video ERROR_VARIABLE video_error GET "${json}" frames ${i} video_sha256)
  string(JSON audio ERROR_VARIABLE audio_error GET "${json}" frames ${i} audio_sha256)
  string(LENGTH "${video}" video_length)
  string(LENGTH "${audio}" audio_length)
  if(index_error OR count_error OR video_error OR audio_error OR NOT index EQUAL i OR
      NOT audio_count MATCHES "^[0-9]+$" OR NOT video MATCHES "^[0-9a-f]+$" OR
      NOT audio MATCHES "^[0-9a-f]+$" OR NOT video_length EQUAL 64 OR NOT audio_length EQUAL 64)
    message(FATAL_ERROR "Souper native frame receipt ${i} is incomplete")
  endif()
endforeach()
message(STATUS "Souper native receipt: public library and loaded libretro module replayed 12 judged frames twice; malformed states preserved both consumers")
