/* Cartridge images built in memory (images.h). */
#include "images.h"

#include <string.h>

void images_fill(uint8_t *p, size_t n, uint32_t seed)
{
    uint32_t x = seed * 2654435761u + 0x9E3779B9u;
    for (size_t i = 0; i < n; i++) {
        x = x * 1664525u + 1013904223u;
        p[i] = (uint8_t)(x >> 24);
    }
}

void images_footer(uint8_t *body, size_t n)
{
    size_t mapped = n < 48u * IMAGES_KIB ? n : 48u * IMAGES_KIB;
    uint32_t start = 0x10000u - (uint32_t)mapped;
    uint8_t *f = body + n - 8u; /* $FFF8-$FFFF */
    f[0] = 0xFFu;
    f[1] = (uint8_t)(((start >> 8) & 0xF0u) | 0x07u);
    f[4] = (uint8_t)(start & 0xFFu);
    f[5] = (uint8_t)(start >> 8);
}

void images_header(uint8_t *h, uint8_t version, const char *title, uint32_t rom_size,
                   uint16_t cart_type)
{
    memset(h, 0, IMAGES_HEADER);
    h[0] = version;
    memcpy(h + 1, "ATARI7800", 9);
    size_t len = strlen(title);
    memcpy(h + 17, title, len < 32u ? len : 32u);
    h[49] = (uint8_t)(rom_size >> 24);
    h[50] = (uint8_t)(rom_size >> 16);
    h[51] = (uint8_t)(rom_size >> 8);
    h[52] = (uint8_t)rom_size;
    h[53] = (uint8_t)(cart_type >> 8);
    h[54] = (uint8_t)cart_type;
    h[55] = 1u;
    h[56] = 1u;
    memcpy(h + 100, "ACTUAL CART DATA STARTS HERE", 28);
}

size_t images_linear(uint8_t *buf, uint8_t version, uint32_t body)
{
    images_header(buf, version, "Clean Linear", body, 0u);
    images_fill(buf + IMAGES_HEADER, body, body);
    images_footer(buf + IMAGES_HEADER, body);
    return IMAGES_HEADER + body;
}

size_t images_a78(uint8_t *buf, uint8_t version, uint32_t rom_size, uint16_t cart_type,
                  uint32_t body)
{
    images_header(buf, version, "Image", rom_size, cart_type);
    images_fill(buf + IMAGES_HEADER, body, rom_size ^ cart_type ^ body);
    return IMAGES_HEADER + body;
}

size_t images_headerless(uint8_t *buf, uint32_t n)
{
    images_fill(buf, n, n + 1u);
    images_footer(buf, n);
    return n;
}

size_t images_seeded(uint8_t *buf, uint32_t n, uint32_t seed)
{
    images_fill(buf, n, seed);
    images_footer(buf, n);
    return n;
}

size_t images_mirrored(uint8_t *buf)
{
    size_t n = images_seeded(buf, 32u * IMAGES_KIB, IMAGES_SEED_MIRRORED);
    memcpy(buf + 16u * IMAGES_KIB, buf + 24u * IMAGES_KIB, 8u * IMAGES_KIB);
    return n;
}

void images_swap_halves(uint8_t *p, size_t n)
{
    for (size_t bank = 0; bank + 16u * IMAGES_KIB <= n; bank += 16u * IMAGES_KIB) {
        for (size_t i = 0; i < 8u * IMAGES_KIB; i++) {
            uint8_t t = p[bank + i];
            p[bank + i] = p[bank + 8u * IMAGES_KIB + i];
            p[bank + 8u * IMAGES_KIB + i] = t;
        }
    }
}

size_t images_activision(uint8_t *buf)
{
    static const uint8_t program[] = {
        0xADu, 0x00u, 0xA0u, 0x8Du, 0x00u, 0x22u, /* LDA $A000; STA $2200 */
        0xA9u, 0x00u, 0x8Du, 0x8Du, 0xFFu,        /* data ignored; select five */
        0xADu, 0x00u, 0xC0u, 0x8Du, 0x01u, 0x22u, /* LDA $C000; STA $2201 */
        0x4Cu, 0x11u, 0xE1u                       /* JMP $E111 */
    };
    for (uint32_t half = 0u; half < 16u; half++) {
        memset(buf + half * 8192u, (int)(0x30u + half), 8192u);
    }
    memcpy(buf + 0x1E100u, program, sizeof program);
    buf[0x1FFF8u] = 0xFFu;
    buf[0x1FFF9u] = 0xE7u;
    buf[0x1FFFCu] = 0x00u;
    buf[0x1FFFDu] = 0xE1u;
    return 128u * IMAGES_KIB;
}

