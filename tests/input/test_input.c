/* Controllers and console switches as a game reads them, one case per run:

     muhtari-test-input <case>
     muhtari-test-input --write-cart FILE [even] [--write-cart FILE [even]]...

   Each case builds a 48 KiB linear cartridge in memory from the 6502 bytes
   below, so no image is tracked. Every frame the cartridge waits for MSTAT's
   VBLANK, reads SWCHA, SWCHB and INPT0-INPT5 once and stores the eight bytes
   in a ring in RAM; then, in the visible part of the frame, it writes each
   byte to BACKGRND for its own band of 28 lines, so with DMA off (the
   hand-off's CTRL $60) the picture shows every value read.

   The input comes from rows written as muhtari-movie 1 text and read by
   runner/movie.c, so the path from a movie's spelling to the register a
   game reads is the one the runner takes. Movie line k is set before frame
   k; the first frame after a load starts at the hand-off, so line 1 never
   reaches the machine and every row's input is on line 2 (docs/embedding.md,
   Input). --write-cart writes the cartridge to FILE instead, the even
   variant reading only on even frames, for the runner's movie tests. Exit
   status: 0 when the case passes or the files are written, 1 when it fails,
   2 on an unknown case. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"
#include "internal.h"
#include "movie.h"

/* A 48 KiB linear body maps $4000-$FFFF; the program starts at $4000. */
#define BODY (48u * IMAGES_KIB)
#define ROM_BASE 0x4000u
#define PARAMS 0x4F00u /* the case's settings, read by the program */
#define RTI_AT 0x4F10u /* the NMI handler */

/* Where the program keeps its state, as indexes into system RAM ($1800):
   the ring of 16 entries of 8 bytes at $1800, and in page 0's RAM ($40 is
   $2040) the frame count and the offset of the entry last written. */
#define RING 0x0000u
#define FRAME_COUNT 0x0840u
#define LAST_ENTRY 0x0841u

/* The bytes each entry holds, in order. */
enum { SWCHA, SWCHB, INPT0, INPT1, INPT2, INPT3, INPT4, INPT5, ENTRY };

/* The program's settings at PARAMS: INPTCTRL's value, CTLSWB's, whether to
   write SWCHB and what, and the flags below. */
typedef struct settings {
    uint8_t inptctrl, ctlswb, write_swchb, swchb, flags;
} settings;
#define READ_EVEN 0x01u  /* read only on even frames */
#define READ_NEVER 0x02u /* never read an input register */
#define DMA_ON 0x04u     /* turn DMA on: the case puts a display list in RAM */

/* INPTCTRL $07: locked, MARIA on, the cartridge mapped, as a game leaves
   it (docs/hardware/bus.md, INPTCTRL). */
static const settings plain = {0x07u, 0x00u, 0x00u, 0x00u, 0x00u};

/* clang-format off */
static const uint8_t program[] = {
    0xD8u,                      /* $4000 CLD */
    0xADu, 0x00u, 0x4Fu,        /* $4001 LDA $4F00 */
    0x85u, 0x01u,               /* $4004 STA $01: INPTCTRL */
    0xADu, 0x01u, 0x4Fu,        /* $4006 LDA $4F01 */
    0x8Du, 0x83u, 0x02u,        /* $4009 STA $0283: CTLSWB */
    0xADu, 0x02u, 0x4Fu,        /* $400C LDA $4F02 */
    0xF0u, 0x06u,               /* $400F BEQ $4017: no SWCHB write */
    0xADu, 0x03u, 0x4Fu,        /* $4011 LDA $4F03 */
    0x8Du, 0x82u, 0x02u,        /* $4014 STA $0282: SWCHB */
    0xADu, 0x04u, 0x4Fu,        /* $4017 LDA $4F04 */
    0x29u, DMA_ON,              /* $401A AND #DMA_ON */
    0xF0u, 0x04u,               /* $401C BEQ $4022 */
    0xA9u, 0x40u,               /* $401E LDA #$40 */
    0x85u, 0x3Cu,               /* $4020 STA $3C: CTRL, DMA on */
    0xA5u, 0x28u,               /* $4022 LDA $28: MSTAT */
    0x30u, 0xFCu,               /* $4024 BMI $4022: until the visible frame */
    0xA5u, 0x28u,               /* $4026 LDA $28 */
    0x10u, 0xFCu,               /* $4028 BPL $4026: until VBLANK */
    0xE6u, 0x40u,               /* $402A INC $40: the frame count */
    0xADu, 0x04u, 0x4Fu,        /* $402C LDA $4F04 */
    0x29u, READ_NEVER,          /* $402F AND #READ_NEVER */
    0xD0u, 0x41u,               /* $4031 BNE $4074 */
    0xADu, 0x04u, 0x4Fu,        /* $4033 LDA $4F04 */
    0x29u, READ_EVEN,           /* $4036 AND #READ_EVEN */
    0xF0u, 0x06u,               /* $4038 BEQ $4040 */
    0xA5u, 0x40u,               /* $403A LDA $40 */
    0x29u, 0x01u,               /* $403C AND #1: the count is the frame number less 1 */
    0xF0u, 0x34u,               /* $403E BEQ $4074: an odd frame reads nothing */
    0xA5u, 0x40u,               /* $4040 LDA $40 */
    0x29u, 0x0Fu,               /* $4042 AND #15 */
    0x0Au,                      /* $4044 ASL */
    0x0Au,                      /* $4045 ASL */
    0x0Au,                      /* $4046 ASL */
    0xAAu,                      /* $4047 TAX: the entry's offset */
    0xADu, 0x80u, 0x02u,        /* $4048 LDA $0280: SWCHA */
    0x9Du, 0x00u, 0x18u,        /* $404B STA $1800,X */
    0xADu, 0x82u, 0x02u,        /* $404E LDA $0282: SWCHB */
    0x9Du, 0x01u, 0x18u,        /* $4051 STA $1801,X */
    0xA5u, 0x08u,               /* $4054 LDA $08: INPT0 */
    0x9Du, 0x02u, 0x18u,        /* $4056 STA $1802,X */
    0xA5u, 0x09u,               /* $4059 LDA $09: INPT1 */
    0x9Du, 0x03u, 0x18u,        /* $405B STA $1803,X */
    0xA5u, 0x0Au,               /* $405E LDA $0A: INPT2 */
    0x9Du, 0x04u, 0x18u,        /* $4060 STA $1804,X */
    0xA5u, 0x0Bu,               /* $4063 LDA $0B: INPT3 */
    0x9Du, 0x05u, 0x18u,        /* $4065 STA $1805,X */
    0xA5u, 0x0Cu,               /* $4068 LDA $0C: INPT4 */
    0x9Du, 0x06u, 0x18u,        /* $406A STA $1806,X */
    0xA5u, 0x0Du,               /* $406D LDA $0D: INPT5 */
    0x9Du, 0x07u, 0x18u,        /* $406F STA $1807,X */
    0x86u, 0x41u,               /* $4072 STX $41: the entry last written */
    0xA5u, 0x28u,               /* $4074 LDA $28 */
    0x30u, 0xFCu,               /* $4076 BMI $4074: until the visible frame */
    0xA6u, 0x41u,               /* $4078 LDX $41 */
    0xA0u, 0x08u,               /* $407A LDY #8: eight bands */
    0xBDu, 0x00u, 0x18u,        /* $407C LDA $1800,X */
    0x85u, 0x20u,               /* $407F STA $20: BACKGRND */
    0xA9u, 0x1Cu,               /* $4081 LDA #28 */
    0x85u, 0x42u,               /* $4083 STA $42: lines left in the band */
    0x85u, 0x24u,               /* $4085 STA $24: MARIA's WSYNC */
    0xC6u, 0x42u,               /* $4087 DEC $42 */
    0xD0u, 0xFAu,               /* $4089 BNE $4085 */
    0xE8u,                      /* $408B INX */
    0x88u,                      /* $408C DEY */
    0xD0u, 0xEDu,               /* $408D BNE $407C */
    0xA9u, 0x00u,               /* $408F LDA #0 */
    0x85u, 0x20u,               /* $4091 STA $20: BACKGRND back to 0 */
    0x4Cu, 0x22u, 0x40u,        /* $4093 JMP $4022 */
};
/* clang-format on */

