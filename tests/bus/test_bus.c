/* The bus through the shipped archive, one case per run:

     muhtari-test-bus <case>

   Each case acts as the CPU: it calls the two bus functions on an instance
   in its power-on state and reads m->bus.ticks around each call, or runs a
   hand-assembled program through the CPU from RAM at $1800. Every timing
   check also requires that the nearest wrong model would give a different
   result, so a failure prints the expected value, the actual one and the
   wrong model's (docs/hardware/bus.md). Exit status: 0 when the case
   passes, 1 when it fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <string.h>

#include "internal.h"
#include "../alloc.h"

static muhtari machine;

/* The power-on state with INPTCTRL set to inptctrl and the HALT gate's
   count at 0. */
static muhtari *fresh(uint8_t inptctrl)
{
    memset(&machine, 0, sizeof machine);
    muhtari__power_on(&machine);
    machine.bus.inptctrl = inptctrl;
    return &machine;
}

/* MEN set and INPTCTRL locked, as a 7800 game leaves it. */
#define LOCKED_7800 0x07u

/* Fails when got is not expected, or when the wrong model named would give
   the same value, so that the case could not tell the two apart. unit names
   what is counted. */
static int expect_count(const char *what, const char *unit, uint64_t got, uint64_t expected,
                        const char *model, uint64_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives %llu %s\n", what, model, (unsigned long long)expected, unit);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected %llu %s got %llu (%s gives %llu)\n", what,
               (unsigned long long)expected, unit, (unsigned long long)got, model,
               (unsigned long long)wrong);
        return 0;
    }
    return 1;
}

static int expect_ticks(const char *what, uint64_t got, uint64_t expected, const char *model,
                        uint64_t wrong)
{
    return expect_count(what, "ticks", got, expected, model, wrong);
}

/* What request_halt or raise_nmi returned: 1 taken, 0 ignored. */
static int expect_taken(const char *what, int got, int expected, const char *model)
{
    return expect_count(what, "as return", (uint64_t)got, (uint64_t)expected, model,
                        (uint64_t)!expected);
}

/* The ticks one read and one write at addr take. */
static int cycle_ticks(muhtari *m, uint16_t addr, uint64_t expected, const char *model,
                       uint64_t wrong)
{
    char what[48];
    int ok = 1;
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, addr);
    snprintf(what, sizeof what, "read $%04x", (unsigned)addr);
    ok &= expect_ticks(what, m->bus.ticks - before, expected, model, wrong);
    before = m->bus.ticks;
    muhtari__bus_write(m, addr, 0x00u);
    snprintf(what, sizeof what, "write $%04x", (unsigned)addr);
    ok &= expect_ticks(what, m->bus.ticks - before, expected, model, wrong);
    return ok;
}

/* With MEN set, RAM, its page-0 window, MARIA and the cartridge space take
   4 ticks; a model with every cycle slow gives 6. */
static int cycle_fast_4_ticks(void)
{
    static const uint16_t fast[] = {0x0040u, 0x1800u, 0x2000u, 0x0020u, 0x8000u};
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    for (size_t i = 0u; i < sizeof fast / sizeof fast[0]; i++) {
        ok &= cycle_ticks(m, fast[i], 4u, "every cycle slow", 6u);
    }
    return ok;
}

/* With MEN set, the TIA and its mirrors, the RIOT's ports and its RAM take
   6 ticks, where a model with one cycle length gives 4; $0240, decoded by
   nothing, takes 4, where slowing the RIOT's whole page would give 6. */
static int cycle_slow_6_ticks(void)
{
    static const uint16_t slow[] = {0x0000u, 0x001Fu, 0x0100u, 0x0300u, 0x0280u, 0x0480u};
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    for (size_t i = 0u; i < sizeof slow / sizeof slow[0]; i++) {
        ok &= cycle_ticks(m, slow[i], 6u, "every cycle fast", 4u);
    }
    ok &= cycle_ticks(m, 0x0240u, 4u, "a slow RIOT page", 6u);
    return ok;
}

/* With MEN clear every cycle takes 6 ticks, where the 7800 decode would
   give 4 for these addresses. Writing 0 keeps MEN clear. */
static int cycle_men_off_all_slow(void)
{
    static const uint16_t addrs[] = {0x0040u, 0x1800u, 0x2000u, 0x0020u, 0x8000u, 0x0240u};
    muhtari *m = fresh(0x00u);
    int ok = 1;
    for (size_t i = 0u; i < sizeof addrs / sizeof addrs[0]; i++) {
        ok &= cycle_ticks(m, addrs[i], 6u, "the 7800 decode", 4u);
    }
    return ok;
}

static int expect_byte(const char *what, uint8_t got, uint8_t expected, const char *model,
                       uint8_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives $%02x\n", what, model, (unsigned)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected $%02x got $%02x (%s gives $%02x)\n", what, (unsigned)expected,
               (unsigned)got, model, (unsigned)wrong);
        return 0;
    }
    return 1;
}

/* Loads program at $1800 with S = $FD and points the CPU at it. */
static void load(muhtari *m, const uint8_t *program, size_t size)
{
    memcpy(&m->ram[0], program, size);
    m->cpu.pc = MUHTARI__RAM_BASE;
    m->cpu.s = 0xFDu;
}

/* Where a stack-page address sits in m->ram: $0140-$01FF is $2140-$21FF. */
static uint16_t ram_index_of(uint16_t addr)
{
    return (uint16_t)(addr + 0x2000u - MUHTARI__RAM_BASE);
}