size_t images_2600(uint8_t *buf, uint32_t n)
{
    images_fill(buf, n, n + 2u);
    uint8_t *f = buf + n - 8u; /* $FFF8-$FFFF, as a 2600 cartridge maps them */
    f[0] = 0u;
    f[1] = 0u;
    f[4] = 0x00u;
    f[5] = 0xF0u;
    f[6] = 0x00u;
    f[7] = 0xF0u;
    return n;
}

size_t images_bankset(uint8_t *buf, uint32_t half_kib, uint8_t ram)
{
    uint32_t half = half_kib * IMAGES_KIB;
    uint32_t block = half_kib < 128u ? 4096u : 16384u;
    images_header(buf, 4u, "BankSet tags", half * 2u, 0u);
    buf[64] = (uint8_t)(half_kib < 128u ? 0u : 1u);
    buf[65] = (uint8_t)(0x80u | ram);
    for (uint32_t set = 0u; set < 2u; set++) {
        for (uint32_t bank = 0u; bank < half / block; bank++) {
            uint8_t *p = buf + IMAGES_HEADER + set * half + bank * block;
            memset(p, (int)(0x10u + set * 0x80u + bank * 2u), block);
            p[block - 1u]++;
        }
    }
    return IMAGES_HEADER + half * 2u;
}

static void ym_program_write(uint8_t *program, size_t *length, uint8_t reg, uint8_t value)
{
    static const uint8_t address_store[] = {0xA9u, 0x00u, 0x8Du, 0x60u, 0x04u};
    static const uint8_t data_store[] = {0xA9u, 0x00u, 0x8Du, 0x61u, 0x04u};
    uint8_t address[sizeof address_store];
    uint8_t data[sizeof data_store];
    memcpy(address, address_store, sizeof address);
    memcpy(data, data_store, sizeof data);
    address[1] = reg;
    data[1] = value;
    memcpy(program + *length, address, sizeof address);
    *length += sizeof address;
    memcpy(program + *length, data, sizeof data);
    *length += sizeof data;
}

static void ym_program_branch(uint8_t *program, size_t *length, uint8_t opcode, size_t target)
{
    program[*length] = opcode;
    (*length)++;
    int32_t displacement = (int32_t)target - (int32_t)(*length + 1u);
    program[*length] = (uint8_t)(int8_t)displacement;
    (*length)++;
}

static void ym_program_wait(uint8_t *program, size_t *length)
{
    size_t poll = *length;
    program[(*length)++] = 0xADu; /* LDA $0461: YM status */
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    ym_program_branch(program, length, 0x30u, poll); /* BMI while busy. */
}

static void ym_program_store(uint8_t *program, size_t *length, uint8_t value,
                             uint16_t address)
{
    program[(*length)++] = 0xA9u; /* LDA #value */
    program[(*length)++] = value;
    program[(*length)++] = 0x8Du; /* STA absolute */
    program[(*length)++] = (uint8_t)address;
    program[(*length)++] = (uint8_t)(address >> 8);
}

static void ym_program_timer_diagnostic(uint8_t *program, size_t *length)
{
    for (uint16_t address = 0x2200u; address < 0x2204u; address++)
        ym_program_store(program, length, 0u, address);

    /* A=1023 expires after one output step; B=255 after sixteen. */
    ym_program_write(program, length, 0x10u, 0xFFu);
    ym_program_wait(program, length);
    ym_program_write(program, length, 0x11u, 0x03u);
    ym_program_wait(program, length);
    ym_program_write(program, length, 0x12u, 0xFFu);
    ym_program_wait(program, length);
    ym_program_write(program, length, 0x14u, 0x0Fu);
    program[(*length)++] = 0xADu; /* Save immediate busy/status sample. */
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    program[(*length)++] = 0x8Du;
    program[(*length)++] = 0x00u;
    program[(*length)++] = 0x22u;

    size_t poll_a = *length;
    program[(*length)++] = 0xADu;
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    program[(*length)++] = 0x29u; /* AND #$01: wait for timer A's flag. */
    program[(*length)++] = 0x01u;
    ym_program_branch(program, length, 0xF0u, poll_a); /* BEQ until timer A expires. */
    program[(*length)++] = 0xADu;
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    program[(*length)++] = 0x8Du; /* Store timer A's independently timed status. */
    program[(*length)++] = 0x01u;
    program[(*length)++] = 0x22u;
    program[(*length)++] = 0x58u; /* CLI: admit the v4-qualified timer IRQ. */
    program[(*length)++] = 0x78u; /* SEI until timer B's flag is copied to the mailbox. */

    size_t poll_b = *length;
    program[(*length)++] = 0xADu; /* LDA $0461 */
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    program[(*length)++] = 0x29u; /* AND #$02 */
    program[(*length)++] = 0x02u;
    ym_program_branch(program, length, 0xF0u, poll_b); /* BEQ until timer B expires. */
    program[(*length)++] = 0xADu;
    program[(*length)++] = 0x61u;
    program[(*length)++] = 0x04u;
    program[(*length)++] = 0x8Du;
    program[(*length)++] = 0x03u;
    program[(*length)++] = 0x22u;
    program[(*length)++] = 0x58u; /* CLI after preserving the timer B status sample. */
}

