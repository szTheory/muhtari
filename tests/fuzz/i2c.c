/* Bounded RIOT pin streams share one invariant consumer between portable
   replay and libFuzzer. Each byte is one action, at most 4096 actions; waits
   perform exactly 16 real bus reads. No host-time wait or unbounded packet
   loop exists. The cap keeps malformed wire traffic useful at fuzz rates
   while covering full pages and the selected programming interval.
   Wiring and programming: docs/hardware/controllers.md, Serial EEPROM. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "seeds.h"
#include "../alloc.h"

#define I2C_LIMIT 4096u
static uint8_t cart[128u + 4096u];
static uint8_t persistent_before[34816];
static struct muhtari observed, peer;

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static void require(int condition, const char *why)
{
    if (!condition) {
        fprintf(stderr, "i2c invariant: %s\n", why);
#ifdef MUHTARI_FUZZ_REPLAY
        exit(1);
#else
        abort();
#endif
    }
}

/* Low nibble of a pin action is SCL/SDA high bits. */
static uint8_t pin(unsigned scl, unsigned sda)
{
    return (uint8_t)((scl != 0u ? 2u : 0u) | (sda != 0u ? 1u : 0u));
}

static size_t seed_start(uint8_t *out, size_t n)
{
    out[n++] = pin(0u, 1u);
    out[n++] = pin(1u, 1u);
    out[n++] = pin(1u, 0u);
    out[n++] = pin(0u, 0u);
    return n;
}

static size_t seed_byte(uint8_t *out, size_t n, uint8_t byte)
{
    for (unsigned bit = 0u; bit < 8u; bit++) {
        unsigned high = ((unsigned)byte >> (7u - bit)) & 1u;
        out[n++] = pin(0u, high);
        out[n++] = pin(1u, high);
        out[n++] = pin(0u, high);
    }
    out[n++] = pin(0u, 1u);
    out[n++] = pin(1u, 1u);
    out[n++] = pin(0u, 1u);
    return n;
}

uint32_t seeds_count(void)
{
    return 7u;
}

const char *seeds_name(uint32_t i)
{
    static const char *const names[] = {"empty",      "page-port1",  "page-port2", "interrupted",
                                        "busy-reset", "all-actions", "maximum"};
    return names[i];
}

size_t seeds_build(uint32_t i, uint8_t *out)
{
    if (i == 0u)
        return 0u;
    if (i >= 5u) {
        size_t count = i == 6u ? I2C_LIMIT : 256u;
        for (size_t at = 0u; at < count; at++)
            out[at] = (uint8_t)at;
        return count;
    }
    out[0] = (uint8_t)(i == 1u ? 0u : 3u); /* either port; HSC coexistence */
    out[1] = 0x40u;                        /* ORA zero: DDRA selects open drain */
    size_t n = seed_start(out, 2u);
    n = seed_byte(out, n, 0xA0u);
    n = seed_byte(out, n, 0xFFu); /* ignored A15, last device page */
    n = seed_byte(out, n, 0xFEu);
    for (unsigned byte = 0u; byte < 66u; byte++)
        n = seed_byte(out, n, (uint8_t)byte);
    if (i == 3u)
        n = seed_start(out, n); /* abort buffered page */
    out[n++] = pin(0u, 0u);
    out[n++] = pin(1u, 0u);
    out[n++] = pin(1u, 1u);
    if (i == 4u)
        out[n++] = 0xA0u; /* reset before completion */
    for (unsigned wait = 0u; wait < 400u; wait++)
        out[n++] = 0xB0u;
    n = seed_start(out, n);
    n = seed_byte(out, n, 0xA1u); /* current read, master final NAK */
    for (unsigned bit = 0u; bit < 9u; bit++) {
        out[n++] = pin(0u, 1u);
        out[n++] = pin(1u, 1u);
        out[n++] = 0x80u; /* pure peek during data/ACK phases */
        out[n++] = pin(0u, 1u);
    }
    require(n <= I2C_LIMIT, "seed exceeds operation bound");
    return n;
}

static void action(muhtari *m, uint8_t byte)
{
    unsigned shift = m->battery.eeprom_port == 1u ? 4u : 0u;
    unsigned kind = byte >> 4;
    if (kind < 4u) {
        uint8_t low =
            (uint8_t)((((byte & 2u) == 0u ? 8u : 0u) | ((byte & 1u) == 0u ? 4u : 0u)) << shift);
        muhtari__bus_write(m, 0x0281u, low);
    } else if (kind < 8u) {
        muhtari__bus_write(m, 0x0280u, (uint8_t)((byte & 15u) << shift));
    } else if (kind == 8u) {
        uint64_t ticks = m->bus.ticks;
        struct muhtari__savekey before = m->savekey;
        (void)muhtari__bus_peek(m, 0x0280u);
        muhtari_battery_info info = {0};
        info.size = (uint32_t)sizeof info;
        require(muhtari_get_battery(m, &info) == MUHTARI_STATUS_OK, "battery query failed");
        require(ticks == m->bus.ticks && memcmp(&before, &m->savekey, sizeof before) == 0,
                "observer advanced protocol or time");
    } else if (kind == 9u) {
        (void)muhtari__bus_read(m, 0x0280u);
    } else if (kind == 10u) {
        require(muhtari_power_cycle(m) == MUHTARI_STATUS_OK, "reset failed");
    } else if (kind == 11u) {
        for (unsigned read = 0u; read < 16u; read++)
            (void)muhtari__bus_read(m, 0x1800u);
    } else if (kind == 12u) {
        struct muhtari__ports before = m->ports;
        muhtari_input input = {0};
        input.size = (uint32_t)sizeof input;
        input.device[m->battery.eeprom_port - 1u] = MUHTARI_PORT_UNPLUGGED;
        require(muhtari_set_input(m, &input) == MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE,
                "hot removal accepted");
        require(memcmp(&before, &m->ports, sizeof before) == 0, "refused input mutated ports");
    } else {
        /* Invalid load selectors are refused without altering live hardware. */
        muhtari_media invalid = {0};
        invalid.size = (uint32_t)sizeof invalid;
        invalid.image = cart;
        invalid.image_size = sizeof cart;
        invalid.port[0] = UINT32_MAX;
        require(muhtari_load(m, &invalid) == MUHTARI_STATUS_ERR_ARGUMENT,
                "invalid selector accepted");
    }
}

