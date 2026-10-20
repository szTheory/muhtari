/* Controllers and console switches as a game reads them, one case per run:

     muhtari-test-input <case>

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
   Input). Exit status: 0 when the case passes, 1 when it fails, 2 on an
   unknown case. */
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

/* An instance running the program with settings s, loaded through
   muhtari_load and its fast boot. NULL on failure. */
static muhtari *boot(const settings *s)
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
   rows, and copies the entry the last frame read into got. */
static void play(const settings *s, uint32_t count, uint8_t *rows, uint8_t got[ENTRY])
{
    muhtari *m = boot(s);
    if (m == NULL) {
        check_failures++;
        return;
    }
    for (uint32_t k = 0; k < count; k++) {
        CHECK_EQ_U64(muhtari_set_input(m, &frames[k]), MUHTARI_STATUS_OK);
        frame(m, rows);
    }
    const uint8_t *r = ram(m);
    CHECK_EQ_U64(r[FRAME_COUNT], count - 1u); /* one read each frame after the first */
    memcpy(got, r + RING + r[LAST_ENTRY], ENTRY);
    muhtari_destroy(m);
}

/* A row: a movie and what the cartridge reads on its last line. */
typedef struct row {
    const char *movie;
    uint8_t swcha;
} row;

#define PROLINES "muhtari-movie 1\nport1 proline\nport2 proline\n|......|......|...BB|\n"

/* Each direction on each port pulls its own SWCHA pin to 0, port 0's in
   bits 7-4 as right, left, down, up and port 1's in bits 3-0
   (A7800-CARTRIDGES-AND-PERIPHERALS section 3.1). Each also changes the
   picture of the frame that reads it. */
static int directions(void)
{
    static const row rows[] = {
        {PROLINES "|U.....|......|...BB|\n", 0xEFu}, /* section 3.1: pin 1, SWCHA b4 */
        {PROLINES "|.D....|......|...BB|\n", 0xDFu}, /* section 3.1: pin 2, SWCHA b5 */
        {PROLINES "|..L...|......|...BB|\n", 0xBFu}, /* section 3.1: pin 3, SWCHA b6 */
        {PROLINES "|...R..|......|...BB|\n", 0x7Fu}, /* section 3.1: pin 4, SWCHA b7 */
        {PROLINES "|......|U.....|...BB|\n", 0xFEu}, /* section 3.1: pin 1, SWCHA b0 */
        {PROLINES "|......|.D....|...BB|\n", 0xFDu}, /* section 3.1: pin 2, SWCHA b1 */
        {PROLINES "|......|..L...|...BB|\n", 0xFBu}, /* section 3.1: pin 3, SWCHA b2 */
        {PROLINES "|......|...R..|...BB|\n", 0xF7u}, /* section 3.1: pin 4, SWCHA b3 */
    };
    uint8_t got[ENTRY];
    uint32_t idle = parse(PROLINES "|......|......|...BB|\n");
    play(&plain, idle, baseline, got);
    CHECK_EQ_HEX(got[SWCHA], 0xFFu);
    for (size_t i = 0; i < sizeof rows / sizeof rows[0]; i++) {
        uint32_t count = parse(rows[i].movie);
        CHECK_EQ_U64(count, 2u);
        play(&plain, count, video, got);
        if (got[SWCHA] != rows[i].swcha) {
            fprintf(stderr, "FAIL row %u: SWCHA 0x%02X, expected 0x%02X\n", (unsigned)i,
                    (unsigned)got[SWCHA], (unsigned)rows[i].swcha);
            check_failures++;
        }
        CHECK(memcmp(video, baseline, sizeof video) != 0);
    }
    return check_failures == 0;
}

int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        {"directions", directions},
    };
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
