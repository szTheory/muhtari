# Trace-only test targets reference the dual-compiled objects omitted by this
# product configuration. Reject that incompatible combination before test
# registration so users get a clear option error instead of a link failure.
if(NOT MUHTARI_TRACE_LOOP AND MUHTARI_TESTS)
  message(FATAL_ERROR
    "MUHTARI_TRACE_LOOP=OFF requires MUHTARI_TESTS=OFF because trace-specific test targets need the omitted trace objects")
endif()
