/* API version 3: what it adds to version 2, loading, power cycling, input,
   memory spans and counters.
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
    CHECK_EQ_U64(MUHTARI_BUTTON_UP, 1);
    CHECK_EQ_U64(MUHTARI_BUTTON_DOWN, 2);
    CHECK_EQ_U64(MUHTARI_BUTTON_LEFT, 4);
    CHECK_EQ_U64(MUHTARI_BUTTON_RIGHT, 8);
    CHECK_EQ_U64(MUHTARI_BUTTON_1, 16);
    CHECK_EQ_U64(MUHTARI_BUTTON_2, 32);
    CHECK_EQ_U64(MUHTARI_SWITCH_RESET, 1);
    CHECK_EQ_U64(MUHTARI_SWITCH_SELECT, 2);
    CHECK_EQ_U64(MUHTARI_SWITCH_PAUSE, 4);
    CHECK_EQ_U64(MUHTARI_SWITCH_LEFT_DIFFICULTY_A, 8);
    CHECK_EQ_U64(MUHTARI_SWITCH_RIGHT_DIFFICULTY_A, 16);
    CHECK_EQ_U64(MUHTARI_MEMORY_SYSTEM_RAM, 1);
    CHECK_EQ_U64(MUHTARI_MEMORY_RIOT_RAM, 2);
    CHECK_EQ_U64(MUHTARI_MEMORY_CART_RAM, 3);
    CHECK_EQ_U64(sizeof(muhtari_input), 24);
    CHECK_EQ_U64(sizeof(muhtari_counters), 24);

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

    muhtari_input in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.device[0] = MUHTARI_PORT_PROLINE;
    in.device[1] = MUHTARI_PORT_JOYSTICK;
    in.buttons[0] = MUHTARI_BUTTON_UP | MUHTARI_BUTTON_1;
    in.buttons[1] = 0u;
    in.switches = MUHTARI_SWITCH_PAUSE;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_OK);
    in.device[1] = MUHTARI_PORT_PADDLE;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);

    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    span.data = NULL;
    span.bytes = 0u;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &span), MUHTARI_STATUS_OK);
    CHECK(span.data != NULL);
    CHECK_EQ_U64(span.bytes, 4096);

    muhtari_counters counters;
    memset(&counters, 0, sizeof counters);
    counters.size = (uint32_t)sizeof counters;
    counters.frames = 1u;
    counters.lag_frames = 1u;
    CHECK_EQ_U64(muhtari_get_counters(m, &counters), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(counters.frames, 0);
    CHECK_EQ_U64(counters.lag_frames, 0);

    muhtari_destroy(m);
    CHECK_DONE();
}
