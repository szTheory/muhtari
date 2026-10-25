/* The two boots: the fast-boot hand-off, the state the NTSC BIOS leaves
   as it jumps to a cartridge, set directly with no BIOS run (A7800-SYSTEM
   section 7, C10; docs/hardware/bus.md, The run loop); and the BIOS boot,
   power-on and RES with the BIOS mapped (A7800-SYSTEM sections 7 and 8;
   docs/embedding.md, Booting with a BIOS). */
#include <string.h>

#include "internal.h"

/* The beam at the first cartridge opcode fetch: row 239, column 436, which
   is line 240, t 24, 8,190 ticks before VBLANK start (A7800-SYSTEM section
   7, C10, measured on Choplifter). */
#define BOOT_LINE 240u
#define BOOT_T 24u
#define BOOT_TICK (BOOT_LINE * MUHTARI__LINE_TICKS + BOOT_T)
_Static_assert(MUHTARI__VBLANK_START - BOOT_TICK == 8190u, "C10: 8,190 ticks before VBLANK");

/* The CPU's cycle grid against the beam: the hand-off fetch starts at t 24,
   a multiple of 4, so the grid's offset is 0. Derived from C10, fitted; an
   RTL simulation would confirm it. */
#define BOOT_CPU_PHASE 0u
_Static_assert(BOOT_TICK % 4u == BOOT_CPU_PHASE, "the hand-off tick sets the CPU phase");

/* C10's CPU and system state: INPTCTRL $16 (acting as $06) with the HALT
   gate's two writes counted, P with I and D set, S = X = $16, CTRL $60
   and DPP $1F84, and MARIA's DMA internals: the DL at $2200, the next DLL
   entry at $1FF3 and OFFSET 2. U reads 1 on every 6502, so P holds it. */
#define BOOT_INPTCTRL 0x16u
#define BOOT_SX 0x16u
#define BOOT_P 0x2Cu
#define BOOT_CTRL 0x60u
#define BOOT_DPPH 0x1Fu
#define BOOT_DPPL 0x84u
#define BOOT_DL 0x2200u
#define BOOT_DLL 0x1FF3u
#define BOOT_OFFSET 2u

/* MARIA's registers, as indexes into reg (A7800-MARIA section 2.4). */
#define REG_DPPH 0x0Cu
#define REG_DPPL 0x10u
#define REG_CTRL 0x1Cu

void muhtari__boot_fast(muhtari *m)
{
    /* The CPU, the bus, RAM by ram_fill, the RIOT and the cartridge's banks
       and RAM at power-on; palettes, BACKGRND and CHARBASE 0. */
    muhtari__power_on(m);
    memset(&m->maria, 0, sizeof m->maria);
    m->bus.inptctrl = (uint8_t)(BOOT_INPTCTRL & MUHTARI__INPTCTRL_BITS);
    m->bus.halt_gate_writes = 2u;
    m->maria.reg[REG_CTRL] = BOOT_CTRL;
    m->maria.reg[REG_DPPH] = BOOT_DPPH;
    m->maria.reg[REG_DPPL] = BOOT_DPPL;
    m->maria.dl = BOOT_DL;
    m->maria.dll = BOOT_DLL;
    m->maria.offset = BOOT_OFFSET;

    /* PAL handoff is a fitted compatibility snapshot, with no measured
       undocumented DMA residue (docs/hardware/bus.md, PAL startup). */
    int pal = m->effective_region == MUHTARI_REGION_PAL;
    if (pal) {
        m->maria.reg[REG_DPPH] = 0x27u;
        m->maria.reg[REG_DPPL] = 0x07u;
        m->maria.dl = 0u;
        m->maria.dll = 0u;
        m->maria.offset = 0u;
        memset(m->ram + 0x800u, 0xFF, 64u);
        memset(m->ram + 0x900u, 0xFF, 64u);
    }

    m->maria.origin = 0u;
    m->bus.ticks = m->maria.origin + (pal ? 140016u : BOOT_TICK);
    /* TIA ran silently on its absolute lattice while fast boot skipped
       the CPU. Actual cartridge phi2 instead begins here at cycle zero;
       the released output grid begins here too (embedding.md). */
    muhtari__tia_catch_up(m, m->bus.ticks);
    m->bus.clock_origin_tick = m->bus.ticks;
    m->bus.clock_origin_phi2 = m->bus.phi2;
    muhtari__cart_reset(m);
    muhtari__maria_start(m);
    m->bus.next_deadline = 0u; /* recomputed by the first cycle */
    m->cpu.pc = (uint16_t)(muhtari__cart_peek(m, 0xFFFCu) |
                           ((uint16_t)muhtari__cart_peek(m, 0xFFFDu) << 8));
    m->cpu.s = BOOT_SX;
    m->cpu.x = BOOT_SX;
    m->cpu.a = pal ? 0x60u : 0u;
    m->cpu.y = 0u;
    m->cpu.p = BOOT_P;

    /* Frame 1 is the 8,190 ticks from the hand-off to VBLANK start. */
    m->frame_start = m->bus.ticks;
    struct muhtari__timing timing = muhtari__timing(m);
    m->frame_deadline = m->maria.origin + timing.vblank;
    if (m->frame_deadline <= m->frame_start)
        m->frame_deadline += timing.frame_ticks;
    m->audio_rem = 0u;
    m->loaded = 1u;
    m->boot.handoff_frame = 1u;
    m->boot.rejected = 0u;
}

