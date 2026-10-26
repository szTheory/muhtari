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

/* Selected console timing; one scheduler and one sample lattice per machine
   (docs/hardware/bus.md, audio.md). Zero/no-content selects NTSC. */
struct muhtari__timing {
    uint32_t lines, height, frame_ticks, vblank;
    uint32_t sample_num, sample_den, max_samples;
};
static inline struct muhtari__timing muhtari__timing_for_region(uint32_t region)
{
    if (region == MUHTARI_REGION_PAL) {
        const struct muhtari__timing pal = {313u, 292u, 142102u, 139874u, 4000u, 591149u, 962u};
        return pal;
    }
    const struct muhtari__timing ntsc = {263u, 242u, 119402u, 117174u, 88u, 13125u, 801u};
    return ntsc;
}

/* Compact support and the instruction-boundary overshoot proof are in
   docs/hardware/audio.md. A held access clears RDY and at most two DMA
   lines before its six-tick bus cycle; an opcode takes at most eight
   accesses, followed by at most seven interrupt-entry accesses. */
#define MUHTARI__SYNTH_TAPS 16u
#define MUHTARI__SYNTH_PHASES 64u
#define MUHTARI__SYNTH_Q23 INT64_C(8388608)
#define MUHTARI__SYNTH_Q30 INT64_C(1073741824)
#define MUHTARI__SYNTH_MAX_LEVEL 18360
#define MUHTARI__SYNTH_RDY_TICKS 456u
#define MUHTARI__SYNTH_DMA_TICKS (2u * 454u)
#define MUHTARI__SYNTH_HOLD_ROUND_TICKS 12u
#define MUHTARI__SYNTH_ACCESS_TICKS 6u
#define MUHTARI__SYNTH_OPCODE_CYCLES 8u
#define MUHTARI__SYNTH_INTERRUPT_CYCLES 7u
#define MUHTARI__SYNTH_OVERSHOOT_TICKS                                                             \
    ((MUHTARI__SYNTH_RDY_TICKS + MUHTARI__SYNTH_DMA_TICKS + MUHTARI__SYNTH_HOLD_ROUND_TICKS +      \
      MUHTARI__SYNTH_ACCESS_TICKS) *                                                               \
     (MUHTARI__SYNTH_OPCODE_CYCLES + MUHTARI__SYNTH_INTERRUPT_CYCLES))
#define MUHTARI__SYNTH_CARRY_SAMPLES ((MUHTARI__SYNTH_OVERSHOOT_TICKS * 88u + 13124u) / 13125u)
#define MUHTARI__SYNTH_PCM_CAPACITY 1103u

/* All synthesis history is owned by the machine. pcm is a FIFO: at most
   962 current-frame samples plus 141 bounded PAL instruction-carry samples.
   horizon is the logical half-open output end, tick the hardware catch-up
   position; a pending mixed tick coalesces simultaneous source writes. */
struct muhtari__synth {
    uint64_t tick, origin, horizon;
    uint64_t pending_tick;
    int64_t impulse[16];
    int64_t integrator_q23, previous_q30, blocker_q30;
    uint64_t pre_clip_peak, clipped_samples;
    uint32_t phase, pcm_read, pcm_count;
    uint32_t sample_num, sample_den, max_samples;
    uint32_t kernel_adds, kernel_adds_per_sample, carry_high_water;
    int32_t mixed_level;
    uint16_t raw[10];
    uint8_t head, pending, pokeys, secondary;
    int16_t pcm[MUHTARI__SYNTH_PCM_CAPACITY];
};

/* Independent TIA horizontal/audio state; docs/hardware/tia.md. */
struct muhtari__tia_channel {
    uint8_t audc, audf, audv;
    uint8_t count, enabled, output;
    uint8_t noise, pulse, noise_input, pulse_gate;
};
struct muhtari__tia {
    uint64_t next_tick, epoch_tick;
    uint16_t epoch_phase;
    uint8_t phase, initialized;
    uint8_t input_latch_enable, input_dump, input_latch[2];
    struct muhtari__tia_channel channel[2];
};

