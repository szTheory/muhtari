/* Declarations shared by the core's source files. Not installed. */
#ifndef MUHTARI_INTERNAL_H
#define MUHTARI_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "muhtari.h"

/* The NTSC frame the core emits: 320 pixels by 242 rows, frame rows 0-241
   being rasters 16-257 (A7800-MARIA section 6). */
#define MUHTARI__WIDTH 320u
#define MUHTARI__HEIGHT_NTSC 242u

/* One NTSC frame is 263 lines of 454 ticks, 119,402 ticks. At 48 kHz that is
   88 samples per 13,125 ticks, 800 or 801 per frame (A7800-AUDIO, timing
   table). */
#define MUHTARI__TICKS_PER_FRAME_NTSC 119402u
#define MUHTARI__SAMPLES_PER_PERIOD 88u
#define MUHTARI__TICKS_PER_PERIOD 13125u
#define MUHTARI__MAX_AUDIO_SAMPLES 801u

/* The bounds of muhtari_frame.video_pitch. The cap keeps row offsets inside a
   32-bit size_t. */
#define MUHTARI__PITCH_MIN 320u
#define MUHTARI__PITCH_MAX 65536u

/* SALLY's registers. P is held raw, as the stack and the vectors see it.
   jammed is set by a JAM opcode; the CPU then runs no more instructions.
   poll_phi2 is the phi2 count at which the last instruction's deciding cycle
   completed: an NMI latched at or before it is taken after that instruction
   (docs/hardware/sally.md, NMI). */
struct muhtari__cpu {
    uint64_t poll_phi2;
    uint16_t pc;
    uint8_t a, x, y, s, p;
    uint8_t jammed;
};

/* The bus's state (docs/hardware/bus.md). ticks counts MARIA clocks and
   phi2 counts completed CPU cycles, held ones included. nmi_phi2 is the phi2
   count of the cycle that latched an NMI edge, 0 when none is latched.
   next_deadline is the earliest tick at which a pending event needs the
   bus's slow path; UINT64_MAX when none is pending. open_bus is the last
   byte on the data bus; inptctrl is INPTCTRL's four bits; halt_gate_writes
   counts unlocked writes in the TIA range, up to 2. rdy_release is the
   tick from which a held read may run, 0 while RDY is high. */
struct muhtari__bus {
    uint64_t phi2;
    uint64_t nmi_phi2;
    uint64_t ticks;
    uint64_t next_deadline;
    uint64_t rdy_release;
    uint8_t open_bus;
    uint8_t inptctrl;
    uint8_t halt_gate_writes;
};

/* INPTCTRL's bits (docs/hardware/bus.md, INPTCTRL). */
#define MUHTARI__INPTCTRL_LOCK 0x01u
#define MUHTARI__INPTCTRL_MEN 0x02u
#define MUHTARI__INPTCTRL_EXT 0x04u
#define MUHTARI__INPTCTRL_TIA_EN 0x08u
#define MUHTARI__INPTCTRL_BITS 0x0Fu

/* The 4 KiB of system RAM at $1800-$27FF (docs/hardware/bus.md, Memory map). */
#define MUHTARI__RAM_BASE 0x1800u
#define MUHTARI__RAM_SIZE 0x1000u

/* RAM fill patterns at power-on. */
#define MUHTARI__RAM_FILL_ZERO 0u

/* The ANE and LXA constant three real SALLYs show, the default profile's
   (docs/hardware/sally.md, Unstable opcodes). */
#define MUHTARI__ANE_LXA_SALLY 0x00u

/* The machine profile: what differs between consoles of one model.
   ane_lxa_constant is the value ANE and LXA OR into A (docs/hardware/sally.md,
   Unstable opcodes); riot_orb_power_on is the RIOT's port B output register
   at power-on; ram_fill is a MUHTARI__RAM_FILL_* pattern; tia_phase is the
   TIA line's offset at power-on, in ticks. */
struct muhtari__profile {
    uint8_t ane_lxa_constant;
    uint8_t riot_orb_power_on;
    uint8_t ram_fill;
    uint16_t tia_phase;
};

struct muhtari {
    muhtari_allocator allocator; /* the caller's, copied at create */
    uint32_t audio_rem;          /* ticks carried into the next frame's sample count */
    struct muhtari__cpu cpu;
    struct muhtari__bus bus;
    struct muhtari__profile profile;
    uint8_t ram[MUHTARI__RAM_SIZE]; /* $1800-$27FF */
};

/* The size of each struct as first released: the end of its last field. A
   caller's size tag below this is rejected. */
#define MUHTARI__CONFIG_SIZE_FIRST                                                                 \
    ((uint32_t)(offsetof(muhtari_config, allocator) + sizeof(muhtari_allocator)))
#define MUHTARI__VERSIONS_SIZE_FIRST                                                               \
    ((uint32_t)(offsetof(muhtari_versions, behaviour_revision) + sizeof(uint32_t)))