static alloc_counts counts;
static uint8_t image[IMAGES_HEADER + BODY];
static uint8_t video[MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC];
static uint8_t baseline[MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC];
static int16_t audio[2u * MUHTARI__MAX_AUDIO_SAMPLES];
static muhtari_input frames[8];

static void put(uint16_t addr, uint8_t v)
{
    image[IMAGES_HEADER + (addr - ROM_BASE)] = v;
}

/* Builds the cartridge running the program with settings s into image. */
static void build(const settings *s)
{
    memset(image, 0, sizeof image);
    images_header(image, 3u, "Input", BODY, 0u);
    memcpy(image + IMAGES_HEADER, program, sizeof program);
    put(PARAMS, s->inptctrl);
    put(PARAMS + 1u, s->ctlswb);
    put(PARAMS + 2u, s->write_swchb);
    put(PARAMS + 3u, s->swchb);
    put(PARAMS + 4u, s->flags);
    put(RTI_AT, 0x40u); /* RTI */
    put(0xFFFAu, (uint8_t)(RTI_AT & 0xFFu));
    put(0xFFFBu, (uint8_t)(RTI_AT >> 8));
    put(0xFFFCu, (uint8_t)(ROM_BASE & 0xFFu));
    put(0xFFFDu, (uint8_t)(ROM_BASE >> 8));
}

/* An instance running the program with settings s, loaded through
   muhtari_load and its fast boot. NULL on failure. */
static muhtari *boot(const settings *s)
{
    build(s);
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_create failed\n");
        return NULL;
    }
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = sizeof image;
    if (muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_load failed\n");
        muhtari_destroy(m);
        return NULL;
    }
    return m;
}

/* Runs one frame into rows. */
static void frame(muhtari *m, uint8_t *rows)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = rows;
    io.video_pitch = MUHTARI__WIDTH;
    io.audio = audio;
    io.audio_capacity = (uint32_t)(sizeof audio / sizeof audio[0]);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
}

/* System RAM, through the public span. */
static const uint8_t *ram(muhtari *m)
{
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &span), MUHTARI_STATUS_OK);
    return (const uint8_t *)span.data;
}

/* Parses text into frames; returns the frame count, 0 on a refusal. */
static uint32_t parse(const char *text)
{
    muhtari__movie_header h;
    muhtari__movie_error err;
    uint32_t count = 0u;
    if (muhtari__movie_parse((const uint8_t *)text, strlen(text), &h, frames,
                             (uint32_t)(sizeof frames / sizeof frames[0]), &count, &err) != 0) {
        fprintf(stderr, "movie refused at %u:%u: %s, expected %s\n", (unsigned)err.line,
                (unsigned)err.column, err.what, err.expected);
        check_failures++;
        return 0u;
    }
    return count;
}

/* Runs count frames from a fresh load, line k set before frame k, into
   rows; copies the entry the last frame read into got and returns the
   io_hazard count. */
static uint64_t play(const settings *s, uint32_t count, uint8_t *rows, uint8_t got[ENTRY])
{
    muhtari *m = boot(s);
    if (m == NULL) {
        check_failures++;
        return 0u;
    }
    for (uint32_t k = 0; k < count; k++) {
        CHECK_EQ_U64(muhtari_set_input(m, &frames[k]), MUHTARI_STATUS_OK);
        frame(m, rows);
    }
    const uint8_t *r = ram(m);
    CHECK_EQ_U64(r[FRAME_COUNT], count - 1u); /* one read each frame after the first */
    memcpy(got, r + RING + r[LAST_ENTRY], ENTRY);
    uint64_t hazard = m->ports.io_hazard;
    muhtari_destroy(m);
    return hazard;
}