/* Every TIA-range write writes INPTCTRL until LOCK is set; bits 4-7 are
   not stored, so $16 acts as $06. After the lock write, writes change
   nothing, and MEN stays set. */
static int inptctrl_lock(void)
{
    muhtari *m = fresh(0x00u);
    int ok = 1;
    muhtari__bus_write(m, 0x0001u, 0x16u);
    ok &= expect_byte("INPTCTRL after $16", m->bus.inptctrl, 0x06u, "storing 8 bits", 0x16u);
    muhtari__bus_write(m, 0x0100u, 0x07u);
    ok &= expect_byte("INPTCTRL after $07 at the $0100 mirror", m->bus.inptctrl, 0x07u,
                      "ignoring mirrors", 0x06u);
    muhtari__bus_write(m, 0x0001u, 0x00u);
    ok &= expect_byte("INPTCTRL after the lock", m->bus.inptctrl, 0x07u, "no lock", 0x00u);
    ok &= cycle_ticks(m, 0x1800u, 4u, "MEN cleared after the lock", 6u);
    return ok;
}

/* LDA $0240 reads an address nothing drives, so it returns the last byte on
   the bus: the operand's high byte, $02. A read after a write returns the
   written byte. A model of zeros gives $00 both times. */
static int open_bus_unmapped(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x40, 0x02, /* $1800 LDA $0240 */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    load(m, program, sizeof program);
    muhtari__cpu_step(m);
    ok &= expect_byte("LDA $0240", m->cpu.a, 0x02u, "zeros", 0x00u);
    muhtari__bus_write(m, 0x1900u, 0x5Au);
    ok &= expect_byte("read $0240 after writing $5a", muhtari__bus_read(m, 0x0240u), 0x5Au, "zeros",
                      0x00u);
    return ok;
}

/* Reads the TIA (or MARIA) register at addr right after a write of bus to
   RAM leaves bus on the data bus. */
static uint8_t read_after(muhtari *m, uint8_t bus, uint16_t addr)
{
    muhtari__bus_write(m, 0x1900u, bus);
    return muhtari__bus_read(m, addr);
}

/* The TIA drives D7-D6 of its collision registers and D7 of CXBLPF and
   INPT0-INPT5, with idle inputs: D7 = 0, except INPT4 and INPT5, whose
   buttons are up. Every other bit is the open-bus byte. MARIA's MSTAT at
   $28 drives D7 alone, VBLANK, which is set on line 0 at power-on. */
static int open_bus_write_only(void)
{
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    ok &= expect_byte("read $01 (CXM1P) after $ff", read_after(m, 0xFFu, 0x0001u), 0x3Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $0101 (CXM1P) after $ff", read_after(m, 0xFFu, 0x0101u), 0x3Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $06 (CXBLPF) after $ff", read_after(m, 0xFFu, 0x0006u), 0x7Fu,
                      "D7-D6 driven", 0x3Fu);
    ok &= expect_byte("read $08 (INPT0) after $ff", read_after(m, 0xFFu, 0x0008u), 0x7Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $0c (INPT4) after $00", read_after(m, 0x00u, 0x000Cu), 0x80u,
                      "open bus", 0x00u);
    ok &= expect_byte("read $28 (MARIA) after $a5", read_after(m, 0xA5u, 0x0028u), 0xA5u,
                      "the TIA's drivers", 0x25u);
    return ok;
}

/* A write to WSYNC ($02) holds RDY low until the TIA's line wraps: the
   first tick after the write's data tick, tick 5, at which tick plus
   tia_phase is a multiple of 456. A read then repeats whole 4-tick cycles
   until one starts at or after that tick, and runs in it; a write is not
   held. From tick 6 the read starts at 458 and ends at 462 with phi2 115
   (1 + 113 held + 1). */
static int rdy_holds_reads_only(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02", m->bus.ticks, 462u, "no hold", 10u);
    ok &= expect_ticks("phi2 after the held read", m->bus.phi2, 115u, "held cycles uncounted", 2u);

    m = fresh(LOCKED_7800);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    muhtari__bus_write(m, 0x1800u, 0x00u);
    ok &= expect_ticks("write after STA $02", m->bus.ticks, 10u, "writes held", 462u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02 and a write", m->bus.ticks, 462u, "the write releasing",
                       14u);

    /* With the TIA line 100 ticks in at tick 0, it wraps at tick 356; the
       read starts at 358. */
    m = fresh(LOCKED_7800);
    m->profile.tia_phase = 100u;
    muhtari__tia_reset(m);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02, tia_phase 100", m->bus.ticks, 362u,
                       "ignoring tia_phase", 462u);
    return ok;
}

/* INC $02 reads $02, writes it back, then writes the result: both writes
   reach WSYNC and neither is held, so it takes 4 + 4 + 6 + 6 + 6 ticks and
   leaves its result, $03, on the bus. The next opcode fetch is held: NOP
   from tick 26 fetches at 458 and ends at 466. */
static int rdy_rmw_completes_both_writes(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xE6, 0x02, /* $1800 INC $02 */
        0xEA,       /* $1802 NOP */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    load(m, program, sizeof program);
    muhtari__cpu_step(m);
    ok &= expect_ticks("INC $02", m->bus.ticks, 26u, "the final write held", 462u);
    ok &= expect_byte("the bus after INC $02", m->bus.open_bus, 0x03u, "one write", 0x02u);
    muhtari__cpu_step(m);
    ok &= expect_ticks("NOP after INC $02", m->bus.ticks, 466u, "no hold", 34u);
    return ok;
}

