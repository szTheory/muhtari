/* API version 3: what it adds to version 2, loading and power cycling.
   FROZEN once released, as v1.c and v2.c are: it calls every new function,
   assigns every field of the new structs and checks every new constant, so a
   rename, a removal or a changed value fails to build or fails here. */
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "../check.h"

/* A clean 4 KiB linear image: an A78 version 3 header over a body of NOPs
   whose reset vector points at its start, $F000. */
static uint8_t image[128 + 4096];

static void *api_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void api_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

int main(void)
{
    CHECK_EQ_U64(MUHTARI_API_VERSION, 3);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_BIOS_REJECTED_CART, 10);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_INVALID_BIOS, 11);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_REGION, 12);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, 13);
    CHECK_EQ_U64(MUHTARI_PORT_UNPLUGGED, 256);

    image[0] = 3u;
    memcpy(image + 1, "ATARI7800", 9);
    image[51] = 0x10u; /* size field 4096 */
    image[55] = 1u;
    image[56] = 1u;
    memset(image + 128, 0xEA, 4096);
    image[128 + 4096 - 4] = 0x00u;
    image[128 + 4096 - 3] = 0xF0u;

    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = api_alloc;
    cfg.allocator.free = api_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);

    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.region = 0u;
    media.image = image;
    media.image_size = sizeof image;
    media.bios = NULL;
    media.bios_size = 0u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    media.region = MUHTARI_REGION_PAL;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_REGION);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_power_cycle(NULL), MUHTARI_STATUS_ERR_ARGUMENT);

    muhtari_destroy(m);
    CHECK_DONE();
}