/* Owned timer/audio state; docs/hardware/pokey.md. Chip clocks count real
   CLK2 edges independently of the bus's completed-cycle counter. */
struct muhtari__pokey_channel {
    uint64_t reload, fire, last_fire;
    uint32_t fires;
    uint16_t count;
    uint8_t audf, audc, output, hp, starting;
};
struct muhtari__pokey {
    uint64_t cycle, tick, half;
    uint64_t carry[2], resync[2];
    uint32_t poly17;
    uint8_t poly4, poly5, noise[3];
    uint16_t prescale28, prescale114;
    uint8_t audctl, skctl, irqen, irqst, initialized;
    uint8_t hp_delay[2], hp_capture[2];
    uint8_t irq_enable[3], irq_signal; /* section 5.7 propagation, pokey.md */
    struct muhtari__pokey_channel channel[4];
};
void muhtari__pokey_reset(muhtari *m, unsigned chip);
void muhtari__pokey_write(muhtari *m, unsigned chip, uint8_t reg, uint8_t v, uint64_t tick);
uint8_t muhtari__pokey_read(muhtari *m, unsigned chip, uint8_t reg, uint64_t tick);
int muhtari__pokey_irq(const muhtari *m, unsigned chip);
void muhtari__pokey_catch_up(muhtari *m, uint64_t tick);
void muhtari__pokey_half_edge(muhtari *m, uint64_t tick, int falling);

extern const int32_t muhtari__synth_kernel[MUHTARI__SYNTH_PHASES][16];
void muhtari__synth_reset(muhtari *m, uint64_t origin, uint8_t pokeys);
void muhtari__synth_level(muhtari *m, uint64_t tick, unsigned source, uint16_t level);
uint32_t muhtari__synth_render(muhtari *m, uint64_t end, int16_t *out);
void muhtari__synth_coefficients(uint32_t phase, int32_t out[16]);
void muhtari__synth_coefficients_for_period(uint32_t phase, uint32_t period, int32_t out[16]);
int64_t muhtari__synth_round(int64_t value, int64_t divisor);

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
    uint8_t irq_sampled; /* level and I sampled at the deciding cycle */
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
   nmi_at is a raised NMI edge not yet latched, while nmi_pending is set.
   cycle_len is the length in ticks of the cycle in flight, 4 or 6. sync
   is set by the CPU for the read that fetches an opcode, SALLY's SYNC
   cycle, and cleared by that read. */
struct muhtari__bus {
    /* Actual clock-segment origin for cartridge audio. Fast boot skips
       CPU execution; it does not invent phi2 cycles (embedding.md). */
    uint64_t clock_origin_tick, clock_origin_phi2;
    uint64_t phi2;
    uint64_t nmi_phi2;
    uint64_t ticks;
    uint64_t next_deadline;
    uint64_t rdy_release;
    uint64_t halt_at;
    uint64_t halt_until;
    uint64_t halt_fall;
    /* Electrical MARIA interval, independent of the scheduling stage:
       second actual phi2 fall through HALTRST (hardware/bus.md). */
    uint64_t maria_from;
    uint64_t maria_until;
    uint64_t nmi_at;
    uint8_t open_bus;
    uint8_t inptctrl;
    uint8_t halt_gate_writes;
    uint8_t halt_stage;
    uint8_t halt_cycles;
    uint8_t nmi_pending;
    uint8_t cycle_len;
    uint8_t sync;
    uint8_t irq_pipe;  /* synchronization advances on unheld CPU cycles */
    uint8_t irq_level; /* effective wired cartridge level at completed cycle */
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
    uint8_t pokey_init;
};

/* The loaded cartridge: rom holds the info.rom_size bytes the board maps,
   followed by its info.ram_size bytes of RAM, in one allocation through the
   instance's allocator; rom is NULL while none is loaded, ram while the
   board has none. bank is the SuperGame bank register and ram_bank the
   banked-RAM half at $4000, each already reduced to a bank that exists
   (docs/hardware/cartridge.md, Boards). */
