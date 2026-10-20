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

/* The beam: a line is 454 ticks, and a frame's VBLANK starts at line 258,
   t 42, the boundary between one muhtari_run_frame and the next
   (A7800-SYSTEM section 2; docs/hardware/bus.md, The run loop). */
#define MUHTARI__LINE_TICKS 454u
#define MUHTARI__VBLANK_START (258u * MUHTARI__LINE_TICKS + 42u)

/* The bounds of muhtari_frame.video_pitch. The cap keeps row offsets inside a
   32-bit size_t. */
#define MUHTARI__PITCH_MIN 320u
#define MUHTARI__PITCH_MAX 65536u

/* SALLY's registers. P is held raw, as the stack and the vectors see it.
   jammed is set by a JAM opcode; the CPU then runs no more instructions,
   and each step makes one read of $FFFF.
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
   tick from which a held read may run, 0 while RDY is high. A HALT request
   waits in halt_at and halt_until until /HALT falls at halt_fall, and
   halt_cycles counts the cycles run since the fall, through halt_stage.
   nmi_at is a raised NMI edge not yet latched, while nmi_pending is set. */
struct muhtari__bus {
    uint64_t phi2;
    uint64_t nmi_phi2;
    uint64_t ticks;
    uint64_t next_deadline;
    uint64_t rdy_release;
    uint64_t halt_at;
    uint64_t halt_until;
    uint64_t halt_fall;
    uint64_t nmi_at;
    uint8_t open_bus;
    uint8_t inptctrl;
    uint8_t halt_gate_writes;
    uint8_t halt_stage;
    uint8_t halt_cycles;
    uint8_t nmi_pending;
};

/* The 6532's RAM: 128 bytes (docs/hardware/riot.md, RAM). */
#define MUHTARI__RIOT_RAM_SIZE 128u

/* The RIOT's state (docs/hardware/riot.md). The timer is not counted: it is
   the value timer_n written in the cycle whose phi2 count is timer_phi2, on
   an interval of 1 << timer_shift cycles, and every read computes INTIM from
   the cycles since. timer_cleared records a timer access that cleared the
   flag after it set. pa7_flag is PA7's edge flag, pa7_positive the active
   edge, and pa7_detect the edge detector's input as last seen. */