/* A row: the program's settings, a movie, the eight bytes the cartridge
   reads on the movie's last line, and the io_hazard count after it. TIA
   drives only D7 of INPT0-INPT5, so those are compared as D7 alone: H is
   1, L is 0 (docs/hardware/bus.md, Open bus). press, when not 0, is
   pressed on both ports on every line, for devices whose field cannot
   spell it. */
#define H 0x80u
#define L 0x00u
typedef struct row {
    const settings *s;
    const char *movie;
    uint8_t want[ENTRY];
    uint32_t hazard;
    uint32_t press;
} row;

static const uint8_t entry_mask[ENTRY] = {0xFFu, 0xFFu, H, H, H, H, H, H};
static const char *const entry_names[ENTRY] = {"SWCHA", "SWCHB", "INPT0", "INPT1",
                                               "INPT2", "INPT3", "INPT4", "INPT5"};

/* Plays each row and checks every byte it reads and its io_hazard count. */
static void check_rows(const char *name, const row *rows, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        const row *r = &rows[i];
        uint32_t count = parse(r->movie);
        for (uint32_t k = 0; k < count; k++) {
            frames[k].buttons[0] |= r->press;
            frames[k].buttons[1] |= r->press;
        }
        uint8_t got[ENTRY];
        uint64_t hazard = play(r->s, count, video, got);
        for (uint32_t b = 0; b < ENTRY; b++) {
            if ((got[b] & entry_mask[b]) != r->want[b]) {
                fprintf(stderr, "FAIL %s row %u: %s 0x%02X, expected 0x%02X\n", name, (unsigned)i,
                        entry_names[b], (unsigned)(got[b] & entry_mask[b]), (unsigned)r->want[b]);
                check_failures++;
            }
        }
        if (hazard != r->hazard) {
            fprintf(stderr, "FAIL %s row %u: io_hazard %u, expected %u\n", name, (unsigned)i,
                    (unsigned)hazard, (unsigned)r->hazard);
            check_failures++;
        }
    }
}

/* The program's settings for each mode. CTLSWB $14 with SWCHB $00 drives
   PB2 and PB4 low, two-button mode on both ports; CTLSWB $04 on port 0
   alone; CTLSWB $14 with no SWCHB write leaves ORB's power-on $FF on the
   pins; INPTCTRL $0F also sets TIA-EN (section 3.1, section 3.3). */
static const settings two_button = {0x07u, 0x14u, 0x01u, 0x00u, 0x00u};
static const settings two_button_port0 = {0x07u, 0x04u, 0x01u, 0x00u, 0x00u};
static const settings no_swchb_write = {0x07u, 0x14u, 0x00u, 0x00u, 0x00u};
static const settings tia_en = {0x0Fu, 0x14u, 0x01u, 0x00u, 0x00u};

#define PROLINES "muhtari-movie 1\nport1 proline\nport2 proline\n|......|......|...BB|\n"
#define STICKS "muhtari-movie 1\nport1 joystick\nport2 joystick\n|.....|.....|...BB|\n"
#define UNPLUGGED "muhtari-movie 1\nport1 none\nport2 none\n|||...BB|\n"

/* Execute the additive boundary through the same CPU pin-reading program.
   Rejections on either port must retain the complete accepted packet. */
static void extended_digital(void)
{
    muhtari *m = boot(&plain);
    CHECK(m != NULL);
    if (m == NULL) {
        return;
    }
    frame(m, NULL);
    muhtari_input_ex in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.port[0].device = MUHTARI_PORT_PROLINE;
    in.port[0].buttons = MUHTARI_BUTTON_LEFT | MUHTARI_BUTTON_1;
    in.port[1].device = MUHTARI_PORT_JOYSTICK;
    in.port[1].buttons = MUHTARI_BUTTON_DOWN | MUHTARI_BUTTON_1;
    in.switches = MUHTARI_SWITCH_PAUSE;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_OK);
    for (unsigned fault = 0u; fault < 11u; fault++) {
        muhtari_input_ex bad = in;
        bad.port[0].buttons = MUHTARI_BUTTON_UP;
        switch (fault) {
        case 0: bad.port[1].device = 999u; break;
        case 1: bad.port[1].device = MUHTARI_PORT_PADDLE; break;
        case 2: bad.port[1].buttons = MUHTARI_SNES_BUTTON_Y; break;
        case 3: bad.port[1].paddle[0] = 65536u; break;
        case 4: bad.port[1].paddle[1] = 1u; break;
        case 5: bad.port[1].keypad = 0x1000u; break;
        case 6: bad.port[1].dx = INT32_MIN; break;
        case 7: bad.port[1].dy = INT32_MAX; break;
        case 8: bad.port[1].gun_x = 320u; break;
        case 9: bad.port[1].flags = 2u; break;
        default: bad.switches = 32u; break;
        }
        CHECK_EQ_U64(muhtari_set_input_ex(m, &bad),
                     fault == 1u ? MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE
                                 : MUHTARI_STATUS_ERR_ARGUMENT);
    }
    frame(m, video);
    const uint8_t *r = ram(m);
    const uint8_t *got = r + RING + r[LAST_ENTRY];
    CHECK_EQ_HEX(got[SWCHA], 0xBDu);
    CHECK_EQ_HEX(got[SWCHB], 0x37u);
    CHECK_EQ_HEX(got[INPT0] & H, H);
    CHECK_EQ_HEX(got[INPT4] & H, L);
    CHECK_EQ_HEX(got[INPT5] & H, L);
    frame(m, NULL); /* held controls survive with no submission */
    r = ram(m);
    CHECK_EQ_HEX(r[RING + r[LAST_ENTRY] + SWCHA], 0xBDu);
    muhtari_input legacy;
    memset(&legacy, 0, sizeof legacy);
    legacy.size = (uint32_t)sizeof legacy;
    legacy.device[0] = MUHTARI_PORT_PADDLE;
    CHECK_EQ_U64(sizeof legacy, 24u);
    CHECK_EQ_U64(muhtari_set_input(m, &legacy), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    in.port[0].buttons = MUHTARI_BUTTON_RIGHT;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_OK);
    in.port[0].device = MUHTARI_PORT_UNPLUGGED;
    in.port[1].device = MUHTARI_PORT_UNPLUGGED;
    in.port[0].buttons = 0u;
    in.port[1].buttons = 0u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_OK);
    frame(m, NULL);
    r = ram(m);
    CHECK_EQ_HEX(r[RING + r[LAST_ENTRY] + SWCHA], 0xFFu);
    muhtari_destroy(m);
}