struct muhtari__cart {
    uint8_t *rom;
    uint8_t *ram;
    size_t allocation_bytes;
    struct {
        uint8_t *ram;
        uint32_t kind;
        uint8_t control;
    } expansion;
    uint8_t bank;
    uint8_t ram_bank;
    uint8_t irq_mask;
    uint16_t pokey_at[2];
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
    struct muhtari__line_stats stats[313];
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

/* The controller ports as the host sets them (docs/embedding.md, Input).
   device, buttons and switches are the input applied to the machine, each
   device as the host asked, 0 for the cartridge's; the pending_ fields are
   the last input set, waiting for the next VBLANK start, while pending is
   set. Pending input is not machine state. input_read records a CPU read
   of SWCHA, SWCHB or INPT0-5 in the current frame. */
struct muhtari__motion {
    uint64_t next_tick;
    int32_t residual;
    uint8_t phase, negative;
};

struct muhtari__light {
    uint64_t sample_tick, response_tick, response_until, low_until;
    uint16_t sample_raster, sample_x;
    uint8_t delay, width, lines, threshold, low;
};

struct muhtari__pot {
    uint64_t action, last_tick, crossing_tick;
    uint32_t duration;
};

struct muhtari__adapter {
    uint32_t snapshot;
    uint8_t host, position, scanning;
};

struct muhtari__ports {
    struct muhtari__motion motion[2][2];
    struct muhtari__light light[2];
    struct muhtari__pot pot[2][2];
    struct muhtari__adapter adapter[2];
    uint16_t keypad_keys[2];
    uint8_t keypad_low[2];
    uint64_t next_tick; /* zero when no controller deadline is armed */
    uint32_t device[2];
    uint32_t buttons[2];
    uint32_t switches;
    uint32_t pending_device[2];
    uint32_t pending_buttons[2];
    uint32_t pending_switches;
    uint32_t load_device[2];
    muhtari_input_ex pending_ex;
    muhtari_input_ex applied_ex;
    uint8_t input_mode;  /* 0 legacy, 1 extended pending */
    uint8_t initialized; /* an extended packet has activated initial pins */
    uint8_t pending;
    uint8_t input_read;
    uint64_t io_hazard; /* frames a 2600 stick's fire shorted a driven pin */
};

/* The largest BIOS image: 16 KiB at $C000-$FFFF (A7800-SYSTEM section 7). */
#define MUHTARI__BIOS_SIZE_MAX 16384u

/* The boot (docs/embedding.md, Booting with a BIOS). handoff_frame is the
   frame, counted from 1 after a load or power cycle, of the first opcode
   fetch from cartridge space after INPTCTRL's EXT went from 0 to 1, and 0
   until it comes; the fast boot hands off in frame 1. rejected is set when
   the BIOS takes its 2600 path, INPTCTRL locking with MEN = 0 before the
   hand-off, and stays set until the next load or power cycle. */
struct muhtari__boot {
    uint64_t handoff_frame;
    uint8_t rejected;
};

/* With content loaded, each muhtari_run_frame runs from frame_start to
   frame_deadline, both VBLANK-start ticks except frame_start after the
   hand-off (docs/hardware/bus.md, The run loop). bios holds the bios_size
   bytes muhtari_load copied, 0 for fast boot. */
typedef void (*muhtari__audio_observer)(void *user, uint64_t tick, uint8_t source, uint16_t raw);
struct muhtari__observation;
muhtari_status muhtari__synth_observe(muhtari *m, muhtari__audio_observer callback, void *user);
void muhtari__synth_raw(muhtari *m, uint64_t tick, unsigned source, uint8_t raw);
void muhtari__observe_begin(muhtari *m, uint64_t end);
void muhtari__observe_end(muhtari *m);

struct muhtari__battery {
    uint8_t data[34816];
    uint8_t rom[4096];
    size_t bytes;
    uint64_t generation;
    uint32_t hsc_attached;
    uint32_t eeprom_port;
    uint32_t eeprom_device;
    uint32_t firmware_flags;
    uint8_t hsc_sha1[20];
};

/* One 24LC256 page and pin protocol; persistent bytes live only in battery
   (docs/hardware/controllers.md, Serial EEPROM). The selected 5 ms model
   rounds ceil(315000000 / 8800) half-crystal ticks upward. */
#define MUHTARI__SAVEKEY_BUSY_TICKS 35796u
struct muhtari__savekey {
    uint64_t busy_until;
    uint16_t address, page_base;
    uint8_t host_scl, host_sda, sda_low;
    uint8_t phase, next_phase, bits, shift, address_high, read_byte;
    uint8_t page[64], touched[64], page_offset, pending, ack;
};

struct muhtari {
    muhtari__audio_observer observer;
    void *observer_user;
    struct muhtari__observation *observation;
    muhtari_allocator allocator; /* the caller's, copied at create */
    uint32_t audio_rem;          /* ticks carried into the next frame's sample count */
    uint32_t effective_region;   /* committed with successful content installation */
    uint8_t loaded;              /* content is loaded and handed off */
    uint64_t frame_start;
    uint64_t frame_deadline;
    struct muhtari__cpu cpu;
    struct muhtari__bus bus;
    struct muhtari__riot riot;
    struct muhtari__maria maria;
    struct muhtari__profile profile;
    struct muhtari__cart cart;
    struct muhtari__battery battery;
    struct muhtari__savekey savekey;
    uint8_t hsc_load_rom[4096]; /* unpublished load staging; never a hardware/save view */
    uint8_t bios_load_rom[MUHTARI__BIOS_SIZE_MAX]; /* same lifetime as HSC load staging */
    struct muhtari__ports ports;
    struct muhtari__boot boot;
    struct muhtari__synth synth;
    struct muhtari__tia tia;
    struct muhtari__pokey pokey[2];
    int16_t audio_work[962];
    uint64_t frames;                /* frames run since the last load or power cycle */
    uint64_t lag_frames;            /* of those, frames with no CPU read of an input */
    uint8_t ram[MUHTARI__RAM_SIZE]; /* $1800-$27FF */
    uint32_t bios_size;
    uint8_t bios[16384];
};
_Static_assert(sizeof(((muhtari *)0)->bios) == MUHTARI__BIOS_SIZE_MAX, "bios array");

static inline struct muhtari__timing muhtari__timing(const muhtari *m)
{
    return muhtari__timing_for_region(m->effective_region);
}

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
#define MUHTARI__MEDIA_SIZE_FIRST ((uint32_t)(offsetof(muhtari_media, bios_size) + sizeof(size_t)))
#define MUHTARI__BATTERY_SIZE_FIRST ((uint32_t)(offsetof(muhtari_battery_info, hsc_sha1) + 20u))
#define MUHTARI__CONST_SPAN_SIZE_FIRST                                                             \
    ((uint32_t)(offsetof(muhtari_const_span, bytes) + sizeof(size_t)))
#define MUHTARI__INPUT_SIZE_FIRST ((uint32_t)(offsetof(muhtari_input, switches) + sizeof(uint32_t)))
#define MUHTARI__INPUT_EX_SIZE_FIRST 4u
#define MUHTARI__SPAN_SIZE_FIRST ((uint32_t)(offsetof(muhtari_span, bytes) + sizeof(size_t)))
#define MUHTARI__COUNTERS_SIZE_FIRST                                                               \
    ((uint32_t)(offsetof(muhtari_counters, lag_frames) + sizeof(uint64_t)))

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
_Static_assert(sizeof(muhtari_info) == 44, "info size");

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

/* muhtari_media: two uint32_t fields, then two pointer and size pairs; a
   size_t is pointer sized on every target. */
_Static_assert(sizeof(size_t) == MUHTARI__P, "size_t is pointer sized");
_Static_assert(offsetof(muhtari_media, size) == 0, "media.size");
_Static_assert(offsetof(muhtari_media, region) == 4, "media.region");
_Static_assert(offsetof(muhtari_media, image) == 8, "media.image");
_Static_assert(offsetof(muhtari_media, image_size) == 8 + MUHTARI__P, "media.image_size");
_Static_assert(offsetof(muhtari_media, bios) == 8 + 2 * MUHTARI__P, "media.bios");
_Static_assert(offsetof(muhtari_media, bios_size) == 8 + 3 * MUHTARI__P, "media.bios_size");
_Static_assert(offsetof(muhtari_media, hsc_mode) == 8 + 4 * MUHTARI__P, "media.hsc_mode");
_Static_assert(offsetof(muhtari_media, hsc_rom) == 8 + 5 * MUHTARI__P, "media.hsc_rom");
_Static_assert(offsetof(muhtari_media, hsc_rom_size) == 8 + 6 * MUHTARI__P, "media.hsc_rom_size");
_Static_assert(offsetof(muhtari_media, port) == 8 + 7 * MUHTARI__P, "media.port");
_Static_assert(offsetof(muhtari_media, expansion) == 16 + 7 * MUHTARI__P, "media.expansion");
_Static_assert(sizeof(muhtari_media) == 16 + 8 * MUHTARI__P, "media size");

/* muhtari_input is six uint32_t fields: 24 bytes on every target. */
_Static_assert(offsetof(muhtari_input, size) == 0, "input.size");
_Static_assert(offsetof(muhtari_input, device) == 4, "input.device");
_Static_assert(offsetof(muhtari_input, buttons) == 12, "input.buttons");
_Static_assert(offsetof(muhtari_input, switches) == 20, "input.switches");
_Static_assert(sizeof(muhtari_input) == 24, "input size");
_Static_assert(sizeof(muhtari_port_input) == 40, "port input size");
_Static_assert(offsetof(muhtari_input_ex, port) == 4, "extended input ports");
_Static_assert(offsetof(muhtari_input_ex, switches) == 84, "extended input switches");
_Static_assert(sizeof(muhtari_input_ex) == 88, "extended input size");

/* muhtari_span: the size, then a pointer and a size_t from P on, so 64-bit
   targets carry 4 alignment bytes after the size field. */
_Static_assert(offsetof(muhtari_span, size) == 0, "span.size");
_Static_assert(offsetof(muhtari_span, data) == MUHTARI__P, "span.data");
_Static_assert(offsetof(muhtari_span, bytes) == 2 * MUHTARI__P, "span.bytes");
_Static_assert(sizeof(muhtari_span) == 3 * MUHTARI__P, "span size");

/* muhtari_counters: the size, 4 alignment bytes, then two uint64_t fields,
   which every target aligns to 8: 24 bytes. */
_Static_assert(offsetof(muhtari_counters, size) == 0, "counters.size");
_Static_assert(offsetof(muhtari_counters, frames) == 8, "counters.frames");
_Static_assert(offsetof(muhtari_counters, lag_frames) == 16, "counters.lag_frames");
_Static_assert(sizeof(muhtari_counters) == 24, "counters size");

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
   rom_size + ram_size bytes, the RAM after the ROM and zeroed, replacing
   any cartridge loaded before; ERR_NO_MEMORY
   leaves m unchanged. muhtari_destroy frees it (docs/hardware/cartridge.md,
   Loading). */
muhtari_status muhtari__cart_load(muhtari *m, const void *image, size_t n);

/* The second half of muhtari__cart_load: copies the ROM of image, which
   *info identifies, into one new allocation with its RAM and replaces any
   cartridge loaded before. ERR_NO_MEMORY leaves m unchanged. */
muhtari_status muhtari__cart_install(muhtari *m, const void *image, const muhtari_cart_info *info,
                                     uint32_t expansion);

/* The cartridge as the bus sees it, by board (docs/hardware/bus.md, Memory
   map). read returns the byte the board drives at addr, or the open-bus
   byte where it drives none; by_maria is 1 for MARIA's DMA fetches. write
   is a CPU write into cartridge space. peek reads like read with no side
   effect, for the hand-off and the tests. reset puts the bank registers
   at power-on and fills the RAM by ram_fill. */
void muhtari__cart_reset(muhtari *m);
uint8_t muhtari__cart_read(muhtari *m, uint16_t addr, int by_maria);
uint8_t muhtari__cart_dma_read(muhtari *m, uint16_t addr, uint64_t tick);
void muhtari__cart_write(muhtari *m, uint16_t addr, uint8_t v);
uint8_t muhtari__cart_peek(const muhtari *m, uint16_t addr);

/* The fast-boot hand-off: puts the machine where the NTSC BIOS leaves it as
   it jumps to the cartridge, with no BIOS run (A7800-SYSTEM section 7, C10;
   docs/hardware/bus.md, The run loop). Needs a loaded cartridge. */
void muhtari__boot_fast(muhtari *m);

/* The BIOS boot: the power-on state, the beam at line 0, t 0 with MARIA's
   CTRL $00, frame 1 running to the first VBLANK start 117,174 ticks away,
   then RES, which takes PC from the BIOS's reset vector (A7800-SYSTEM
   sections 7 and 8; docs/embedding.md, Booting with a BIOS). Needs a
   loaded cartridge and BIOS. */
void muhtari__boot_bios(muhtari *m);

/* Whether the BIOS answers a read of addr: while EXT = 0, every A15 = 1
   address with MEN = 1, and every A12 = 1 address with MEN = 0, as the
   2600 decode sees it (docs/hardware/bus.md, Memory map). byte is the
   BIOS byte there: the image mirrored over $8000-$FFFF with MEN = 1, its
   top 4 KiB with MEN = 0. */
int muhtari__boot_bios_maps(const muhtari *m, uint16_t addr);
uint8_t muhtari__boot_bios_byte(const muhtari *m, uint16_t addr);

/* The hand-off frame K, 0 until the hand-off; see struct muhtari__boot.
   Not public: the runner reads it as a diagnostic. */
uint64_t muhtari__boot_handoff_frame(const muhtari *m);

/* SALLY's RES sequence, seven cycles: two reads at PC, three stack reads
   that step S down by 3, then PC from $FFFC/$FFFD, with I set
   (docs/hardware/sally.md, Reset). */
void muhtari__cpu_reset(muhtari *m);

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
void muhtari__tia_write(muhtari *m, uint16_t addr, uint8_t v, uint64_t data_tick);
void muhtari__tia_reset(muhtari *m);
void muhtari__tia_catch_up(muhtari *m, uint64_t tick);
void muhtari__tia_pins_changed(muhtari *m);

/* The RIOT as the bus sees it (src/riot.c): A9 selects its ports and timer
   or its RAM, and A0-A6 the register or byte (docs/hardware/riot.md).
   reset puts it in its RES state under m->profile. */
uint8_t muhtari__riot_read(muhtari *m, uint16_t addr);
void muhtari__riot_write(muhtari *m, uint16_t addr, uint8_t v, uint64_t tick);
void muhtari__savekey_pins(muhtari *m, uint64_t tick);
void muhtari__savekey_catch_up(muhtari *m, uint64_t tick);
void muhtari__savekey_reset(muhtari *m);
void muhtari__riot_reset(muhtari *m);
/* Runs PA7's edge detector after the device pins change. */
void muhtari__riot_pins_changed(muhtari *m);

/* The entry points MARIA drives (docs/hardware/bus.md, HALT and NMI).
   request_halt asks for HALT from at_tick until until_tick; with until_tick
   MUHTARI__HALT_UNTIL_MARIA the window's end is the HALTRST tick that
   muhtari__maria_burst returns when the window reaches c2. raise_nmi raises
   an NMI edge at at_tick, and does nothing while MEN = 0. Each takes one at
   a time, and the one in flight stands. raise_nmi returns 1 when the edge
   is queued for latching and 0 when it is ignored; a queued edge can still
   be dropped by the latch, when an earlier latched edge has not been taken
   or MEN was cleared before it latched. MARIA keeps the return as the
   line's MUHTARI__LINE_DLI_QUEUED flag. request_halt returns 1 when it
   takes the request and 0 when it ignores it, for one of two reasons: the
   gate is shut, because it has not yet counted two writes, or a window is
   already in flight, waiting for /HALT to fall or with /HALT low. MARIA
   reads a 0 as no burst this line and sets dma_blocked, the line's
   MUHTARI__LINE_DMA_BLOCKED flag. */
#define MUHTARI__HALT_UNTIL_MARIA UINT64_MAX
int muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick);
int muhtari__bus_maria_selected(const muhtari *m, uint64_t tick);
int muhtari__bus_raise_nmi(muhtari *m, uint64_t at_tick);