/* The ticks one read at $1800 takes. */
static uint64_t read_ticks(muhtari *m)
{
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x1800u);
    return m->bus.ticks - before;
}

/* Requests HALT now for 3 cycles and returns the ticks the next three reads
   take: 12 when the request is ignored, 4 + 4 + (4 + 12) when it is taken. */
static uint64_t halt_three_reads(muhtari *m)
{
    uint64_t before = m->bus.ticks;
    muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 12u);
    for (int i = 0; i < 3; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
    return m->bus.ticks - before;
}

/* HALT reaches SALLY only once two unlocked writes in the TIA range have
   been counted, whatever their data and at any mirror; writes after LOCK do
   not count. */
static int halt_gate_two_writes(void)
{
    int ok = 1;
    muhtari *m = fresh(0x02u);
    ok &= expect_taken("a request with the gate shut", muhtari__bus_request_halt(m, 0u, 12u), 0,
                       "an open gate");
    ok &= expect_ticks("HALT after no write", halt_three_reads(m), 12u, "an open gate", 24u);
    muhtari__bus_write(m, 0x0001u, 0x02u);
    ok &= expect_ticks("HALT after one write", halt_three_reads(m), 12u, "one write opening", 24u);
    muhtari__bus_write(m, 0x0101u, 0xFEu);
    ok &= expect_ticks("HALT after a second write at $0101", halt_three_reads(m), 24u,
                       "mirrors not counted", 12u);

    m = fresh(0x02u);
    muhtari__bus_write(m, 0x0001u, 0x03u);
    muhtari__bus_write(m, 0x0001u, 0x02u);
    ok &= expect_ticks("HALT after a lock write and one more", halt_three_reads(m), 12u,
                       "locked writes counted", 24u);
    return ok;
}

/* INC $06 from $1800 reads $06 (CXBLPF: D7 idle 0, the rest the operand
   left on the bus, $06), writes $06 back, then writes $07. Both writes are
   unlocked TIA writes, so the gate counts 2 and INPTCTRL ends $07. LDA $06
   writes nothing and STA $06 writes once. */
static int halt_gate_counts_rmw_dummy(void)
{
    /* clang-format off */
    static const uint8_t inc[] = {
        0xE6, 0x06, /* $1800 INC $06 */
    };
    static const uint8_t lda[] = {
        0xA5, 0x06, /* $1800 LDA $06 */
    };
    static const uint8_t sta[] = {
        0x85, 0x06, /* $1800 STA $06 */
    };
    /* clang-format on */
    int ok = 1;
    muhtari *m = fresh(0x02u);
    load(m, inc, sizeof inc);
    muhtari__cpu_step(m);
    ok &= expect_count("gate after INC $06", "writes", m->bus.halt_gate_writes, 2u,
                       "the dummy write not counted", 1u);
    ok &=
        expect_byte("INPTCTRL after INC $06", m->bus.inptctrl, 0x07u, "INPTCTRL left alone", 0x02u);
    m = fresh(0x02u);
    load(m, lda, sizeof lda);
    muhtari__cpu_step(m);
    ok &= expect_count("gate after LDA $06", "writes", m->bus.halt_gate_writes, 0u, "reads counted",
                       1u);
    m = fresh(0x02u);
    load(m, sta, sizeof sta);
    m->cpu.a = 0x02u;
    muhtari__cpu_step(m);
    ok &= expect_count("gate after STA $06", "writes", m->bus.halt_gate_writes, 1u,
                       "the operand fetch counted", 2u);
    return ok;
}

/* A request at a phi2 fall: the cycle starting there (c0) and the next (c1)
   run in 4 ticks, and c2 is frozen for the m = 5 cycles HALT is held, 4 + 20
   ticks, each counted in phi2. */
/* Owned 32 KiB CPU/MARIA sets. The CPU observes $51; DMA draws color 3,
   where accidentally selecting CPU graphics would draw color 1. */
static int bankset_frame(void)
{
    static uint8_t image[128u + 65536u];
    static uint8_t video[320u * 292u];
    static const uint8_t code[] = {0xADu, 0x00u, 0xB0u, 0x8Du, 0x00u, 0x18u, 0xA9u, 0xA0u,
                                   0x85u, 0x2Cu, 0xA9u, 0u,    0x85u, 0x30u, 0xA9u, 0x16u,
                                   0x85u, 0x21u, 0xA9u, 0x38u, 0x85u, 0x23u, 0xA9u, 0x40u,
                                   0x85u, 0x3Cu, 0x4Cu, 0x1Au, 0x80u};
    memset(image, 0, sizeof image);
    image[0] = 3u;
    memcpy(image + 1u, "ATARI7800", 9u);
    image[50] = 1u;    /* 65536-byte body, big endian */
    image[53] = 0x20u; /* BankSet */
    memcpy(image + 128u, code, sizeof code);
    for (unsigned set = 0u; set < 2u; set++) {
        uint8_t *p = image + 128u + set * 32768u;
        for (unsigned zone = 0u; zone < 32u; zone++) {
            p[0x2000u + zone * 3u] = 15u;
            p[0x2001u + zone * 3u] = 0xA1u;
        }
        p[0x2101u] = 31u;
        p[0x2102u] = 0xB1u;
        memset(p + 0x3100u, set == 0u ? 0x40 : 0xC0, 4096u);
    }
    image[128u + 0x3000u] = 0x51u;
    image[128u + 32768u + 0x3000u] = 0xA7u;
    image[128u + 0x7FFDu] = 0x80u;
    alloc_counts counts;
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK)
        return 0;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = sizeof image;
    muhtari_status status = muhtari_load(m, &media);
    int ok = expect_count("BankSet public load", "status", status, MUHTARI_STATUS_OK,
                          "refused board", MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    if (status == MUHTARI_STATUS_OK) {
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = 320u;
        for (unsigned frame = 0u; frame < 2u; frame++) {
            ok &= muhtari_run_frame(m, &io) == MUHTARI_STATUS_OK;
        }
        ok &= expect_count("CPU sentinel", "byte", m->ram[0], 0x51u, "MARIA set", 0xA7u);
        ok &= expect_count("native DMA pixel", "color", video[40u * 320u], 0x38u, "CPU graphics",
                           0x16u);
    }
    muhtari_destroy(m);
    return ok;
}

