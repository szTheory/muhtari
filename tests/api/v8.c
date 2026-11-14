/* API 8's state format version and additive size-tagged version prefix. */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "muhtari.h"
#include "../alloc.h"
#include "../check.h"

struct versions_v7 {
    uint32_t size;
    uint32_t library_major;
    uint32_t library_minor;
    uint32_t library_patch;
    uint32_t api;
    uint32_t abi;
    uint32_t behaviour_revision;
};

static void api_surface(void)
{
    CHECK_EQ_U64(MUHTARI_API_VERSION, 8u);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_STATE_UNAVAILABLE, 15u);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_INVALID_STATE, 16u);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_STATE, 17u);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_STATE_MISMATCH, 18u);
    CHECK_EQ_U64(offsetof(muhtari_versions, state_format), sizeof(struct versions_v7));
    CHECK_EQ_U64(sizeof(muhtari_versions), sizeof(struct versions_v7) + sizeof(uint32_t));

    muhtari_versions versions;
    memset(&versions, 0, sizeof versions);
    versions.size = (uint32_t)sizeof versions;
    CHECK_EQ_U64(muhtari_get_versions(&versions), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(versions.api, 8u);
    CHECK_EQ_U64(versions.abi, 1u);
    CHECK_EQ_U64(versions.behaviour_revision, 8u);
    CHECK_EQ_U64(versions.state_format, 3u);

    struct {
        struct versions_v7 old;
        uint8_t tail[8];
    } older;
    memset(&older, 0xA5, sizeof older);
    older.old.size = (uint32_t)sizeof older.old;
    CHECK_EQ_U64(muhtari_get_versions((muhtari_versions *)(void *)&older.old), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(older.old.api, 8u);
    CHECK_EQ_U64(older.old.abi, 1u);
    CHECK_EQ_U64(older.old.behaviour_revision, 8u);
    for (size_t i = 0u; i < sizeof older.tail; i++)
        CHECK_EQ_U64(older.tail[i], 0xA5u);
}

static void state_surface(void)
{
    alloc_counts counts;
    muhtari_config config = alloc_config(&counts);
    muhtari *m = NULL;
    uint8_t bytes[16];
    memset(bytes, 0xA5, sizeof bytes);
    CHECK_EQ_U64(muhtari_create(&config, &m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_state_size(NULL), 0u);
    CHECK_EQ_U64(muhtari_state_size(m), 0u);
    CHECK_EQ_U64(muhtari_save_state(NULL, bytes, sizeof bytes), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(NULL, bytes, sizeof bytes), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, NULL, sizeof bytes), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(m, NULL, sizeof bytes), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, bytes, sizeof bytes), MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    CHECK_EQ_U64(muhtari_load_state(m, bytes, sizeof bytes), MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    for (size_t i = 0u; i < sizeof bytes; i++)
        CHECK_EQ_U64(bytes[i], 0xA5u);
    muhtari_destroy(m);
    CHECK_EQ_U64(counts.live_bytes, 0u);
}

int main(void)
{
    api_surface();
    state_surface();
    CHECK_DONE();
}
