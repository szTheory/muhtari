/* Frozen API-6 packet declarations, independent of the current header. */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../check.h"

typedef uint32_t muhtari_status;
typedef struct muhtari muhtari;
typedef struct muhtari_allocator {
    void *(*alloc)(void *user, size_t size);
    void (*free)(void *user, void *ptr, size_t size);
    void *user;
} muhtari_allocator;
typedef struct muhtari_config {
    uint32_t size;
    muhtari_allocator allocator;
} muhtari_config;
typedef struct muhtari_input {
    uint32_t size, device[2], buttons[2], switches;
} muhtari_input;
typedef struct muhtari_port_input {
    uint32_t device, buttons, paddle[2], keypad;
    int32_t dx, dy;
    uint32_t gun_x, gun_y, flags;
} muhtari_port_input;
typedef struct muhtari_input_ex {
    uint32_t size;
    muhtari_port_input port[2];
    uint32_t switches;
} muhtari_input_ex;
muhtari_status muhtari_create(const muhtari_config *, muhtari **);
void muhtari_destroy(muhtari *);
muhtari_status muhtari_set_input(muhtari *, const muhtari_input *);
muhtari_status muhtari_set_input_ex(muhtari *, const muhtari_input_ex *);

static void *layout_alloc(void *user, size_t n)
{
    (void)user;
    return malloc(n);
}
static void layout_free(void *user, void *p, size_t n)
{
    (void)user;
    (void)n;
    free(p);
}

int main(void)
{
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
    cfg.allocator.alloc = layout_alloc;
    cfg.allocator.free = layout_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), 0u);
    struct {
        muhtari_input_ex input;
        uint32_t canary;
    } object;
    memset(&object, 0, sizeof object);
    object.input.size = 88u;
    object.input.port[0].device = 1u;
    object.input.port[0].buttons = 17u;
    object.input.port[1].device = 5u;
    object.input.switches = 4u;
    object.canary = 0xA5A5A5A5u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &object.input), 0u);
    object.input.port[1].device = 3u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &object.input), 13u);
    CHECK_EQ_HEX(object.canary, 0xA5A5A5A5u);
    muhtari_input old;
    memset(&old, 0, sizeof old);
    old.size = 24u;
    CHECK_EQ_U64(muhtari_set_input(m, &old), 0u);
    old.device[0] = 3u;
    CHECK_EQ_U64(muhtari_set_input(m, &old), 13u);
    muhtari_destroy(m);
    CHECK_DONE();
}