/* Execute CPU stores into each RAM owner, then DMA real graphics from RAM.
   Shared RAM draws CPU color1; HALT RAM draws the unequal MARIA color3. */
static int bankset_ram_frame(void)
{
    static uint8_t image[128u + 65536u];
    static uint8_t video[320u * 292u];
    static const uint8_t code[] = {0xA9u, 0x40u, 0x8Du, 0u,    0x41u, 0xA9u, 0xC0u, 0x8Du, 0u,
                                   0xC1u, 0xA9u, 0xA0u, 0x85u, 0x2Cu, 0xA9u, 0u,    0x85u, 0x30u,
                                   0xA9u, 0x16u, 0x85u, 0x21u, 0xA9u, 0x38u, 0x85u, 0x23u, 0xA9u,
                                   0x40u, 0x85u, 0x3Cu, 0x4Cu, 0x1Eu, 0x80u};
    int ok = 1;
    for (unsigned halt = 0u; halt < 2u; halt++) {
        memset(image, 0, sizeof image);
        image[0] = 3u;
        memcpy(image + 1u, "ATARI7800", 9u);
        image[50] = 1u;
        image[53] = halt != 0u ? 0x60u : 0x20u;
        image[54] = halt != 0u ? 0u : 4u;
        memcpy(image + 128u, code, sizeof code);
        for (unsigned set = 0u; set < 2u; set++) {
            uint8_t *p = image + 128u + set * 32768u;
            for (unsigned zone = 0u; zone < 32u; zone++) {
                p[0x2000u + zone * 3u] = 15u;
                p[0x2001u + zone * 3u] = 0xA1u;
            }
            p[0x2101u] = 31u;
            p[0x2102u] = 0x41u;
        }
        image[128u + 0x7FFDu] = 0x80u;
        alloc_counts counts;
        muhtari_config cfg = alloc_config(&counts);
        muhtari *m = NULL;
        if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK)
            return 0;
        muhtari_media media = {0};
        media.size = (uint32_t)sizeof media;
        media.image = image;
        media.image_size = sizeof image;
        if (muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
            muhtari_destroy(m);
            return 0;
        }
        memset(m->cart.ram, 0x40, 16384u);
        if (halt != 0u)
            memset(m->cart.ram + 16384u, 0xC0, 16384u);
        uint32_t allocations = counts.allocs;
        muhtari_frame io = {0};
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = 320u;
        for (unsigned frame = 0u; frame < 2u; frame++) {
            ok &= muhtari_run_frame(m, &io) == MUHTARI_STATUS_OK;
        }
        ok &= expect_count("CPU RAM store", "byte", m->cart.ram[256u], 0x40u, "redirected value",
                           0xC0u);
        if (halt != 0u) {
            ok &= expect_count("MARIA redirect", "byte", m->cart.ram[16640u], 0xC0u, "CPU value",
                               0x40u);
        }
        ok &=
            expect_count("RAM native DMA pixel", "color", video[40u * 320u],
                         halt != 0u ? 0x38u : 0x16u, "other RAM owner", halt != 0u ? 0x16u : 0x38u);
        ok &= counts.allocs == allocations;
        /* The bus owns MEN and BIOS priority above the cartridge decoder. */
        m->bus.inptctrl = 0u;
        m->bus.open_bus = 0xE7u;
        ok &= muhtari__bus_peek(m, 0xB000u) == 0xE7u;
        m->bus.inptctrl = 2u; /* MEN on, EXT off: BIOS answers upper ROM. */
        m->bios_size = 4096u;
        memset(m->bios, 0x6Bu, sizeof m->bios);
        ok &= muhtari__bus_peek(m, 0xF000u) == 0x6Bu;
        muhtari_destroy(m);
        ok &= counts.allocs == counts.frees;
    }
    return ok;
}