/* Each direction on each port pulls its own SWCHA pin to 0, port 0's in
   bits 7-4 as right, left, down, up and port 1's in bits 3-0
   (A7800-CARTRIDGES-AND-PERIPHERALS section 3.1). With no switch held and
   both difficulties B, SWCHB reads $3F (docs/hardware/controllers.md,
   Switches). Each direction also changes the picture of the frame that
   reads it. */
static int directions(void)
{
    extended_digital();
    static const row rows[] = {
        {&plain, PROLINES "|U.....|......|...BB|\n", {0xEFu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 1, SWCHA b4 */
        {&plain, PROLINES "|.D....|......|...BB|\n", {0xDFu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 2, SWCHA b5 */
        {&plain, PROLINES "|..L...|......|...BB|\n", {0xBFu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 3, SWCHA b6 */
        {&plain, PROLINES "|...R..|......|...BB|\n", {0x7Fu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 4, SWCHA b7 */
        {&plain, PROLINES "|......|U.....|...BB|\n", {0xFEu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 1, SWCHA b0 */
        {&plain, PROLINES "|......|.D....|...BB|\n", {0xFDu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 2, SWCHA b1 */
        {&plain, PROLINES "|......|..L...|...BB|\n", {0xFBu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 3, SWCHA b2 */
        {&plain, PROLINES "|......|...R..|...BB|\n", {0xF7u, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: pin 4, SWCHA b3 */
    };
    check_rows("directions", rows, sizeof rows / sizeof rows[0]);
    uint8_t got[ENTRY];
    (void)play(&plain, parse(PROLINES "|......|......|...BB|\n"), baseline, got);
    CHECK_EQ_HEX(got[SWCHA], 0xFFu);
    for (size_t i = 0; i < sizeof rows / sizeof rows[0]; i++) {
        (void)play(&plain, parse(rows[i].movie), video, got);
        CHECK(memcmp(video, baseline, sizeof video) != 0);
    }
    return check_failures == 0;
}

/* Two-button mode: the right button, button 1, reads 1 on INPT0 [INPT2]
   and the left, button 2, on INPT1 [INPT3], while INPT4 [INPT5] reads 1.
   SWCHB shows PB2 and PB4 driven 0. */
static int proline_two_button(void)
{
    static const row rows[] = {
        /* section 3.2: right button on INPT0, 1 = pressed; INPT4/5 read 1 */
        {&two_button, PROLINES "|....1.|......|...BB|\n", {0xFFu, 0x2Bu, H, L, L, L, H, H}, 0u, 0u},
        /* section 3.2: left button on INPT1 */
        {&two_button, PROLINES "|.....2|......|...BB|\n", {0xFFu, 0x2Bu, L, H, L, L, H, H}, 0u, 0u},
        /* section 3.2: port 1's right button on INPT2 */
        {&two_button, PROLINES "|......|....1.|...BB|\n", {0xFFu, 0x2Bu, L, L, H, L, H, H}, 0u, 0u},
        /* section 3.2: port 1's left button on INPT3 */
        {&two_button, PROLINES "|......|.....2|...BB|\n", {0xFFu, 0x2Bu, L, L, L, H, H, H}, 0u, 0u},
        /* section 3.2: nothing pressed */
        {&two_button, PROLINES "|......|......|...BB|\n", {0xFFu, 0x2Bu, L, L, L, L, H, H}, 0u, 0u},
        /* section 3.1: PB2 alone puts port 0 in two-button mode; port 1 stays
           one-button, so its button 1 pulls INPT5 low */
        {&two_button_port0,
         PROLINES "|....1.|....1.|...BB|\n",
         {0xFFu, 0x3Bu, H, L, H, L, H, L},
         0u,
         0u},
    };
    check_rows("proline_two_button", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* One-button mode: either button pulls INPT4 [INPT5] to 0. INPT0-INPT3
   follow the buttons in both modes, as section 3.2 says MAME, a7800 and
   MiSTer have it; the console's one-button readings are unverified. */
static int proline_one_button(void)
{
    static const row rows[] = {
        /* section 3.2: either button pulls INPT4 low */
        {&plain, PROLINES "|....1.|......|...BB|\n", {0xFFu, 0x3Fu, H, L, L, L, L, H}, 0u, 0u},
        /* section 3.2 */
        {&plain, PROLINES "|.....2|......|...BB|\n", {0xFFu, 0x3Fu, L, H, L, L, L, H}, 0u, 0u},
        /* section 3.2: port 1, both buttons, pull INPT5 low */
        {&plain, PROLINES "|......|....12|...BB|\n", {0xFFu, 0x3Fu, L, L, H, H, H, L}, 0u, 0u},
        /* section 3.2: nothing pressed */
        {&plain, PROLINES "|......|......|...BB|\n", {0xFFu, 0x3Fu, L, L, L, L, H, H}, 0u, 0u},
    };
    check_rows("proline_one_button", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* A 2600 stick's fire pulls pin 6 low in either mode. In two-button mode
   that shorts the driven pin, so each frame it is held adds 1 to
   io_hazard. INPT0-INPT3 have nothing on them. */
static int joystick_fire(void)
{
    static const row rows[] = {
        /* section 3.2: fire pulls INPT4 low */
        {&plain, STICKS "|....1|.....|...BB|\n", {0xFFu, 0x3Fu, L, L, L, L, L, H}, 0u, 0u},
        /* section 3.2: port 1's fire on INPT5 */
        {&plain, STICKS "|.....|....1|...BB|\n", {0xFFu, 0x3Fu, L, L, L, L, H, L}, 0u, 0u},
        /* section 3.2 and section 3.3: INPT4 still reads 0 in two-button
           mode, and the frame counts */
        {&two_button, STICKS "|....1|.....|...BB|\n", {0xFFu, 0x2Bu, L, L, L, L, L, H}, 1u, 0u},
        /* section 3.3: two frames held, two counted */
        {&two_button,
         STICKS "|.....|....1|...BB|\n|.....|....1|...BB|\n",
         {0xFFu, 0x2Bu, L, L, L, L, H, L},
         2u,
         0u},
        /* section 3.3: no fire, no count */
        {&two_button, STICKS "|.....|.....|...BB|\n", {0xFFu, 0x2Bu, L, L, L, L, H, H}, 0u, 0u},
    };
    check_rows("joystick_fire", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* CTLSWB $14 with no SWCHB write: ORB powers on $FF, so PB2 and PB4 are
   driven high and both ports stay one-button, as One on One needs. */
static int ctlswb_no_swchb_write(void)
{
    static const row rows[] = {
        /* section 3.3: one-button, so button 1 pulls INPT4 low */
        {&no_swchb_write,
         PROLINES "|....1.|......|...BB|\n",
         {0xFFu, 0x3Fu, H, L, L, L, L, H},
         0u,
         0u},
        /* section 3.3: a stick's fire is no hazard in one-button mode */
        {&no_swchb_write, STICKS "|....1|.....|...BB|\n", {0xFFu, 0x3Fu, L, L, L, L, L, H}, 0u, 0u},
    };
    check_rows("ctlswb_no_swchb_write", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* INPTCTRL's TIA-EN set: PB2 and PB4 are driven low, yet the ports stay
   one-button. */
static int tia_en_disables_two_button(void)
{
    static const row rows[] = {
        /* section 3.1: TIA-EN disables two-button mode */
        {&tia_en, PROLINES "|....1.|......|...BB|\n", {0xFFu, 0x2Bu, H, L, L, L, L, H}, 0u, 0u},
        /* section 3.1: so a stick's fire is no hazard */
        {&tia_en, STICKS "|....1|.....|...BB|\n", {0xFFu, 0x2Bu, L, L, L, L, L, H}, 0u, 0u},
    };
    check_rows("tia_en_disables_two_button", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* Reset, Select and Pause read 0 on SWCHB D0, D1 and D3 while held; a
   difficulty switch at A reads 1 on D6 (left) and D7 (right); D2, D4 and
   D5, input pins with nothing driving them, read 1
   (docs/hardware/controllers.md, Switches; docs/hardware/riot.md, Ports). */
static int switches(void)
{
    static const row rows[] = {
        {&plain, PROLINES "|......|......|R..BB|\n", {0xFFu, 0x3Eu, L, L, L, L, H, H}, 0u, 0u},
        {&plain, PROLINES "|......|......|.S.BB|\n", {0xFFu, 0x3Du, L, L, L, L, H, H}, 0u, 0u},
        {&plain, PROLINES "|......|......|..PBB|\n", {0xFFu, 0x37u, L, L, L, L, H, H}, 0u, 0u},
        {&plain, PROLINES "|......|......|...AB|\n", {0xFFu, 0x7Fu, L, L, L, L, H, H}, 0u, 0u},
        {&plain, PROLINES "|......|......|...BA|\n", {0xFFu, 0xBFu, L, L, L, L, H, H}, 0u, 0u},
        {&plain, PROLINES "|......|......|RSPAA|\n", {0xFFu, 0xF4u, L, L, L, L, H, H}, 0u, 0u},
    };
    check_rows("switches", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* An unplugged port's pins read released, whatever the host presses: SWCHA
   high, INPT0-INPT3 0 and INPT4 [INPT5] 1, in either mode. */
static int unplugged(void)
{
    static const row rows[] = {
        /* section 3.1: nothing closes a switch */
        {&plain, UNPLUGGED "|||...BB|\n", {0xFFu, 0x3Fu, L, L, L, L, H, H}, 0u, 0x3Fu},
        /* section 3.1 */
        {&two_button, UNPLUGGED "|||...BB|\n", {0xFFu, 0x2Bu, L, L, L, L, H, H}, 0u, 0x3Fu},
    };
    check_rows("unplugged", rows, sizeof rows / sizeof rows[0]);
    return check_failures == 0;
}

/* SWCHA bit 7, port 0's right, is PA7. With the falling edge selected, a
   Right press applied at VBLANK start sets PA7's flag at that tick, and its
   release, a rising edge, does not (docs/hardware/riot.md, PA7). */
/* A waveform executes real LDA/STA instructions in system RAM. The RIOT
   access therefore uses the slow bus cycle's data tick, including DDR
   release edges (docs/hardware/controllers.md, Serial EEPROM). */
static void serial_write(muhtari *m, uint16_t reg, uint8_t value)
{
    const uint8_t code[] = {0xA9u, value, 0x8Du, (uint8_t)reg, (uint8_t)(reg >> 8)};
    memcpy(m->ram, code, sizeof code);
    m->cpu.pc = 0x1800u;
    muhtari__cpu_step(m);
    muhtari__cpu_step(m);
}

static uint8_t serial_read(muhtari *m)
{
    const uint8_t code[] = {0xADu, 0x80u, 0x02u};
    memcpy(m->ram, code, sizeof code);
    m->cpu.pc = 0x1800u;
    muhtari__cpu_step(m);
    return m->cpu.a;
}

static unsigned serial_drive_ora;
static uint64_t serial_pin_tick;

static void serial_pins(muhtari *m, unsigned port, unsigned scl, unsigned sda)
{
    unsigned shift = port == 1u ? 4u : 0u;
    uint8_t low = (uint8_t)(((scl == 0u ? 8u : 0u) | (sda == 0u ? 4u : 0u)) << shift);
    if (serial_drive_ora != 0u) {
        serial_write(m, 0x0280u, (uint8_t)((12u << shift) & (unsigned)~low));
        serial_pin_tick = m->bus.ticks - 1u;
        serial_write(m, 0x0281u, (uint8_t)(12u << shift));
    } else {
        serial_write(m, 0x0281u, low);
        serial_pin_tick = m->bus.ticks - 1u;
    }
}

static void serial_start(muhtari *m, unsigned p)
{
    serial_pins(m, p, 0u, 1u);
    serial_pins(m, p, 1u, 1u);
    serial_pins(m, p, 1u, 0u);
    serial_pins(m, p, 0u, 0u);
}

static void serial_stop(muhtari *m, unsigned p)
{
    serial_pins(m, p, 0u, 0u);
    serial_pins(m, p, 1u, 0u);
    serial_pins(m, p, 1u, 1u);
}

static unsigned serial_send(muhtari *m, unsigned p, uint8_t byte)
{
    unsigned shift = p == 1u ? 4u : 0u;
    for (unsigned i = 0u; i < 8u; i++) {
        unsigned bit = (unsigned)(byte >> (7u - i)) & 1u;
        serial_pins(m, p, 0u, bit);
        serial_pins(m, p, 1u, bit);
        serial_pins(m, p, 0u, bit);
    }
    serial_pins(m, p, 0u, 1u);
    serial_pins(m, p, 1u, 1u);
    unsigned ack = ((unsigned)serial_read(m) >> (shift + 2u)) & 1u;
    serial_pins(m, p, 0u, 1u);
    return ack;
}

static uint8_t serial_receive(muhtari *m, unsigned p, unsigned nak)
{
    uint8_t value = 0u;
    unsigned shift = p == 1u ? 4u : 0u;
    for (unsigned i = 0u; i < 8u; i++) {
        serial_pins(m, p, 0u, 1u);
        serial_pins(m, p, 1u, 1u);
        value =
            (uint8_t)(((unsigned)value << 1) | (((unsigned)serial_read(m) >> (shift + 2u)) & 1u));
        serial_pins(m, p, 0u, 1u);
    }
    serial_pins(m, p, 0u, nak);
    serial_pins(m, p, 1u, nak);
    serial_pins(m, p, 0u, nak);
    serial_pins(m, p, 0u, 1u);
    return value;
}

static muhtari *serial_boot(unsigned p, uint32_t device)
{
    muhtari *m = boot(&plain);
    if (m == NULL) {
        return NULL;
    }
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = sizeof image;
    uint8_t old_port = image[55];
    uint8_t old_hint = image[58];
    if (device != 0u) {
        media.port[p - 1u] = device;
    } else if (p == 1u) {
        image[55] = MUHTARI_PORT_ATARIVOX;
    } else {
        image[58] = 2u; /* metadata defaults the EEPROM to port 2 */
    }
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    image[55] = old_port;
    image[58] = old_hint;
    CHECK_EQ_U64(m->battery.eeprom_port, p);
    CHECK_EQ_U64(muhtari__ports_device(m, p - 1u), m->battery.eeprom_device);
    serial_write(m, 0x0280u, 0u);
    return m;
}

static void serial_address(muhtari *m, unsigned p, uint16_t address)
{
    serial_start(m, p);
    CHECK_EQ_U64(serial_send(m, p, 0xA0u), 0u);
    CHECK_EQ_U64(serial_send(m, p, (uint8_t)(address >> 8)), 0u);
    CHECK_EQ_U64(serial_send(m, p, (uint8_t)address), 0u);
}

static void serial_wait(muhtari *m)
{
    uint64_t until = m->bus.ticks + 35796u;
    while (m->bus.ticks < until) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
}

static int savekey_protocol(void)
{
    for (unsigned p = 1u; p <= 2u; p++) {
        for (unsigned kind = 0u; kind < 3u; kind++) {
            muhtari *m = serial_boot(p, kind == 0u   ? MUHTARI_PORT_SAVEKEY
                                        : kind == 1u ? MUHTARI_PORT_ATARIVOX
                                                     : 0u);
            CHECK(m != NULL);
            if (m == NULL) {
                continue;
            }
            serial_start(m, p);
            CHECK_EQ_U64(serial_send(m, p, 0xA0u), 0u);
            CHECK_EQ_U64(serial_send(m, p, 0x12u), 0u);
            CHECK_EQ_U64(serial_send(m, p, 0x34u), 0u);
            CHECK_EQ_U64(serial_send(m, p, 0x5Au), 0u);
            serial_stop(m, p);
            CHECK_EQ_U64(m->savekey.busy_until, serial_pin_tick + MUHTARI__SAVEKEY_BUSY_TICKS);
            CHECK_EQ_HEX(m->battery.data[0x1234u], 0xFFu);
            serial_start(m, p);
            CHECK_EQ_U64(serial_send(m, p, 0xA0u), 1u);
            serial_stop(m, p);
            serial_wait(m);
            serial_start(m, p);
            CHECK_EQ_U64(serial_send(m, p, 0xA0u), 0u);
            CHECK_EQ_U64(serial_send(m, p, 0x12u), 0u);
            CHECK_EQ_U64(serial_send(m, p, 0x34u), 0u);
            serial_start(m, p);
            CHECK_EQ_U64(serial_send(m, p, 0xA1u), 0u);
            CHECK_EQ_HEX(serial_receive(m, p, 1u), 0x5Au);
            serial_stop(m, p);
            CHECK_EQ_HEX(m->battery.data[0x1234u], 0x5Au);
            CHECK_EQ_U64(m->battery.generation, 1u);
            muhtari_destroy(m);
        }
    }
    return check_failures == 0;
}

static int savekey_pins(void)
{
    serial_drive_ora = 1u;
    (void)savekey_protocol();
    serial_drive_ora = 0u;
    for (unsigned p = 1u; p <= 2u; p++) {
        muhtari *m = serial_boot(p, MUHTARI_PORT_SAVEKEY);
        if (m == NULL) {
            CHECK(m != NULL);
            continue;
        }
        unsigned shift = p == 1u ? 4u : 0u;
        serial_start(m, p);
        /* A control's eighth falling edge drives ACK, which remains low
           when the host explicitly drives high during turnaround. */
        for (unsigned i = 0u; i < 8u; i++) {
            unsigned bit = (0xA0u >> (7u - i)) & 1u;
            serial_pins(m, p, 0u, bit);
            serial_pins(m, p, 1u, bit);
            serial_pins(m, p, 0u, bit);
        }
        serial_pins(m, p, 0u, 1u);
        serial_write(m, 0x0280u, (uint8_t)(4u << shift));
        serial_write(m, 0x0281u, (uint8_t)(12u << shift));
        CHECK_EQ_U64(((unsigned)serial_read(m) >> (shift + 2u)) & 1u, 0u);
        struct muhtari__savekey before = m->savekey;
        uint64_t generation = m->battery.generation;
        for (unsigned i = 0u; i < 8u; i++) {
            (void)serial_read(m);
            (void)muhtari__bus_peek(m, 0x0280u);
        }
        CHECK(memcmp(&before, &m->savekey, sizeof before) == 0);
        CHECK_EQ_U64(m->battery.generation, generation);
        serial_write(m, 0x0280u, 0u);
        serial_pins(m, p, 1u, 1u);
        CHECK_EQ_U64(((unsigned)serial_read(m) >> (shift + 2u)) & 1u, 0u);
        serial_pins(m, p, 0u, 1u);
        CHECK_EQ_U64(serial_send(m, p, 0x00u), 0u);
        CHECK_EQ_U64(serial_send(m, p, 0x00u), 0u);
        serial_stop(m, p);
        muhtari_destroy(m);
    }
    return check_failures == 0;
}

static int savekey_wrap(void)
{
    for (unsigned p = 1u; p <= 2u; p++) {
        muhtari *m = serial_boot(p, MUHTARI_PORT_SAVEKEY);
        if (m == NULL) {
            CHECK(m != NULL);
            continue;
        }
        m->battery.data[0] = 0x96u;    /* clean host restore, before execution */
        serial_address(m, p, 0xFFFEu); /* ignored A15 */
        for (unsigned i = 0u; i < 130u; i++) {
            CHECK_EQ_U64(serial_send(m, p, (uint8_t)(0x30u + i)), 0u);
        }
        serial_stop(m, p);
        serial_wait(m);
        serial_address(m, p, 0xFFFFu);
        serial_start(m, p);
        CHECK_EQ_U64(serial_send(m, p, 0xA1u), 0u);
        CHECK_EQ_HEX(serial_receive(m, p, 0u), (uint8_t)(0x30u + 129u));
        CHECK_EQ_HEX(serial_receive(m, p, 1u), 0x96u);
        serial_stop(m, p);
        for (unsigned i = 0u; i < 32768u; i++) {
            uint8_t expected = 0xFFu;
            if (i == 0u) {
                expected = 0x96u;
            } else if (i >= 0x7FC0u) {
                unsigned first = (i - 0x7FC0u + 2u) & 63u;
                unsigned last = first + ((129u - first) / 64u) * 64u;
                expected = (uint8_t)(0x30u + last);
            }
            CHECK_EQ_HEX(m->battery.data[i], expected);
        }
        CHECK_EQ_U64(m->battery.generation, 1u);
        /* A current read continues after the previous master NAK. */
        serial_start(m, p);
        CHECK_EQ_U64(serial_send(m, p, 0xA1u), 0u);
        CHECK_EQ_HEX(serial_receive(m, p, 1u), 0xFFu);
        serial_stop(m, p);
        muhtari_destroy(m);
    }
    return check_failures == 0;
}

static int savekey_recovery(void)
{
    muhtari *m = serial_boot(2u, MUHTARI_PORT_ATARIVOX);
    if (m == NULL) {
        CHECK(m != NULL);
        return 0;
    }
    serial_start(m, 2u);
    CHECK_EQ_U64(serial_send(m, 2u, 0xA2u), 1u);
    CHECK_EQ_U64(serial_send(m, 2u, 0u), 1u);
    serial_stop(m, 2u);
    for (unsigned incomplete = 0u; incomplete < 8u; incomplete++) {
        serial_start(m, 2u);
        for (unsigned bit = 0u; bit < incomplete; bit++) {
            serial_pins(m, 2u, 0u, 0u);
            serial_pins(m, 2u, 1u, 0u);
            serial_pins(m, 2u, 0u, 0u);
        }
        serial_stop(m, 2u);
    }
    serial_start(m, 2u);
    CHECK_EQ_U64(serial_send(m, 2u, 0xA0u), 0u);
    CHECK_EQ_U64(serial_send(m, 2u, 0x20u), 0u);
    serial_stop(m, 2u); /* incomplete address */
    serial_address(m, 2u, 0x1234u);
    serial_stop(m, 2u); /* address-only STOP does not program */
    serial_start(m, 2u);
    CHECK_EQ_U64(serial_send(m, 2u, 0xA1u), 0u);
    CHECK_EQ_HEX(serial_receive(m, 2u, 1u), 0xFFu);
    CHECK_EQ_U64(serial_send(m, 2u, 0u), 1u); /* final master NAK releases */
    serial_stop(m, 2u);
    serial_address(m, 2u, 0x0001u);
    CHECK_EQ_U64(serial_send(m, 2u, 0x11u), 0u);
    serial_start(m, 2u); /* recovery aborts un-stopped buffered data */
    CHECK_EQ_U64(serial_send(m, 2u, 0xA1u), 0u);
    CHECK_EQ_HEX(serial_receive(m, 2u, 1u), 0xFFu);
    serial_stop(m, 2u);
    serial_wait(m);
    serial_start(m, 2u);
    CHECK_EQ_U64(serial_send(m, 2u, 0xA0u), 0u);
    CHECK_EQ_U64(m->battery.generation, 0u);
    CHECK_EQ_HEX(m->battery.data[1], 0xFFu);
    serial_stop(m, 2u);
    muhtari_destroy(m);
    return check_failures == 0;
}

static int pa7_edge(void)
{
    uint32_t count = parse(PROLINES "|...R..|......|...BB|\n");
    CHECK_EQ_U64(count, 2u);
    muhtari *m = boot(&plain);
    if (m == NULL) {
        return 0;
    }
    CHECK_EQ_U64(muhtari_set_input(m, &frames[0]), MUHTARI_STATUS_OK);
    frame(m, video);
    frame(m, video);
    CHECK_EQ_U64(m->riot.pa7_flag, 0u);

    /* What muhtari_run_frame does first at this VBLANK start. */
    uint64_t at = m->bus.ticks;
    CHECK_EQ_U64(muhtari_set_input(m, &frames[1]), MUHTARI_STATUS_OK);
    muhtari__ports_latch(m);
    CHECK_EQ_U64(m->riot.pa7_flag, 1u);
    CHECK_EQ_U64(m->bus.ticks, at);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x0280u), 0x7Fu);

    (void)muhtari__riot_read(m, 0x0285u); /* TIMINT clears the flag */
    CHECK_EQ_U64(m->riot.pa7_flag, 0u);
    CHECK_EQ_U64(muhtari_set_input(m, &frames[0]), MUHTARI_STATUS_OK);
    muhtari__ports_latch(m);
    CHECK_EQ_U64(m->riot.pa7_flag, 0u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* Runs n frames of the program with settings s from a fresh load; with
   peek, peeks SWCHA and INPT4 before each. Returns the instance's
   counters through *c. */
static void counted(const settings *s, uint32_t n, int peek, muhtari_counters *c)
{
    memset(c, 0, sizeof *c);
    c->size = (uint32_t)sizeof *c;
    muhtari *m = boot(s);
    if (m == NULL) {
        check_failures++;
        return;
    }
    if ((s->flags & DMA_ON) != 0u) {
        /* 16 zones of 16 lines at DPP's hand-off value $1F84, each listing
           one object one byte wide whose graphics are at $0280 on the
           zone's last line: OFFSET 0 adds nothing to the high byte
           (A7800-MARIA section 2.1). */
        muhtari_span span;
        memset(&span, 0, sizeof span);
        span.size = (uint32_t)sizeof span;
        CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &span), MUHTARI_STATUS_OK);
        for (uint32_t z = 0; z < 16u; z++) {
            span.data[0x784u + 3u * z] = 0x0Fu;
            span.data[0x784u + 3u * z + 1u] = 0x22u;
            span.data[0x784u + 3u * z + 2u] = 0x00u;
        }
        static const uint8_t dl[6] = {0x80u, 0x1Fu, 0x02u, 0x00u, 0x00u, 0x00u};
        memcpy(span.data + 0xA00u, dl, sizeof dl);
    }
    for (uint32_t k = 0; k < n; k++) {
        if (peek) {
            (void)muhtari__bus_peek(m, 0x0280u);
            (void)muhtari__bus_peek(m, 0x000Cu);
        }
        frame(m, video);
    }
    CHECK_EQ_U64(muhtari_get_counters(m, c), MUHTARI_STATUS_OK);
    if ((s->flags & DMA_ON) != 0u) {
        CHECK_EQ_HEX(m->maria.reg[0x1Cu], 0x40u); /* CTRL: DMA on */
    }
    muhtari_destroy(m);
}

/* A lag frame is one in which the CPU reads no input register. The
   program reading on even frames only lags on the N/2 odd ones; reading
   every frame, it lags only on the hand-off frame, which reads nothing. */
static int lag_even_frames(void)
{
    static const settings even = {0x07u, 0x00u, 0x00u, 0x00u, READ_EVEN};
    muhtari_counters c;
    counted(&even, 60u, 0, &c);
    CHECK_EQ_U64(c.frames, 60u);
    CHECK_EQ_U64(c.lag_frames, 30u);
    counted(&plain, 60u, 0, &c);
    CHECK_EQ_U64(c.lag_frames, 1u);
    return check_failures == 0;
}

/* MARIA's DMA fetch from $0280 and a peek of SWCHA or INPT4 are no CPU
   reads, so a program that never reads input lags on every frame. */
static int lag_dma_and_peek(void)
{
    static const settings never_dma = {0x07u, 0x00u, 0x00u, 0x00u, READ_NEVER | DMA_ON};
    muhtari_counters c;
    counted(&never_dma, 10u, 1, &c);
    CHECK_EQ_U64(c.frames, 10u);
    CHECK_EQ_U64(c.lag_frames, 10u);
    return check_failures == 0;
}

/* Writes the cartridge for each --write-cart FILE [even] in argv to FILE.
   Returns the exit status. */
static int write_carts(int argc, char **argv)
{
    static const settings even = {0x07u, 0x00u, 0x00u, 0x00u, READ_EVEN};
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--write-cart") != 0 || i + 1 >= argc) {
            fprintf(stderr, "usage: muhtari-test-input --write-cart FILE [even]...\n");
            return 2;
        }
        const char *path = argv[++i];
        int is_even = i + 1 < argc && strcmp(argv[i + 1], "even") == 0;
        i += is_even;
        build(is_even ? &even : &plain);
        FILE *f = fopen(path, "wb");
        if (f == NULL || fwrite(image, 1u, sizeof image, f) != sizeof image) {
            fprintf(stderr, "cannot write %s\n", path);
            if (f != NULL) {
                (void)fclose(f);
            }
            return 1;
        }
        if (fclose(f) != 0) {
            fprintf(stderr, "cannot write %s\n", path);
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        {"directions", directions},
        {"proline_two_button", proline_two_button},
        {"proline_one_button", proline_one_button},
        {"joystick_fire", joystick_fire},
        {"ctlswb_no_swchb_write", ctlswb_no_swchb_write},
        {"tia_en_disables_two_button", tia_en_disables_two_button},
        {"switches", switches},
        {"unplugged", unplugged},
        {"pa7_edge", pa7_edge},
        {"savekey.pins", savekey_pins},
        {"savekey.protocol", savekey_protocol},
        {"savekey.wrap", savekey_wrap},
        {"savekey.recovery", savekey_recovery},
        {"lag_even_frames", lag_even_frames},
        {"lag_dma_and_peek", lag_dma_and_peek},
    };
    if (argc > 1 && strcmp(argv[1], "--write-cart") == 0) {
        return write_carts(argc, argv);
    }
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-input <case>\n");
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            (void)cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "unknown case %s\n", argv[1]);
    return 2;
}