static void normalize(struct muhtari *copy)
{
    memset(&copy->allocator, 0, sizeof copy->allocator);
    copy->cart.rom = copy->cart.ram = NULL;
    copy->maria.rows = NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    alloc_counts counts[3];
    muhtari *m[3] = {NULL, NULL, NULL};
    unsigned mode = size != 0u ? data[0] : 0u;
    memset(cart, 0, sizeof cart);
    cart[0] = 3u;
    memcpy(cart + 1u, "ATARI7800", 9u);
    cart[51] = 0x10u;
    cart[55] = cart[56] = MUHTARI_PORT_PROLINE;
    cart[128] = 0x4Cu;
    cart[130] = 0xF0u;
    cart[128u + 4093u] = 0xF0u;
    muhtari_media media = {0};
    media.size = (uint32_t)sizeof media;
    media.image = cart;
    media.image_size = sizeof cart;
    media.port[mode & 1u] = (mode & 4u) != 0u ? MUHTARI_PORT_ATARIVOX : MUHTARI_PORT_SAVEKEY;
    media.hsc_mode = (mode & 2u) != 0u ? MUHTARI_HSC_ON : MUHTARI_HSC_OFF;
    for (unsigned i = 0u; i < 3u; i++) {
        muhtari_config config = alloc_config(&counts[i]);
        require(muhtari_create(&config, &m[i]) == MUHTARI_STATUS_OK, "create failed");
        require(muhtari_load(m[i], &media) == MUHTARI_STATUS_OK, "load failed");
    }
    if (size > I2C_LIMIT)
        size = I2C_LIMIT;
    for (size_t at = 1u; at < size; at++) {
        memcpy(persistent_before, m[0]->battery.data, sizeof persistent_before);
        uint64_t generation = m[0]->battery.generation;
        uint64_t due = m[0]->savekey.busy_until;
        unsigned pending = m[0]->savekey.pending;
        action(m[0], data[at]);
        action(m[1], data[at]);
        /* Only a due old page can publish during any of these actions. A
           reset changes epochs, so compare its pre-reset time separately. */
        if ((data[at] >> 4) != 10u && (pending == 0u || m[0]->bus.ticks < due)) {
            require(memcmp(persistent_before, m[0]->battery.data, sizeof persistent_before) == 0 &&
                        generation == m[0]->battery.generation,
                    "uncommitted traffic changed persistent bytes");
        }
    }
    /* Exercise no-poll completion once per input, not once per action. */
    for (unsigned i = 0u; i < 2u; i++) {
        muhtari_frame frame = {0};
        frame.size = (uint32_t)sizeof frame;
        require(muhtari_run_frame(m[i], &frame) == MUHTARI_STATUS_OK, "bounded frame failed");
    }
#ifdef MUHTARI_I2C_FAULT
    m[2]->battery.data[0] = 0u;
#endif
    for (size_t at = 0u; at < sizeof m[2]->battery.data; at++)
        require(m[2]->battery.data[at] == 0xFFu, "isolated instance changed");
    if (media.hsc_mode == MUHTARI_HSC_ON) {
        for (size_t at = 0u; at < 2048u; at++)
            require(m[0]->battery.data[at] == 0xFFu, "serial traffic changed HSC region");
    }
    for (size_t at = m[0]->battery.bytes; at < sizeof m[0]->battery.data; at++)
        require(m[0]->battery.data[at] == 0xFFu, "serial traffic changed inactive region");
    memcpy(&observed, m[0], sizeof observed);
    memcpy(&peer, m[1], sizeof peer);
    normalize(&observed);
    normalize(&peer);
    require(memcmp(&observed, &peer, sizeof observed) == 0, "identical traffic diverged");
    for (unsigned i = 0u; i < 3u; i++) {
        require(counts[i].allocs == 2u, "traffic allocated after load");
        muhtari_destroy(m[i]);
        require(counts[i].frees == 2u, "instance allocation leaked");
    }
    return 0;
}

#define MUHTARI_REPLAY_NAME "fuzz.regress.i2c"
#ifdef MUHTARI_FUZZ_REPLAY
int i2c_replay_main(int argc, char **argv);
#define main i2c_replay_main
#endif
#include "replay_main.h"
#ifdef MUHTARI_FUZZ_REPLAY
#undef main
int main(int argc, char **argv)
{
    if (argc != 3 || strcmp(argv[1], "--write-seeds") != 0)
        return i2c_replay_main(argc, argv);
    uint8_t bytes[I2C_LIMIT];
    for (uint32_t i = 0u; i < seeds_count(); i++) {
        char path[4096];
        int length = snprintf(path, sizeof path, "%s/%s", argv[2], seeds_name(i));
        require(length >= 0 && (size_t)length < sizeof path, "seed path too long");
        size_t count = seeds_build(i, bytes);
        FILE *file = fopen(path, "wb");
        require(file != NULL, "cannot open seed output");
        require(fwrite(bytes, 1u, count, file) == count, "cannot write seed output");
        require(fclose(file) == 0, "cannot close seed output");
    }
    return 0;
}
#endif