static int halt_views(void)
{
    static uint8_t rom[65536u];
    static uint8_t ram[32768u];
    static muhtari snapshot;
    static const uint8_t second[] = {8u, 9u, 10u, 11u};
    static const uint8_t c2[] = {8u, 10u, 10u, 12u};
    static const uint8_t release[] = {15u, 17u, 17u, 19u};
    int ok = 1;
    for (unsigned row = 0u; row < 4u; row++) {
        muhtari *m = fresh(LOCKED_7800);
        memset(rom, 0, sizeof rom);
        m->cart.rom = rom;
        m->cart.info.board = MUHTARI_BOARD_BANKSET;
        m->cart.info.rom_size = sizeof rom;
        m->cart.ram = ram;
        m->cart.info.ram_size = sizeof ram;
        m->cart.info.board_options = MUHTARI_BOARD_HALT_BANKED_RAM;
        memset(ram, 0, sizeof ram);
        ram[0] = 0x52u;
        ram[16383u] = 0x53u;
        ram[16384u] = 0xA8u;
        ram[32767u] = 0xA9u;
        rom[0x3000u] = 0x51u;
        rom[32768u + 0x3000u] = 0xA7u;
        m->bus.halt_gate_writes = 2u;
        m->maria.dl = 0x8000u; /* empty ordinary DL */
        m->maria.offset = 1u;
        m->maria.dma_lrc = 454u;
        m->maria.dma_line = 20u;
        muhtari__bus_request_halt(m, 0u, MUHTARI__HALT_UNTIL_MARIA);
        (void)muhtari__bus_read(m, (row & 2u) != 0u ? 0x0480u : 0x1800u);
        (void)muhtari__bus_read(m, (row & 1u) != 0u ? 0x0480u : 0xB000u);
        ok &=
            expect_ticks("actual second fall", m->bus.maria_from, second[row], "one fast edge", 4u);
        ok &= expect_ticks("c2 starts", m->bus.ticks, c2[row], "one cycle", 4u);
        ok &= muhtari__cart_dma_read(m, 0xB000u, second[row] - 1u) == 0x51u;
        ok &= muhtari__cart_dma_read(m, 0xB000u, second[row]) == 0xA7u;
        ok &= muhtari__cart_dma_read(m, 0x4000u, second[row] - 1u) == 0x52u;
        ok &= muhtari__cart_dma_read(m, 0x7FFFu, second[row] - 1u) == 0x53u;
        ok &= muhtari__cart_dma_read(m, 0x4000u, second[row]) == 0xA8u;
        ok &= muhtari__cart_dma_read(m, 0x7FFFu, second[row]) == 0xA9u;
        memcpy(&snapshot, m, sizeof snapshot);
        ok &= muhtari__bus_peek(m, 0xB000u) == 0x51u;
        ok &= muhtari__peek(m, 0xB000u) == 0x51u;
        ok &= memcmp(&snapshot, m, sizeof snapshot) == 0;
        (void)muhtari__bus_read(m, 0x1800u);
        /* Empty DL: four fetches 2 ticks apart, then 1-tick tail.
           Thus HALTRST-7 is the observed first fetch, with no added LRC. */
        ok &=
            expect_ticks("first fetch", m->bus.maria_until - 7u, c2[row], "retroactive fetch", 6u);
        ok &= expect_ticks("HALTRST", m->bus.maria_until, release[row], "enlarged deadline", 454u);
        ok &= muhtari__cart_dma_read(m, 0xB000u, release[row] - 1u) == 0xA7u;
        ok &= muhtari__cart_dma_read(m, 0xB000u, release[row]) == 0x51u;
        ok &= muhtari__cart_dma_read(m, 0x4000u, release[row] - 1u) == 0xA8u;
        ok &= muhtari__cart_dma_read(m, 0x7FFFu, release[row] - 1u) == 0xA9u;
        ok &= muhtari__cart_dma_read(m, 0x4000u, release[row]) == 0x52u;
        ok &= muhtari__cart_dma_read(m, 0x7FFFu, release[row]) == 0x53u;
        /* Non-HALT VBE uses CPU DLL data despite being called by MARIA. */
        rom[0x2000u] = 7u;
        rom[32768u + 0x2000u] = 3u;
        m->maria.reg[0x0Cu] = 0xA0u;
        m->maria.reg[0x10u] = 0u;
        (void)muhtari__maria_vbe(m, m->bus.ticks);
        ok &= m->maria.offset == 7u;
        /* RAM DLL helpers also keep the CPU half outside HALT. */
        m->maria.reg[0x0Cu] = 0x40u;
        ram[0] = 7u;
        ram[1] = 0x80u;
        ram[2] = 0u;
        ram[16384u] = 3u;
        ram[16385u] = 0x81u;
        ram[16386u] = 0u;
        (void)muhtari__maria_vbe(m, m->bus.ticks);
        ok &= m->maria.offset == 7u && m->maria.dl == 0x8000u;
        /* The next DLL's final byte is sampled on HALTRST, before release. */
        m->maria.dl = 0x8000u;
        m->maria.dll = 0xA000u;
        m->maria.offset = 0u;
        rom[32768u + 0x2002u] = 0x69u;
        uint64_t fall = m->bus.ticks;
        m->maria.dma_lrc = fall + 454u;
        muhtari__bus_request_halt(m, fall, MUHTARI__HALT_UNTIL_MARIA);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= m->maria.dl == 0x0069u && m->maria.offset == 3u;
        /* The final DLL sample at HALTRST selects MARIA physical RAM too. */
        m->maria.dl = 0x8000u;
        m->maria.dll = 0x4000u;
        m->maria.offset = 0u;
        ram[16384u] = 3u;
        ram[16385u] = 0u;
        ram[16386u] = 0x6Au;
        fall = m->bus.ticks;
        m->maria.dma_lrc = fall + 454u;
        muhtari__bus_request_halt(m, fall, MUHTARI__HALT_UNTIL_MARIA);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= m->maria.dl == 0x006Au && m->maria.offset == 3u;
        /* A delayed origin cannot extend the absolute LRC abort budget. */
        fall = m->bus.ticks;
        m->maria.dl = 0x8000u;
        m->maria.offset = 1u;
        m->maria.dma_lrc = fall + 10u;
        muhtari__bus_request_halt(m, fall, MUHTARI__HALT_UNTIL_MARIA);
        (void)muhtari__bus_read(m, 0x0480u);
        (void)muhtari__bus_read(m, 0x0480u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= m->bus.maria_until == fall + 13u;
        ok &= (m->maria.stats[20].flags & MUHTARI__LINE_ABORTED) != 0u;
    }
    return ok;
}

/* Bus-level counterexample, not a demonstrated ROM instruction stream:
   slow RIOT c0 then fast cartridge-RAM c1 commits at tick 10. */
static int halt_store_order(void)
{
    static uint8_t rom[32768u], ram[16384u];
    muhtari *m = fresh(LOCKED_7800);
    memset(rom, 0, sizeof rom);
    memset(ram, 0, sizeof ram);
    m->bus.halt_gate_writes = 2u;
    m->cart.rom = rom;
    m->cart.ram = ram;
    m->cart.info.board = MUHTARI_BOARD_LINEAR;
    m->cart.info.rom_size = sizeof rom;
    m->cart.info.ram_size = sizeof ram;
    rom[256u] = 0x40u;
    rom[257u] = 0xC0u;
    ram[1u] = 31u;
    ram[2u] = 0x80u;
    m->maria.dl = 0x4000u;
    m->maria.offset = 1u;
    m->maria.dma_lrc = 454u;
    m->maria.dma_line = 20u;
    muhtari__bus_request_halt(m, 0u, MUHTARI__HALT_UNTIL_MARIA);
    (void)muhtari__bus_read(m, 0x0480u);
    muhtari__bus_write(m, 0x4000u, 1u);
    int ok = m->bus.ticks == 10u && m->bus.maria_from == 10u && ram[0] == 1u;
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_count("committed c1 graphics", "cell", m->maria.line_ram[0][0], 3u,
                       "pre-store graphics", 1u);
    return ok;
}

/* Retained sample traces expose these ordinary/last costs at actual
   stretched handoffs. Reproduce their fetch/release arithmetic without
   depending on a sample's opaque frame or audio digest (bus.md, HALT). */
static int halt_stretched_costs(void)
{
    static const struct {
        uint8_t headers, width, five, last, slow_first;
        uint16_t cost, release, resume;
    } rows[] = {{0u, 0u, 0u, 0u, 0u, 9u, 17u, 30u},
                {1u, 30u, 1u, 0u, 0u, 109u, 117u, 130u},
                {1u, 32u, 1u, 1u, 0u, 121u, 129u, 142u},
                {2u, 24u, 0u, 0u, 1u, 169u, 177u, 190u},
                {2u, 29u, 0u, 0u, 0u, 199u, 207u, 218u}};
    static uint8_t rom[32768u];
    int ok = 1;
    for (unsigned i = 0u; i < sizeof rows / sizeof rows[0]; i++) {
        muhtari *m = fresh(LOCKED_7800);
        memset(m->ram, 0, sizeof m->ram);
        memset(rom, 0, sizeof rom);
        m->cart.rom = rom;
        m->cart.info.board = MUHTARI_BOARD_LINEAR;
        m->cart.info.rom_size = sizeof rom;
        m->bus.halt_gate_writes = 2u;
        m->maria.dl = 0x1800u;
        m->maria.dll = 0x1A00u;
        m->maria.offset = rows[i].last ? 0u : 1u;
        m->maria.dma_line = 20u;
        m->maria.dma_lrc = 424u;
        unsigned p = 0u;
        for (unsigned h = 0u; h < rows[i].headers; h++) {
            m->ram[p++] = 0u;
            m->ram[p++] = rows[i].five ? 0x40u : (uint8_t)(32u - rows[i].width);
            m->ram[p++] = 0x80u;
            if (rows[i].five)
                m->ram[p++] = (uint8_t)((32u - rows[i].width) & 31u);
            m->ram[p++] = 0u;
        }
        muhtari__bus_request_halt(m, 0u, MUHTARI__HALT_UNTIL_MARIA);
        (void)muhtari__bus_read(m, rows[i].slow_first ? 0x0480u : 0x1800u);
        (void)muhtari__bus_read(m, rows[i].slow_first ? 0x1800u : 0x0480u);
        ok &= m->bus.ticks == 10u;
        ok &= m->bus.maria_from == (rows[i].slow_first ? 10u : 9u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= expect_ticks("stretched HALTRST", m->bus.maria_until, rows[i].release,
                           "unclamped origin", (uint64_t)rows[i].release - 2u);
        ok &= expect_ticks("stretched c2 completion", m->bus.ticks, (uint64_t)rows[i].resume + 4u,
                           "early CPU resumption", 0u);
        ok &= m->maria.stats[20].cost == rows[i].cost;
        ok &= m->maria.stats[20].flags == (rows[i].last ? MUHTARI__LINE_LAST : 0u);
        ok &= m->bus.nmi_pending == 0u && m->bus.nmi_phi2 == 0u;
        ok &= m->maria.dma_lrc == 424u;
    }
    return ok;
}

static int halt_latency_two_cycles(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    muhtari__bus_request_halt(m, 4u, 24u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "one cycle of latency", 24u);
    ok &= expect_ticks("c2", read_ticks(m), 24u, "no freeze", 4u);
    ok &= expect_count("phi2 after c2", "cycles", m->bus.phi2, 9u, "frozen cycles uncounted", 4u);
    return bankset_frame() && bankset_ram_frame() && halt_views() && halt_store_order() &&
           halt_stretched_costs() && ok;
}

/* A request at tick 2, inside a 6-tick cycle, takes effect at that cycle's
   end: c0 starts at 6, and c2 is frozen for the 3 cycles from 6 to 18.
   Counting the cycle in flight as c0 would freeze the second fast read. */
static int halt_waits_for_slow_fall(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    muhtari__bus_request_halt(m, 2u, 18u);
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x0000u);
    ok &= expect_ticks("the slow cycle in flight", m->bus.ticks - before, 6u,
                       "freezing the cycle in flight", 18u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "the in-flight cycle as c0", 16u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "the in-flight cycle as c0", 16u);
    ok &= expect_ticks("c2", read_ticks(m), 16u, "no freeze", 4u);
    return ok;
}

/* HALT freezes a write as it freezes a read: c2 as a write takes 4 + 20
   ticks. */
static int halt_freezes_writes(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    muhtari__bus_request_halt(m, 4u, 24u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "one cycle of latency", 24u);
    uint64_t before = m->bus.ticks;
    muhtari__bus_write(m, 0x1800u, 0x00u);
    ok &= expect_ticks("c2, a write", m->bus.ticks - before, 24u, "writes never frozen", 4u);
    return ok;
}

/* One window at a time. A request made after /HALT falls, before c2's hold,
   is ignored and returns 0: c2 still freezes for the first window's 5
   cycles. A request made before the first window's fall is ignored too, so
   /HALT falls at tick 8, not 4. Once a window ends, the next request is
   taken. */
static int halt_one_window_at_a_time(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_taken("the first request", muhtari__bus_request_halt(m, 4u, 24u), 1,
                       "every request ignored");
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_taken("a second request after the fall",
                       muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 40u), 0,
                       "the second request replacing the first");
    ok &= expect_ticks("c1, a second request after the fall", read_ticks(m), 4u,
                       "one cycle of latency", 24u);
    ok &= expect_ticks("c2, a second request after the fall", read_ticks(m), 24u,
                       "the second request replacing the first", 4u);
    ok &= expect_ticks("a request after the window", halt_three_reads(m), 24u,
                       "requests ignored after a window", 12u);

    m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_taken("the first request, at tick 8", muhtari__bus_request_halt(m, 8u, 28u), 1,
                       "every request ignored");
    ok &= expect_taken("a second request before the fall", muhtari__bus_request_halt(m, 4u, 24u), 0,
                       "the second request replacing the first");
    ok &= expect_ticks("the cycle before the fall", read_ticks(m), 4u,
                       "freezing the cycle before the fall", 24u);
    ok &=
        expect_ticks("c0, a second request before the fall", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1, a second request before the fall", read_ticks(m), 4u,
                       "the second request replacing the first", 24u);
    ok &= expect_ticks("c2, a second request before the fall", read_ticks(m), 24u,
                       "the second request replacing the first", 4u);
    return ok;
}

