/* The fast-boot hand-off: the state the NTSC BIOS leaves as it jumps to a
   cartridge, set directly with no BIOS run (A7800-SYSTEM section 7, C10;
   docs/hardware/bus.md, The run loop). */
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

    m->maria.origin = 0u;
    m->bus.ticks = m->maria.origin + BOOT_TICK;
    muhtari__maria_start(m);
    m->bus.next_deadline = 0u; /* recomputed by the first cycle */
    m->cpu.pc = (uint16_t)(muhtari__cart_peek(m, 0xFFFCu) |
                           ((uint16_t)muhtari__cart_peek(m, 0xFFFDu) << 8));
    m->cpu.s = BOOT_SX;
    m->cpu.x = BOOT_SX;
    m->cpu.a = 0u;
    m->cpu.y = 0u;
    m->cpu.p = BOOT_P;

    /* Frame 1 is the 8,190 ticks from the hand-off to VBLANK start. */
    m->frame_start = m->bus.ticks;
    m->frame_deadline = m->maria.origin + MUHTARI__VBLANK_START;
    m->audio_rem = 0u;
    m->loaded = 1u;
}