#define MUHTARI__INFO_SIZE_FIRST                                                                   \
    ((uint32_t)(offsetof(muhtari_info, max_audio_samples) + sizeof(uint32_t)))
#define MUHTARI__FRAME_SIZE_FIRST                                                                  \
    ((uint32_t)(offsetof(muhtari_frame, audio_count) + sizeof(uint32_t)))

/* The public layouts, pinned on every target. P is the pointer size; every
   field sits where the previous one ends, except that muhtari_config's
   allocator starts at P, so 64-bit targets carry 4 alignment bytes after
   its size field. */
#define MUHTARI__P sizeof(void *)
_Static_assert(offsetof(muhtari_allocator, alloc) == 0, "allocator.alloc");
_Static_assert(offsetof(muhtari_allocator, free) == MUHTARI__P, "allocator.free");
_Static_assert(offsetof(muhtari_allocator, user) == 2 * MUHTARI__P, "allocator.user");
_Static_assert(sizeof(muhtari_allocator) == 3 * MUHTARI__P, "allocator size");

_Static_assert(offsetof(muhtari_config, size) == 0, "config.size");
_Static_assert(offsetof(muhtari_config, allocator) == MUHTARI__P, "config.allocator");
_Static_assert(sizeof(muhtari_config) == 4 * MUHTARI__P, "config size");

_Static_assert(offsetof(muhtari_versions, size) == 0, "versions.size");
_Static_assert(offsetof(muhtari_versions, library_major) == 4, "versions.library_major");
_Static_assert(offsetof(muhtari_versions, library_minor) == 8, "versions.library_minor");
_Static_assert(offsetof(muhtari_versions, library_patch) == 12, "versions.library_patch");
_Static_assert(offsetof(muhtari_versions, api) == 16, "versions.api");
_Static_assert(offsetof(muhtari_versions, abi) == 20, "versions.abi");
_Static_assert(offsetof(muhtari_versions, behaviour_revision) == 24, "versions.behaviour_revision");
_Static_assert(sizeof(muhtari_versions) == 28, "versions size");

_Static_assert(offsetof(muhtari_info, size) == 0, "info.size");
_Static_assert(offsetof(muhtari_info, width) == 4, "info.width");
_Static_assert(offsetof(muhtari_info, height) == 8, "info.height");
_Static_assert(offsetof(muhtari_info, fps_num) == 12, "info.fps_num");
_Static_assert(offsetof(muhtari_info, fps_den) == 16, "info.fps_den");
_Static_assert(offsetof(muhtari_info, par_num) == 20, "info.par_num");
_Static_assert(offsetof(muhtari_info, par_den) == 24, "info.par_den");
_Static_assert(offsetof(muhtari_info, sample_rate_num) == 28, "info.sample_rate_num");
_Static_assert(offsetof(muhtari_info, sample_rate_den) == 32, "info.sample_rate_den");
_Static_assert(offsetof(muhtari_info, max_audio_samples) == 36, "info.max_audio_samples");
_Static_assert(sizeof(muhtari_info) == 40, "info size");

_Static_assert(offsetof(muhtari_frame, size) == 0, "frame.size");
_Static_assert(offsetof(muhtari_frame, video_pitch) == 4, "frame.video_pitch");
_Static_assert(offsetof(muhtari_frame, video) == 8, "frame.video");
_Static_assert(offsetof(muhtari_frame, audio) == 8 + MUHTARI__P, "frame.audio");
_Static_assert(offsetof(muhtari_frame, audio_capacity) == 8 + 2 * MUHTARI__P,
               "frame.audio_capacity");
_Static_assert(offsetof(muhtari_frame, audio_count) == 12 + 2 * MUHTARI__P, "frame.audio_count");
_Static_assert(sizeof(muhtari_frame) == 16 + 2 * MUHTARI__P, "frame size");

/* Returns OK when the size tag at s is at least first and every byte past
   ours is zero; ERR_STRUCT_SIZE otherwise. */
muhtari_status muhtari__check_size(const void *s, uint32_t first, uint32_t ours);

/* The test card's native colour at (x, y) in a frame h rows tall. */
uint8_t muhtari__testcard_pixel(uint32_t x, uint32_t y, uint32_t h);

/* One CPU bus cycle: a read or a write at addr. The CPU makes every access
   through these two; the CPU test programs define their own. */
uint8_t muhtari__bus_read(muhtari *m, uint16_t addr);
void muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v);

/* The TIA as the bus sees it (src/tia.c): the byte a read of addr returns,
   its undriven bits from the open-bus byte, and a write whose data reaches
   the TIA at data_tick. */
uint8_t muhtari__tia_read(const muhtari *m, uint16_t addr);
void muhtari__tia_write(muhtari *m, uint16_t addr, uint64_t data_tick);

/* Runs exactly one instruction. */
void muhtari__cpu_step(muhtari *m);

/* Puts the CPU, the bus and RAM in their power-on state under m->profile. */
void muhtari__power_on(muhtari *m);

#endif /* MUHTARI_INTERNAL_H */