void muhtari__boot_bios(muhtari *m)
{
    /* Power-on (A7800-SYSTEM section 8): the CPU's registers 0, INPTCTRL
       0, so MEN = 0 and EXT = 0 and the BIOS answers the 2600 decode's
       A12 = 1, RAM by ram_fill, the RIOT and the cartridge at power-on,
       and MARIA with CTRL $00, DMA off. */
    muhtari__power_on(m);
    memset(&m->maria, 0, sizeof m->maria);
    m->maria.origin = 0u;
    m->bus.ticks = 0u;
    muhtari__maria_start(m);
    m->bus.next_deadline = 0u;
    m->boot.handoff_frame = 0u;
    m->boot.rejected = 0u;

    /* Frame 1 runs from line 0, t 0 to the first VBLANK start, 117,174
       ticks; RES's seven cycles are its first. */
    m->frame_start = 0u;
    m->frame_deadline = muhtari__timing(m).vblank;
    m->audio_rem = 0u;
    m->loaded = 1u;
    muhtari__cpu_reset(m);
}
_Static_assert(MUHTARI__VBLANK_START == 117174u, "frame 1 under the BIOS");

/* While EXT = 0 the BIOS ROM is selected in place of the cartridge: with
   MEN = 1 at every A15 = 1 address, the image mirrored over $8000-$FFFF
   (MiSTer; a 16 KiB image twice, a 4 KiB one eight times), and with
   MEN = 0 at every A12 = 1 address, where the 2600 decode sees the image's
   top 4 KiB (A7800-SYSTEM section 4; 7800OpenBIOS's notes on INPTCTRL). */
int muhtari__boot_bios_maps(const muhtari *m, uint16_t addr)
{
    uint8_t c = m->bus.inptctrl;
    if (m->bios_size == 0u || (c & MUHTARI__INPTCTRL_EXT) != 0u) {
        return 0;
    }
    if ((c & MUHTARI__INPTCTRL_MEN) != 0u) {
        return (addr & 0x8000u) != 0u;
    }
    return (addr & 0x1000u) != 0u;
}

uint8_t muhtari__boot_bios_byte(const muhtari *m, uint16_t addr)
{
    if ((m->bus.inptctrl & MUHTARI__INPTCTRL_MEN) != 0u) {
        return m->bios[addr & (m->bios_size - 1u)];
    }
    return m->bios[m->bios_size - 0x1000u + (addr & 0x0FFFu)];
}

uint64_t muhtari__boot_handoff_frame(const muhtari *m)
{
    return m->boot.handoff_frame;
}