/* The tick of SALLY's n-th phi1 after tick t, n >= 1: its cycle starts,
   from the end of the cycle in flight on, every 4 ticks
   (docs/hardware/maria.md, The DLI). */
uint64_t muhtari__bus_phi1_after(const muhtari *m, uint64_t t, uint32_t n);

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
void muhtari__maria_flush(muhtari *m, uint64_t tick);

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

/* The byte a CPU read of addr would return, with no side effect: no timer
   flag cleared, no cycle run, no input read recorded. For the tests. */
uint8_t muhtari__bus_peek(const muhtari *m, uint16_t addr);
uint8_t muhtari__riot_peek(const muhtari *m, uint16_t addr);

/* Memory as the CPU would read it at addr under the current decode: system
   RAM, the RIOT's RAM and cartridge space, and the open-bus byte for every
   other address, so no device register is read. It changes no state. For
   the diagnostic driver, which reads the mailbox in RAM. */
uint8_t muhtari__peek(const muhtari *m, uint16_t addr);

/* Applies the pending input, if any, at a VBLANK start (src/ports.c;
   docs/embedding.md, Input). */
void muhtari__ports_latch(muhtari *m);
void muhtari__ports_reset(muhtari *m);
void muhtari__ports_dump(muhtari *m, uint64_t tick);
void muhtari__ports_rows(muhtari *m, uint64_t tick);
void muhtari__ports_host_pins(muhtari *m, uint64_t tick);
uint64_t muhtari__ports_next_event(const muhtari *m);
void muhtari__ports_catch_up(muhtari *m, uint64_t tick);
void muhtari__ports_light_sample(muhtari *m, uint32_t raster, uint32_t x, uint64_t tick,
                                 uint8_t color, int visible);
void muhtari__bus_rearm(muhtari *m);
muhtari_status muhtari__ports_check_device_ex(uint32_t device);

/* The device wired to port 0 or 1: MUHTARI_PORT_PROLINE, _JOYSTICK or
   _UNPLUGGED, the host's choice or else the cartridge's, where a header
   device this version does not model is wired as a ProLine. */
uint32_t muhtari__ports_device(const muhtari *m, uint32_t port);

/* The pins the devices and switches drive (src/ports.c): SWCHA's and
   SWCHB's input pins, and INPT0-INPT5's level, 0 or 1, for n 0 to 5. */
uint8_t muhtari__ports_swcha(const muhtari *m);
uint8_t muhtari__ports_swchb(const muhtari *m);
uint8_t muhtari__ports_inpt(const muhtari *m, uint32_t n);

/* Counts a frame in which a 2600 stick's fire is held in two-button mode. */
void muhtari__ports_count_hazard(muhtari *m);

/* Starts the counters and the lag flag from zero, at load and power cycle. */
void muhtari__ports_reset_counters(muhtari *m);

#endif /* MUHTARI_INTERNAL_H */
