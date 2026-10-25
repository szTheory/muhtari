/* API 6: separate extended input, with all additions exercised. */
#include <stdlib.h>
#include <string.h>
#include "muhtari.h"
#include "../check.h"

static void *api_alloc(void *user, size_t n)
{
    (void)user;
    return malloc(n);
}
static void api_free(void *user, void *p, size_t n)
{
    (void)user;
    (void)n;
    free(p);
}

int main(void)
{
    CHECK(MUHTARI_API_VERSION >= 6);
    CHECK_EQ_U64(MUHTARI_INPUT_GUN_OFFSCREEN, 1u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_Y, 64u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_X, 128u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_L, 256u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_R, 512u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_SELECT, 1024u);
    CHECK_EQ_U64(MUHTARI_SNES_BUTTON_START, 2048u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_C, 64u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_X, 128u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_Y, 256u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_Z, 512u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_MODE, 1024u);
    CHECK_EQ_U64(MUHTARI_MEGA_BUTTON_START, 2048u);
    CHECK_EQ_U64(sizeof(muhtari_input), 24u);
    CHECK_EQ_U64(sizeof(muhtari_port_input), 40u);
    CHECK_EQ_U64(sizeof(muhtari_input_ex), 88u);
    CHECK_EQ_U64(offsetof(muhtari_input_ex, size), 0u);
    CHECK_EQ_U64(offsetof(muhtari_input_ex, port), 4u);
    CHECK_EQ_U64(offsetof(muhtari_input_ex, switches), 84u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, device), 0u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, buttons), 4u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, paddle), 8u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, keypad), 16u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, dx), 20u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, dy), 24u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, gun_x), 28u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, gun_y), 32u);
    CHECK_EQ_U64(offsetof(muhtari_port_input, flags), 36u);
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = api_alloc;
    cfg.allocator.free = api_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    muhtari_versions versions;
    memset(&versions, 0, sizeof versions);
    versions.size = (uint32_t)sizeof versions;
    CHECK_EQ_U64(muhtari_get_versions(&versions), MUHTARI_STATUS_OK);
    CHECK(versions.api >= 6u);
    CHECK_EQ_U64(versions.abi, 1u);
    muhtari_input_ex input;
    memset(&input, 0, sizeof input);
    input.size = (uint32_t)sizeof input;
    input.port[0].device = MUHTARI_PORT_PROLINE;
    input.port[0].buttons = MUHTARI_BUTTON_UP | MUHTARI_BUTTON_1;
    input.port[1].device = MUHTARI_PORT_UNPLUGGED;
    input.switches = MUHTARI_SWITCH_RESET;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &input), MUHTARI_STATUS_OK);
    input.port[1].device = MUHTARI_PORT_KEYPAD;
    input.port[1].paddle[0] = 65535u;
    input.port[1].paddle[1] = 0u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &input), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    input.port[1].device = MUHTARI_PORT_PROLINE;
    input.port[1].keypad = 1u;
    input.port[1].dx = -1;
    input.port[1].dy = 1;
    input.port[1].gun_x = 319u;
    input.port[1].gun_y = 241u;
    input.port[1].flags = MUHTARI_INPUT_GUN_OFFSCREEN;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &input), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_set_input_ex(NULL, &input), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_set_input_ex(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    muhtari_destroy(m);
    CHECK_DONE();
}