static void ym_program_irq_handler(uint8_t *body)
{
    static const uint8_t handler[] = {0x48u,                   /* PHA */
                                      0xEEu, 0x02u, 0x22u,     /* INC $2202 */
                                      0xA9u, 0x14u,             /* LDA #$14 */
                                      0x8Du, 0x60u, 0x04u,     /* STA $0460 */
                                      0xA9u, 0x3Au,             /* LDA #$3A: keep B; stop A; clear both flags. */
                                      0x8Du, 0x61u, 0x04u,     /* STA $0461 */
                                      0x68u, 0x40u};            /* PLA; RTI */
    memcpy(body + 0x3F00u, handler, sizeof handler);
}

static size_t images_ym2151_build(uint8_t *buf, int worst)
{
    const uint32_t body_size = 16u * IMAGES_KIB;
    images_header(buf, 4u, worst ? "XM YM worst traffic" : "XM YM tracer", body_size, 0u);
    buf[58] = 0x02u; /* a persistent SaveKey, for state-refusal immutability */
    buf[63] = 1u;    /* installed XM device */
    buf[66] = 0u;
    buf[67] = 0x08u; /* v4 audio YM flag */
    buf[68] = 0u;
    buf[69] = 0x04u; /* YM IRQ qualification */

    uint8_t *body = buf + IMAGES_HEADER;
    memset(body, 0xEA, body_size);
    body[0] = 0xA2u; /* LDX #$FD: keep the diagnostic IRQ stack in system RAM. */
    body[1] = 0xFDu;
    body[2] = 0x9Au; /* TXS */
    size_t length = 3u;
    static const uint8_t enable[] = {0xA9u, 0x80u, 0x8Du, 0x70u, 0x04u};
    memcpy(body + length, enable, sizeof enable);
    length += sizeof enable;
    ym_program_write(body, &length, 0x20u, 0xC7u); /* both outputs, algorithm 7 */
    ym_program_write(body, &length, 0x28u, 0x4Cu);
    ym_program_write(body, &length, 0x30u, 0x00u);

    static const uint8_t reg_bases[] = {0x60u, 0x80u, 0xA0u, 0xC0u, 0xE0u, 0x40u};
    static const uint8_t values[] = {0x00u, 0x1Fu, 0x00u, 0x00u, 0x0Fu, 0x01u};
    static const uint8_t slots[] = {0u, 8u, 16u, 24u};
    for (unsigned group = 0u; group < sizeof reg_bases; group++) {
        for (unsigned slot = 0u; slot < sizeof slots; slot++) {
            ym_program_write(body, &length, (uint8_t)(reg_bases[group] + slots[slot]),
                             values[group]);
        }
    }
    ym_program_write(body, &length, 0x08u, 0x78u); /* key on all four channel-zero operators */
    ym_program_timer_diagnostic(body, &length);
    uint16_t loop = (uint16_t)(0xC000u + length);
    if (worst) {
        /* Keep a small, legal address/data sequence hot in the CPU bus loop. */
        loop = (uint16_t)(0xC000u + length);
        for (unsigned i = 0u; i < 256u; i++) {
            static const uint8_t regs[] = {0x28u, 0x30u, 0x40u, 0x60u, 0x80u, 0xA0u,
                                           0xC0u, 0xE0u};
            ym_program_write(body, &length, regs[i % (sizeof regs / sizeof regs[0])],
                             (uint8_t)(0x20u + (i & 0x1fu)));
        }
        body[length++] = 0x4Cu;
        body[length++] = (uint8_t)loop;
        body[length++] = (uint8_t)(loop >> 8);
    } else {
        loop = (uint16_t)(0xC000u + length);
        body[length++] = 0x4Cu;
        body[length++] = (uint8_t)loop;
        body[length++] = (uint8_t)(loop >> 8);
    }
    images_footer(body, body_size);
    ym_program_irq_handler(body);
    body[body_size - 2u] = 0x00u;
    body[body_size - 1u] = 0xFFu; /* YM timer IRQ handler at $FF00. */
    return IMAGES_HEADER + body_size;
}

size_t images_ym2151(uint8_t *buf)
{
    return images_ym2151_build(buf, 0);
}

size_t images_ym2151_worst(uint8_t *buf)
{
    return images_ym2151_build(buf, 1);
}