struct muhtari__riot {
    uint64_t timer_phi2;
    uint16_t timer_n;
    uint8_t timer_shift;
    uint8_t timer_cleared;
    uint8_t ora, ddra, orb, ddrb;
    uint8_t pa7_flag;
    uint8_t pa7_positive;
    uint8_t pa7_detect;
    uint8_t ram[MUHTARI__RIOT_RAM_SIZE];
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

/* The loaded cartridge: rom holds the info.rom_size bytes the board maps,
   allocated through the instance's allocator; NULL while none is loaded. */
struct muhtari__cart {
    uint8_t *rom;
    muhtari_cart_info info;
};

/* One line's DMA, as MARIA's burst ran it (docs/hardware/maria.md,
   Statistics). cost is HALTRST minus T0 in ticks; bytes counts the graphics,
   character map and character graphics bytes fetched; headers the object
   headers read whole; objects the objects whose every fetch ran. */
struct muhtari__line_stats {
    uint16_t cost;
    uint16_t bytes;
    uint8_t headers;
    uint8_t objects;
    uint16_t flags; /* MUHTARI__LINE_* */
};

/* The flags of a line's statistics. */
#define MUHTARI__LINE_LAST 0x0001u        /* the zone's last line: the next DLL entry was read */
#define MUHTARI__LINE_ABORTED 0x0002u     /* the burst reached LRC and stopped */
#define MUHTARI__LINE_DLI 0x0004u         /* the entry read on this line has DLI set */
#define MUHTARI__LINE_DLI_QUEUED 0x0008u  /* the bus took the DLI's NMI edge */
#define MUHTARI__LINE_DMA_BLOCKED 0x0010u /* the bus refused the line's HALT request */
#define MUHTARI__LINE_DMA_OFF 0x0020u     /* CTRL had DMA off, so no burst ran */

/* The sticky feature mask: a bit sets on the first line that uses the
   feature and stays set (docs/hardware/maria.md, Statistics). */
#define MUHTARI__FEAT_RM0 0x0001u
#define MUHTARI__FEAT_RM2 0x0002u
#define MUHTARI__FEAT_RM3 0x0004u
#define MUHTARI__FEAT_WRITE_MODE 0x0008u
#define MUHTARI__FEAT_KANGAROO 0x0010u
#define MUHTARI__FEAT_HOLEY8 0x0020u
#define MUHTARI__FEAT_HOLEY16 0x0040u
#define MUHTARI__FEAT_IND1 0x0080u
#define MUHTARI__FEAT_IND2 0x0100u
#define MUHTARI__FEAT_DLI 0x0200u
#define MUHTARI__FEAT_DLI_QUEUED 0x0400u
#define MUHTARI__FEAT_DMA_BLOCKED 0x0800u

/* An NTSC frame's lines (A7800-MARIA section 6). */
#define MUHTARI__LINES_NTSC 263u

/* Line RAM: 160 cells per buffer (A7800-MARIA section 3). */
#define MUHTARI__LINE_CELLS 160u

/* MARIA's state (docs/hardware/maria.md). reg holds the registers at
   $20-$3F, indexed by address minus $20. The display-list walk: dll is the
   next DLL entry's address, dl the zone's DL base, entry the zone's DLL byte
   0 (DLI, H16, H8) and offset the zone's OFFSET, which counts down to 0 on
   the zone's last line. line_ram holds two buffers of 160 cells, each cell
   the palette in bits 4-2 and the data pair in bits 1-0; the burst fills
   line_ram[write_buf]. write_mode is the latch only 5-byte headers load.
   stats is each line's statistics and features the sticky feature mask.
   The beam is counted from origin, the tick of line 0, t 0 of the current
   frame: the next line event is event_index of line event_line, and a
   requested burst is for line dma_line and stops at dma_lrc. mstat is
   MSTAT's bit 7, VBLANK. rows and pitch are the caller's video rows, set by
   muhtari_run_frame for its own duration only and NULL otherwise; px is
   the next pixel of the current line's raster still to render
   (docs/hardware/maria.md, Rendering). */
struct muhtari__maria {
    uint8_t reg[32];
    uint16_t dll;
    uint16_t dl;
    uint8_t entry;
    uint8_t offset;
    uint8_t write_mode;
    uint8_t write_buf;
    uint8_t line_ram[2][MUHTARI__LINE_CELLS];
    struct muhtari__line_stats stats[MUHTARI__LINES_NTSC];
    uint32_t features;
    uint64_t origin;
    uint64_t dma_lrc;
    uint16_t event_line;
    uint16_t dma_line;
    uint8_t event_index;
    uint8_t mstat;
    uint8_t *rows;
    uint32_t pitch;
    uint16_t px;
};

/* With content loaded, each muhtari_run_frame runs from frame_start to
   frame_deadline, both VBLANK-start ticks except frame_start after the
   hand-off (docs/hardware/bus.md, The run loop). */
struct muhtari {
    muhtari_allocator allocator; /* the caller's, copied at create */
    uint32_t audio_rem;          /* ticks carried into the next frame's sample count */
    uint8_t loaded;              /* content is loaded and handed off */
    uint64_t frame_start;
    uint64_t frame_deadline;
    struct muhtari__cpu cpu;
    struct muhtari__bus bus;
    struct muhtari__riot riot;
    struct muhtari__maria maria;
    struct muhtari__profile profile;
    struct muhtari__cart cart;
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
#define MUHTARI__CART_INFO_SIZE_FIRST                                                              \
    ((uint32_t)(offsetof(muhtari_cart_info, title) + sizeof(((muhtari_cart_info *)0)->title)))

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

/* muhtari_cart_info is 26 uint32_t fields and the title: 140 bytes on every
   target. */
_Static_assert(offsetof(muhtari_cart_info, size) == 0, "cart_info.size");
_Static_assert(offsetof(muhtari_cart_info, kind) == 4, "cart_info.kind");
_Static_assert(offsetof(muhtari_cart_info, source) == 8, "cart_info.source");
_Static_assert(offsetof(muhtari_cart_info, flags) == 12, "cart_info.flags");
_Static_assert(offsetof(muhtari_cart_info, warnings) == 16, "cart_info.warnings");
_Static_assert(offsetof(muhtari_cart_info, rom_size) == 20, "cart_info.rom_size");
_Static_assert(offsetof(muhtari_cart_info, board) == 24, "cart_info.board");
_Static_assert(offsetof(muhtari_cart_info, board_options) == 28, "cart_info.board_options");
_Static_assert(offsetof(muhtari_cart_info, ram_size) == 32, "cart_info.ram_size");
_Static_assert(offsetof(muhtari_cart_info, sound) == 36, "cart_info.sound");
_Static_assert(offsetof(muhtari_cart_info, devices) == 40, "cart_info.devices");
_Static_assert(offsetof(muhtari_cart_info, port1) == 44, "cart_info.port1");
_Static_assert(offsetof(muhtari_cart_info, port2) == 48, "cart_info.port2");
_Static_assert(offsetof(muhtari_cart_info, region) == 52, "cart_info.region");
_Static_assert(offsetof(muhtari_cart_info, a78_version) == 56, "cart_info.a78_version");
_Static_assert(offsetof(muhtari_cart_info, a78_rom_size) == 60, "cart_info.a78_rom_size");
_Static_assert(offsetof(muhtari_cart_info, a78_cart_type) == 64, "cart_info.a78_cart_type");
_Static_assert(offsetof(muhtari_cart_info, a78_port1) == 68, "cart_info.a78_port1");
_Static_assert(offsetof(muhtari_cart_info, a78_port2) == 72, "cart_info.a78_port2");
_Static_assert(offsetof(muhtari_cart_info, a78_tv) == 76, "cart_info.a78_tv");
_Static_assert(offsetof(muhtari_cart_info, a78_save) == 80, "cart_info.a78_save");
_Static_assert(offsetof(muhtari_cart_info, a78_xm) == 84, "cart_info.a78_xm");
_Static_assert(offsetof(muhtari_cart_info, a78_mapper) == 88, "cart_info.a78_mapper");
_Static_assert(offsetof(muhtari_cart_info, a78_mapper_options) == 92,
               "cart_info.a78_mapper_options");
_Static_assert(offsetof(muhtari_cart_info, a78_audio) == 96, "cart_info.a78_audio");
_Static_assert(offsetof(muhtari_cart_info, a78_irq) == 100, "cart_info.a78_irq");
_Static_assert(offsetof(muhtari_cart_info, title) == 104, "cart_info.title");
_Static_assert(sizeof(muhtari_cart_info) == 140, "cart_info size");

/* Returns OK when the size tag at s is at least first and every byte past
   ours is zero; ERR_STRUCT_SIZE otherwise. */
muhtari_status muhtari__check_size(const void *s, uint32_t first, uint32_t ours);

/* Writes min(caller size, ours) bytes of a fully built output struct whose
   own size field already holds the caller's value. */
void muhtari__write_out(void *out, const void *full, uint32_t ours);

/* Identifies the n bytes at img against the cartridge tables given, which
   muhtari_probe passes from src/cartdb.c and tests pass from their own list
   (docs/hardware/cartridge.md). Fills *out, size field included, and returns
   the status muhtari_probe returns; img may be NULL only when n is 0. */
struct muhtari__cartdb_key;
struct muhtari__cartdb_entry;
struct muhtari__cartdb_layout;
muhtari_status muhtari__cart_identify(const uint8_t *img, size_t n,
                                      const struct muhtari__cartdb_key *keys, uint32_t key_count,
                                      const struct muhtari__cartdb_entry *entries,
                                      uint32_t entry_count,
                                      const struct muhtari__cartdb_layout *layouts,
                                      uint32_t layout_count, const uint8_t (*prefix)[8],
                                      uint32_t prefix_count, muhtari_cart_info *out);

/* Loads the n bytes at image into m: the status is muhtari_probe's for the
   same bytes. Every refusal returns before anything is allocated and leaves
   m unchanged. On OK the mapped ROM is copied into one allocation of
   rom_size bytes, replacing any cartridge loaded before; ERR_NO_MEMORY
   leaves m unchanged. muhtari_destroy frees it (docs/hardware/cartridge.md,
   Loading). */
muhtari_status muhtari__cart_load(muhtari *m, const void *image, size_t n);

/* The cartridge as the bus sees it, by board (docs/hardware/bus.md, Memory
   map). read returns the byte the board drives at addr, or the open-bus
   byte where it drives none; by_maria is 1 for MARIA's DMA fetches. write
   is a CPU write into cartridge space. peek reads like read with no side
   effect, for the hand-off and the tests. */
uint8_t muhtari__cart_read(muhtari *m, uint16_t addr, int by_maria);
void muhtari__cart_write(muhtari *m, uint16_t addr, uint8_t v);
uint8_t muhtari__cart_peek(const muhtari *m, uint16_t addr);

/* The fast-boot hand-off: puts the machine where the NTSC BIOS leaves it as
   it jumps to the cartridge, with no BIOS run (A7800-SYSTEM section 7, C10;
   docs/hardware/bus.md, The run loop). Needs a loaded cartridge. */
void muhtari__boot_fast(muhtari *m);

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

/* The RIOT as the bus sees it (src/riot.c): A9 selects its ports and timer
   or its RAM, and A0-A6 the register or byte (docs/hardware/riot.md).
   reset puts it in its RES state under m->profile. */
uint8_t muhtari__riot_read(muhtari *m, uint16_t addr);
void muhtari__riot_write(muhtari *m, uint16_t addr, uint8_t v);
void muhtari__riot_reset(muhtari *m);

/* The entry points MARIA drives (docs/hardware/bus.md, HALT and NMI).
   request_halt asks for HALT from at_tick until until_tick; with until_tick
   MUHTARI__HALT_UNTIL_MARIA the window's end is the HALTRST tick that
   muhtari__maria_burst returns when the window reaches c2. raise_nmi raises
   an NMI edge at at_tick, and does nothing while MEN = 0. Each takes one at
   a time, and the one in flight stands. Each returns 1 when it takes the
   request and 0 when it ignores it. request_halt returns 0 for one of two
   reasons: the gate is shut, because it has not yet counted two writes, or
   a window is already in flight, waiting for /HALT to fall or with /HALT
   low. MARIA reads a 0 as no burst this line and sets dma_blocked, the
   line's MUHTARI__LINE_DMA_BLOCKED flag. */
#define MUHTARI__HALT_UNTIL_MARIA UINT64_MAX
int muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick);
int muhtari__bus_raise_nmi(muhtari *m, uint64_t at_tick);

/* MARIA's VBE fetch (docs/hardware/maria.md, Display lists): starting at
   tick t0 it loads the DLL pointer from DPPH and DPPL and reads zone 0's
   entry. Returns the tick at which it ends, t0 + 7. */
uint64_t muhtari__maria_vbe(muhtari *m, uint64_t t0);

/* MARIA's registers as the CPU sees them (docs/hardware/maria.md,
   Registers). read returns MSTAT's VBLANK bit over the open-bus byte at $28
   and the open-bus byte elsewhere. write, whose data reaches MARIA at
   data_tick, first renders the pixels played up to that tick, then stores
   the register; a write to $24, MARIA's WSYNC, holds RDY low until the next
   line's t 0. */
uint8_t muhtari__maria_read(const muhtari *m, uint16_t addr);
void muhtari__maria_write(muhtari *m, uint16_t addr, uint8_t v, uint64_t data_tick);

/* MARIA's line events (docs/hardware/maria.md, Line events). start arms
   them from the beam at the current tick and sets MSTAT to match; the
   power-on and the hand-off call it after placing the beam. next_event is
   the tick of the next one; event fires it and arms the one after. */
void muhtari__maria_start(muhtari *m);
uint64_t muhtari__maria_next_event(const muhtari *m);
void muhtari__maria_event(muhtari *m);

/* The burst the bus runs when MARIA's HALT window reaches c2: T0 is the
   /HALT fall plus 6, and the return is HALTRST (docs/hardware/maria.md, The
   HALT handshake). */
uint64_t muhtari__maria_burst(muhtari *m, uint64_t fall);

/* One line's DMA burst (docs/hardware/maria.md, DMA cost): the PLA starts
   at tick t0, every fetch at its own tick, and a fetch at or after lrc is
   dropped. Fills *st and stats[line] (when line is below 263), ORs the
   feature bits and returns the HALTRST tick. lrc is at most 454 ticks after
   t0. */
uint64_t muhtari__maria_dma_line(muhtari *m, uint64_t t0, uint64_t lrc, uint32_t line,
                                 struct muhtari__line_stats *st);

/* Runs exactly one instruction, then the NMI entry when one is due. On a
   jammed CPU it makes one read of $FFFF instead, and changes nothing else
   (docs/hardware/sally.md, JAM). */
void muhtari__cpu_step(muhtari *m);

/* Puts the CPU, the bus, RAM and the RIOT in their power-on state under
   m->profile, and arms MARIA's line events from the beam at that tick. */
void muhtari__power_on(muhtari *m);

#endif /* MUHTARI_INTERNAL_H */
