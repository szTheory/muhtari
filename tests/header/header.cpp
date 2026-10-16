// The public header alone, compiled as C++17 with warnings fatal. Compiling
// this file is the test; nothing links it.
#include "muhtari.h"

// Every boundary struct starts with its size tag.
static_assert(offsetof(muhtari_versions, size) == 0, "versions size first");
static_assert(offsetof(muhtari_config, size) == 0, "config size first");
static_assert(offsetof(muhtari_info, size) == 0, "info size first");
static_assert(offsetof(muhtari_frame, size) == 0, "frame size first");
static_assert(sizeof(muhtari_status) == 4, "status is 32 bits");
static_assert(MUHTARI_API_VERSION == 1, "API version");

// Each function keeps its declared signature, with C linkage.
static muhtari_status (*const get_versions_fn)(muhtari_versions *) = muhtari_get_versions;
static muhtari_status (*const create_fn)(const muhtari_config *, muhtari **) = muhtari_create;
static void (*const destroy_fn)(muhtari *) = muhtari_destroy;
static muhtari_status (*const get_info_fn)(const muhtari *, muhtari_info *) = muhtari_get_info;
static muhtari_status (*const run_frame_fn)(muhtari *, muhtari_frame *) = muhtari_run_frame;

int header_check(muhtari_status st);

int header_check(muhtari_status st)
{
    // A C++ caller zeroes a struct with empty braces, then sets its size.
    muhtari_frame io = {};
    io.size = static_cast<uint32_t>(sizeof io);
    (void)get_versions_fn;
    (void)create_fn;
    (void)destroy_fn;
    (void)get_info_fn;
    (void)run_frame_fn;
    // A status compares against the constants without a sign or enum warning.
    return (st == MUHTARI_STATUS_OK || st == MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL) &&
           io.audio_count == 0u;
}
