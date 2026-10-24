/* The public header alone, compiled as C17 with warnings fatal. Compiling
   this file is the test; nothing links it. */
#include "muhtari.h"

/* Every boundary struct starts with its size tag. */
_Static_assert(offsetof(muhtari_versions, size) == 0, "versions size first");
_Static_assert(offsetof(muhtari_config, size) == 0, "config size first");
_Static_assert(offsetof(muhtari_info, size) == 0, "info size first");
_Static_assert(offsetof(muhtari_frame, size) == 0, "frame size first");
_Static_assert(offsetof(muhtari_cart_info, size) == 0, "cart_info size first");
_Static_assert(sizeof(muhtari_status) == 4, "status is 32 bits");
_Static_assert(offsetof(muhtari_media, size) == 0, "media size first");
_Static_assert(offsetof(muhtari_input, size) == 0, "input size first");
_Static_assert(offsetof(muhtari_span, size) == 0, "span size first");
_Static_assert(offsetof(muhtari_counters, size) == 0, "counters size first");
_Static_assert(MUHTARI_API_VERSION == 5, "API version");

/* Each function keeps its declared signature. */
static muhtari_status (*const get_versions_fn)(muhtari_versions *) = muhtari_get_versions;
static muhtari_status (*const create_fn)(const muhtari_config *, muhtari **) = muhtari_create;
static void (*const destroy_fn)(muhtari *) = muhtari_destroy;
static muhtari_status (*const get_info_fn)(const muhtari *, muhtari_info *) = muhtari_get_info;
static muhtari_status (*const run_frame_fn)(muhtari *, muhtari_frame *) = muhtari_run_frame;
static muhtari_status (*const probe_fn)(const void *, size_t, muhtari_cart_info *) = muhtari_probe;
static muhtari_status (*const load_fn)(muhtari *, const muhtari_media *) = muhtari_load;
static muhtari_status (*const power_cycle_fn)(muhtari *) = muhtari_power_cycle;
static muhtari_status (*const set_input_fn)(muhtari *, const muhtari_input *) = muhtari_set_input;
static muhtari_status (*const get_memory_fn)(muhtari *, uint32_t,
                                             muhtari_span *) = muhtari_get_memory;
static muhtari_status (*const get_counters_fn)(const muhtari *,
                                               muhtari_counters *) = muhtari_get_counters;

int header_check(muhtari_status st);

int header_check(muhtari_status st)
{
    (void)get_versions_fn;
    (void)create_fn;
    (void)destroy_fn;
    (void)get_info_fn;
    (void)run_frame_fn;
    (void)probe_fn;
    (void)load_fn;
    (void)power_cycle_fn;
    (void)set_input_fn;
    (void)get_memory_fn;
    (void)get_counters_fn;
    /* A status compares against the constants without a sign or enum warning. */
    return st == MUHTARI_STATUS_OK || st == MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL ||
           st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
}