/* An edge raised at a tick is latched by the first cycle whose data tick
   falls at or after it, and nmi_phi2 names that cycle. With MEN clear
   nothing is latched. */
static int nmi_latched_in_cycle(void)
{
    static const struct {
        const char *what;
        uint64_t at;
        uint64_t expected;
        const char *model;
        uint64_t wrong;
    } cases[] = {
        {"edge at tick 6, inside cycle 2", 6u, 2u, "latching at the next cycle", 3u},
        {"edge at tick 8, cycle 2's fall", 8u, 2u, "latching after the fall", 3u},
        {"edge at tick 13, the slow cycle 3's fall", 13u, 3u, "a slow cycle's data at its tick 4",
         4u},
        {"edge at tick 14, the slow cycle 3's end", 14u, 4u, "latching by the cycle's end", 3u},
    };
    int ok = 1;
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        muhtari *m = fresh(LOCKED_7800);
        muhtari__bus_raise_nmi(m, cases[i].at);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x0000u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= expect_count(cases[i].what, "as nmi_phi2", m->bus.nmi_phi2, cases[i].expected,
                           cases[i].model, cases[i].wrong);
    }
    muhtari *m = fresh(0x00u);
    ok &= expect_taken("edge with MEN clear", muhtari__bus_raise_nmi(m, 2u), 0, "no MEN gate");
    (void)muhtari__bus_read(m, 0x1800u);
    ok &=
        expect_count("edge with MEN clear", "as nmi_phi2", m->bus.nmi_phi2, 0u, "no MEN gate", 1u);
    return ok;
}

/* One edge at a time: an edge at tick 14, raised while the edge at tick 6
   is not yet latched, is ignored and returns 0, so cycle 2 latches the
   first. A 1 means the edge is queued for latching, not that it latches:
   an edge queued at tick 6, with a second raise refused behind it, is
   dropped by the latch when MEN is cleared before cycle 2. */
static int nmi_one_edge_at_a_time(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    ok &= expect_taken("the first edge", muhtari__bus_raise_nmi(m, 6u), 1, "every edge ignored");
    ok &= expect_taken("a second edge before the first is latched", muhtari__bus_raise_nmi(m, 14u),
                       0, "the second edge replacing the first");
    for (int i = 0; i < 4; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
    ok &= expect_count("a second edge before the first is latched", "as nmi_phi2", m->bus.nmi_phi2,
                       2u, "the second edge replacing the first", 4u);

    const char *dropped = "a queued edge dropped by the latch";
    m = fresh(LOCKED_7800);
    ok &= expect_taken(dropped, muhtari__bus_raise_nmi(m, 6u), 1, "refusing an edge MEN drops");
    ok &= expect_taken(dropped, muhtari__bus_raise_nmi(m, 14u), 0,
                       "a second edge queued behind the first");
    m->bus.inptctrl = (uint8_t)(m->bus.inptctrl & ~MUHTARI__INPTCTRL_MEN);
    for (int i = 0; i < 4; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
    ok &= expect_count(dropped, "as nmi_phi2", m->bus.nmi_phi2, 0u, "latching under MEN = 0", 2u);
    ok &= expect_count(dropped, "as nmi_pending", m->bus.nmi_pending, 0u, "keeping the edge queued",
                       1u);
    return ok;
}

static int expect_address(const char *what, uint16_t got, uint16_t expected, const char *model,
                          uint16_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives $%04x\n", what, model, (unsigned)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: return address expected $%04x got $%04x (%s gives $%04x)\n", what,
               (unsigned)expected, (unsigned)got, model, (unsigned)wrong);
        return 0;
    }
    return 1;
}

/* The return address an NMI entry pushed, read from the stack page's RAM,
   or 0 when S shows no entry. */
static uint16_t nmi_return(const muhtari *m)
{
    if (m->cpu.s != 0xFAu) {
        return 0u;
    }
    uint16_t lo = ram_index_of(0x01FCu);
    uint16_t hi = ram_index_of(0x01FDu);
    return (uint16_t)(m->ram[lo] | ((uint32_t)m->ram[hi] << 8));
}

/* LDA $1900 takes cycles 1-4 from tick 0; an edge at tick 14 falls in
   cycle 4, its last, so it is taken after the NOP that follows. */
static int nmi_last_cycle_after_next(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x00, 0x19, /* $1800 LDA $1900 */
        0xEA,             /* $1803 NOP */
        0xEA,             /* $1804 NOP */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    load(m, program, sizeof program);
    muhtari__bus_raise_nmi(m, 14u);
    muhtari__cpu_step(m);
    muhtari__cpu_step(m);
    return expect_address("LDA $1900, edge at tick 14", nmi_return(m), 0x1804u,
                          "taking it after LDA", 0x1803u);
}

/* STA $02 from $1800 runs from tick 0 to 14 and pulls RDY low until tick
   456, so CLC's opcode fetch is held in cycles 4 to 114. An edge at tick
   100 falls in held cycle 25, before the fetch completes at phi2 115, so it
   is taken after CLC. Counting from CLC's start would compare against phi2
   4 and take it after the NOP. */
static int nmi_held_fetch_after_wsync(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0x85, 0x02, /* $1800 STA $02 */
        0x18,       /* $1802 CLC */
        0xEA,       /* $1803 NOP */
        0xEA,       /* $1804 NOP */
    };
    /* clang-format on */
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    (void)muhtari__bus_read(m, 0x1800u);
    (void)muhtari__bus_read(m, 0x1801u);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    muhtari__bus_raise_nmi(m, 100u);
    (void)muhtari__bus_read(m, 0x1802u);
    ok &= expect_count("edge at tick 100 in the held fetch", "as nmi_phi2", m->bus.nmi_phi2, 25u,
                       "the completing cycle's count", 115u);

    m = fresh(LOCKED_7800);
    load(m, program, sizeof program);
    muhtari__bus_raise_nmi(m, 100u);
    muhtari__cpu_step(m);
    muhtari__cpu_step(m);
    ok &= expect_address("STA $02; CLC, edge at tick 100", nmi_return(m), 0x1803u,
                         "counting from CLC's start", 0x1804u);
    return ok;
}

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"cycle.fast_4_ticks", cycle_fast_4_ticks},
    {"cycle.slow_6_ticks", cycle_slow_6_ticks},
    {"cycle.men_off_all_slow", cycle_men_off_all_slow},
    {"rdy.holds_reads_only", rdy_holds_reads_only},
    {"rdy.rmw_completes_both_writes", rdy_rmw_completes_both_writes},
    {"inptctrl.lock", inptctrl_lock},
    {"open_bus.unmapped", open_bus_unmapped},
    {"open_bus.write_only", open_bus_write_only},
    {"halt.gate_two_writes", halt_gate_two_writes},
    {"halt.gate_counts_rmw_dummy", halt_gate_counts_rmw_dummy},
    {"halt.latency_two_cycles", halt_latency_two_cycles},
    {"halt.waits_for_slow_fall", halt_waits_for_slow_fall},
    {"halt.freezes_writes", halt_freezes_writes},
    {"halt.one_window_at_a_time", halt_one_window_at_a_time},
    {"nmi.latched_in_cycle", nmi_latched_in_cycle},
    {"nmi.last_cycle_after_next", nmi_last_cycle_after_next},
    {"nmi.held_fetch_after_wsync", nmi_held_fetch_after_wsync},
    {"nmi.one_edge_at_a_time", nmi_one_edge_at_a_time},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-bus <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            return cases[i].run() ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-bus: unknown case %s\n", argv[1]);
    return 2;
}
