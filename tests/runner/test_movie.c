/* The muhtari-movie 1 reader: good movies give their header and frames,
   and each bad one is refused at its exact line and column with its
   message (docs/runner.md, Movies). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../runner/movie.h"
#include "../check.h"

/* A bad movie: its bytes (n of them, or up to the NUL when n is 0), and
   where and why it is refused. */
typedef struct bad {
    size_t n;
    const char *text;
    uint32_t line, column;
    const char *message;
} bad;

#define HEAD "muhtari-movie 1\nport1 proline\nport2 joystick\n"

/* clang-format off */
static const bad bads[] = {
    {0, "", 1, 1, "empty file, expected muhtari-movie 1"},
    {0, "muhtari-movie\n", 1, 1, "not a movie, expected muhtari-movie 1"},
    {0, "muhtari-movie 2\n", 1, 15, "unknown version, expected 1"},
    {0, "muhtari-movie 1\nport1 proline\nport3 none\n", 3, 1, "unknown key, expected"},
    {0, "muhtari-movie 1\nport1 proline\nport1 none\n", 3, 1, "duplicate key, expected each key once"},
    {0, "muhtari-movie 1\nport1 auto\n", 2, 7, "auto, expected proline, joystick, none, savekey or atarivox"},
    {0, "muhtari-movie 1\nport1 mouse\n", 2, 7, "unknown device, expected proline, joystick, none, savekey or atarivox"},
    {0, "muhtari-movie 1\nport1\n", 2, 6, "missing value"},
    {0, "muhtari-movie 1\nport2 none\n|||...BB|\n", 3, 1, "port1 missing, expected a port1 line"},
    {0, "muhtari-movie 1\nport1 none\n", 3, 1, "port2 missing, expected a port2 line"},
    {0, HEAD "region pal\n", 4, 8, "pal is not supported, expected ntsc"},
    {0, HEAD "region secam\n", 4, 8, "unknown region, expected ntsc"},
    {0, HEAD "boot bios\n|......|.....|...BB|\n", 4, 6, "boot bios without bios-sha256"},
    {0, HEAD "boot warm\n", 4, 6, "unknown boot, expected fast or bios"},
    {0, "muhtari-movie 1\nrom-sha256 abc\n", 2, 15, "too few hex digits, expected 64 hex digits"},
    {0, "muhtari-movie 1\nrom-sha256 xyz\n", 2, 12, "not a hex digit, expected 64 hex digits"},
    {0, HEAD "behaviour x\n", 4, 11, "not a revision number"},
    {0, HEAD "|......|.....|\n", 4, 15, "end of line, expected R or ."},
    {0, HEAD "|.....|.....|...BB|\n", 4, 7, "field too short, expected 2 or ."},
    {0, HEAD "|......|......|...BB|\n", 4, 14, "field too long, expected |"},
    {0, HEAD "|......|.....|...BB|x\n", 4, 21, "text after the last field, expected end of line"},
    {0, HEAD "|D.....|.....|...BB|\n", 4, 2, "letter out of place, expected U or ."},
    {0, HEAD "|......|.....|.X.BB|\n", 4, 16, "letter out of place, expected S or ."},
    {0, HEAD "|......|.....|...BC|\n", 4, 19, "letter out of place, expected A or B"},
    {0, HEAD "|......|.....|....B|\n", 4, 18, "letter out of place, expected A or B"},
    {0, "muhtari-movie 1\r\nport1 none\rport2 none\n", 2, 11, "bare CR, expected LF"},
    {27, "muhtari-movie 1\nport1 n\0ne\n", 2, 8, "NUL byte, expected printable ASCII"},
    {0, "\xEF\xBB\xBFmuhtari-movie 1\n", 1, 1, "byte order mark, expected muhtari-movie 1"},
    {0, "muhtari-movie 1\n# caf\x80\n", 2, 6, "byte above $7F, expected printable ASCII"},
    {0, "muhtari-movie 1\nport1\tnone\n", 2, 6, "control character, expected printable ASCII"},
    {0, "muhtari-movie 1\nport1 none\nport2 none\n|||...BB|\n# end\n", 5, 1, "comment after the frames"},
    {0, "muhtari-movie 1\nport1 none\nport2 none\n|||...BB|\n\n", 5, 1, "blank line after the frames"},
    {0, "muhtari-movie 1\nport1 none\nport2 none\n|||...BB|\nport1 none\n", 5, 1, "not a frame line"},
};
/* clang-format on */

static muhtari_input frames[4];

/* Expects the n bytes at p refused at line:column with message in
   "<what>, expected <what>", counting frames only. */
static void refused(const char *name, const uint8_t *p, size_t n, uint32_t line, uint32_t column,
                    const char *message)
{
    muhtari__movie_header h;
    muhtari__movie_error err;
    uint32_t count = 0u;
    memset(&err, 0, sizeof err);
    int rc = muhtari__movie_parse(p, n, &h, NULL, 0u, &count, &err);
    char got[160];
    snprintf(got, sizeof got, "%u:%u: %s, expected %s", (unsigned)err.line, (unsigned)err.column,
             err.what != NULL ? err.what : "(none)",
             err.expected != NULL ? err.expected : "(none)");
    if (rc != -1 || err.line != line || err.column != column || err.what == NULL ||
        strstr(got, message) == NULL) {
        fprintf(stderr, "FAIL %s: got rc %d, %s; expected %u:%u: %s\n", name, rc, got,
                (unsigned)line, (unsigned)column, message);
        check_failures++;
    }
}

static int parse_ok(const char *text, muhtari__movie_header *h, uint32_t *count)
{
    muhtari__movie_error err;
    memset(&err, 0, sizeof err);
    int rc = muhtari__movie_parse((const uint8_t *)text, strlen(text), h, frames,
                                  (uint32_t)(sizeof frames / sizeof frames[0]), count, &err);
    if (rc != 0) {
        fprintf(stderr, "refused at %u:%u: %s, expected %s\n", (unsigned)err.line,
                (unsigned)err.column, err.what, err.expected);
    }
    return rc;
}

/* The grammar's own example, with LF and with CRLF line endings. */
static const char example[] = "muhtari-movie 1\n"
                              "# comments and blank lines: header only\n"
                              "\n"
                              "rom-sha256 "
                              "00112233445566778899aabbccddeeff00112233445566778899AABBCCDDEEFF\n"
                              "port1 proline\n"
                              "port2 joystick\n"
                              "region ntsc\n"
                              "boot fast\n"
                              "behaviour 1\n"
                              "|...R1.|.....|...BB|\n"
                              "|U.L..2|U...1|RS.AB|\n";

static void check_example(const char *text)
{
    muhtari__movie_header h;
    uint32_t count = 0u;
    CHECK_EQ_U64(parse_ok(text, &h, &count), 0);
    CHECK_EQ_U64(h.version, 1u);
    CHECK_EQ_U64(h.has_rom_sha256, 1u);
    CHECK_EQ_HEX(h.rom_sha256[0], 0x00u);
    CHECK_EQ_HEX(h.rom_sha256[1], 0x11u);
    CHECK_EQ_HEX(h.rom_sha256[15], 0xFFu);
    CHECK_EQ_HEX(h.rom_sha256[31], 0xFFu);
    CHECK_EQ_U64(h.device[0], MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(h.device[1], MUHTARI_PORT_JOYSTICK);
    CHECK_EQ_U64(h.has_region, 1u);
    CHECK_EQ_U64(h.region, MUHTARI_REGION_NTSC);
    CHECK_EQ_U64(h.has_boot, 1u);
    CHECK_EQ_U64(h.boot, MUHTARI__MOVIE_BOOT_FAST);
    CHECK_EQ_U64(h.has_bios_sha256, 0u);
    CHECK_EQ_U64(h.has_behaviour, 1u);
    CHECK_EQ_U64(h.behaviour, 1u);
    CHECK_EQ_U64(count, 2u);
    CHECK_EQ_U64(frames[0].size, sizeof frames[0]);
    CHECK_EQ_U64(frames[0].device[0], MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(frames[0].device[1], MUHTARI_PORT_JOYSTICK);
    CHECK_EQ_U64(frames[0].buttons[0], MUHTARI_BUTTON_RIGHT | MUHTARI_BUTTON_1);
    CHECK_EQ_U64(frames[0].buttons[1], 0u);
    CHECK_EQ_U64(frames[0].switches, 0u);
    CHECK_EQ_U64(frames[1].buttons[0], MUHTARI_BUTTON_UP | MUHTARI_BUTTON_LEFT | MUHTARI_BUTTON_2);
    CHECK_EQ_U64(frames[1].buttons[1], MUHTARI_BUTTON_UP | MUHTARI_BUTTON_1);
    CHECK_EQ_U64(frames[1].switches,
                 MUHTARI_SWITCH_RESET | MUHTARI_SWITCH_SELECT | MUHTARI_SWITCH_LEFT_DIFFICULTY_A);
}

/* Appends s to the buffer at *at. */
static void put(char *buf, size_t *at, const char *s)
{
    size_t n = strlen(s);
    memcpy(buf + *at, s, n);
    *at += n;
}

static void good(void)
{
    check_example(example);
    /* The same with CR LF endings. */
    static char crlf[sizeof example * 2u];
    size_t at = 0u;
    for (size_t i = 0; example[i] != '\0'; i++) {
        if (example[i] == '\n') {
            crlf[at++] = '\r';
        }
        crlf[at++] = example[i];
    }
    crlf[at] = '\0';
    check_example(crlf);

    /* A header and no frames: zero frames. bios-sha256 goes with boot bios,
       and a header of 32 lines is allowed. */
    muhtari__movie_header h;
    uint32_t count = 7u;
    static char header32[1024];
    at = 0u;
    put(header32, &at, "muhtari-movie 1\nport1 none\nport2 none\nboot bios\nbios-sha256 ");
    for (uint32_t i = 0; i < 64u; i++) {
        put(header32, &at, "e");
    }
    put(header32, &at, "\n");
    for (uint32_t i = 0; i < 27u; i++) {
        put(header32, &at, i % 2u == 0u ? "# note\n" : "\n");
    }
    header32[at] = '\0';
    CHECK_EQ_U64(parse_ok(header32, &h, &count), 0);
    CHECK_EQ_U64(count, 0u);
    CHECK_EQ_U64(h.device[0], MUHTARI_PORT_UNPLUGGED);
    CHECK_EQ_U64(h.has_boot, 1u);
    CHECK_EQ_U64(h.boot, MUHTARI__MOVIE_BOOT_BIOS);
    CHECK_EQ_U64(h.has_bios_sha256, 1u);
    CHECK_EQ_HEX(h.bios_sha256[31], 0xEEu);

    /* frames NULL counts only; a buffer too small is refused. */
    muhtari__movie_error err;
    CHECK_EQ_U64(
        muhtari__movie_parse((const uint8_t *)example, strlen(example), &h, NULL, 0u, &count, &err),
        0);
    CHECK_EQ_U64(count, 2u);
    CHECK_EQ_U64(muhtari__movie_parse((const uint8_t *)example, strlen(example), &h, frames, 1u,
                                      &count, &err),
                 -1);
    CHECK_EQ_U64(err.line, 11u);
}

static void limits(void)
{
    static char buf[4096];
    size_t at = 0u;

    /* A 65-byte line. */
    put(buf, &at, "muhtari-movie 1\n# ");
    for (uint32_t i = 0; i < 63u; i++) {
        put(buf, &at, "x");
    }
    put(buf, &at, "\n");
    refused("65-byte line", (const uint8_t *)buf, at, 2u, 65u,
            "line over 64 bytes, expected at most 64 bytes");

    /* 63 and 65 hex digits in rom-sha256. */
    at = 0u;
    put(buf, &at, "muhtari-movie 1\nrom-sha256 ");
    for (uint32_t i = 0; i < 63u; i++) {
        put(buf, &at, "a");
    }
    put(buf, &at, "\n");
    refused("63 hex digits", (const uint8_t *)buf, at, 2u, 75u,
            "too few hex digits, expected 64 hex digits");
    at -= 1u;
    put(buf, &at, "aa\n");
    refused("65 hex digits", (const uint8_t *)buf, at, 2u, 76u,
            "too many hex digits, expected 64 hex digits");

    /* A 33rd header line. */
    at = 0u;
    put(buf, &at, "muhtari-movie 1\n");
    for (uint32_t i = 0; i < 31u; i++) {
        put(buf, &at, "#\n");
    }
    put(buf, &at, "port1 none\n");
    refused("33rd header line", (const uint8_t *)buf, at, 33u, 1u,
            "too many header lines, expected at most 32 header lines");

    /* 1,000,000 frames are allowed and the 1,000,001st is refused. */
    static const char head[] = "muhtari-movie 1\nport1 none\nport2 none\n";
    static const char line[] = "|||...BB|\n";
    size_t n = sizeof head - 1u + (size_t)(MUHTARI__MOVIE_FRAMES_MAX + 1u) * (sizeof line - 1u);
    uint8_t *big = malloc(n);
    if (big == NULL) {
        fprintf(stderr, "out of memory\n");
        check_failures++;
        return;
    }
    memcpy(big, head, sizeof head - 1u);
    for (size_t i = sizeof head - 1u; i < n; i += sizeof line - 1u) {
        memcpy(big + i, line, sizeof line - 1u);
    }
    muhtari__movie_header h;
    muhtari__movie_error err;
    uint32_t count = 0u;
    CHECK_EQ_U64(muhtari__movie_parse(big, n - (sizeof line - 1u), &h, NULL, 0u, &count, &err), 0);
    CHECK_EQ_U64(count, MUHTARI__MOVIE_FRAMES_MAX);
    refused("1,000,001st frame", big, n, MUHTARI__MOVIE_FRAMES_MAX + 4u, 1u,
            "too many frames, expected at most 1000000 frames");
    free(big);

    /* A file over 32 MiB is refused by its size, before any byte is read. */
    uint8_t *huge = calloc((size_t)MUHTARI__MOVIE_BYTES_MAX + 1u, 1u);
    if (huge == NULL) {
        fprintf(stderr, "out of memory\n");
        check_failures++;
        return;
    }
    refused("over 32 MiB", huge, (size_t)MUHTARI__MOVIE_BYTES_MAX + 1u, 1u, 1u,
            "file over 32 MiB, expected at most 33554432 bytes");
    free(huge);
}

/* Every typed payload decodes complete state, independently of whether
   this core revision implements the corresponding physical device. */
static void typed(void)
{
    static const struct {
        const char *device, *payload;
        uint32_t id;
    } good_rows[] = {
        {"proline", "UDLR12", MUHTARI_PORT_PROLINE},
        {"joystick", "UDLR1", MUHTARI_PORT_JOYSTICK},
        {"none", "", MUHTARI_PORT_UNPLUGGED},
        {"savekey", "", MUHTARI_PORT_SAVEKEY},
        {"atarivox", "", MUHTARI_PORT_ATARIVOX},
        {"lightgun", "319,241,3", MUHTARI_PORT_LIGHTGUN},
        {"paddle", "65535,123,3", MUHTARI_PORT_PADDLE},
        {"driving", "-2147483648,0,1", MUHTARI_PORT_DRIVING},
        {"trakball", "2147483647,-2147483648,1", MUHTARI_PORT_TRAKBALL},
        {"st-mouse", "-2147483648,2147483647,3", MUHTARI_PORT_ST_MOUSE},
        {"amiga-mouse", "-2147483648,-2147483648,3", MUHTARI_PORT_AMIGA_MOUSE},
        {"keypad", "4095", MUHTARI_PORT_KEYPAD},
        {"snes2atari", "4095", MUHTARI_PORT_SNES2ATARI},
        {"mega7800", "4095", MUHTARI_PORT_MEGA7800},
    };
    char text[512];
    muhtari__movie_header h;
    muhtari__movie_error err;
    muhtari__movie_cursor cursor;
    muhtari_input_ex in;
    uint32_t count;
    for (size_t i = 0u; i < sizeof good_rows / sizeof good_rows[0]; i++) {
        const char *d = good_rows[i].device;
        snprintf(text, sizeof text, "muhtari-movie 2\r\nport1 %s\r\nport2 %s\r\n|%s:%s|%s:%s|RSPAA|\r\n",
                 d, d, d, good_rows[i].payload, d, good_rows[i].payload);
        CHECK_EQ_U64(muhtari__movie_validate_ex((const uint8_t *)text, strlen(text), &h,
                                               &count, &cursor, &err), 0);
        CHECK_EQ_U64(count, 1u);
        CHECK_EQ_U64(h.version, 2u);
        CHECK_EQ_U64(cursor.frame, 0u);
        CHECK_EQ_U64(muhtari__movie_next_ex((const uint8_t *)text, strlen(text), &h,
                                           &cursor, &in, &err), 1);
        CHECK_EQ_U64(cursor.frame, 1u);
        CHECK_EQ_U64(in.size, sizeof in);
        CHECK_EQ_U64(in.port[0].device, good_rows[i].id);
        CHECK(memcmp(&in.port[0], &in.port[1], sizeof in.port[0]) == 0);
        CHECK_EQ_U64(in.switches, 31u);
        switch (in.port[0].device) {
        case MUHTARI_PORT_LIGHTGUN:
            CHECK_EQ_U64(in.port[0].gun_x, 319u);
            CHECK_EQ_U64(in.port[0].gun_y, 241u);
            CHECK_EQ_U64(in.port[0].flags, MUHTARI_INPUT_GUN_OFFSCREEN);
            CHECK_EQ_U64(in.port[0].buttons, MUHTARI_BUTTON_1);
            break;
        case MUHTARI_PORT_PADDLE:
            CHECK_EQ_U64(in.port[0].paddle[0], 65535u);
            CHECK_EQ_U64(in.port[0].paddle[1], 123u);
            CHECK_EQ_U64(in.port[0].buttons, MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2);
            break;
        case MUHTARI_PORT_DRIVING:
            CHECK(in.port[0].dx == INT32_MIN);
            CHECK_EQ_U64(in.port[0].dy, 0u);
            CHECK_EQ_U64(in.port[0].buttons, MUHTARI_BUTTON_1);
            break;
        case MUHTARI_PORT_TRAKBALL:
            CHECK(in.port[0].dx == INT32_MAX && in.port[0].dy == INT32_MIN);
            break;
        case MUHTARI_PORT_ST_MOUSE:
            CHECK(in.port[0].dx == INT32_MIN && in.port[0].dy == INT32_MAX);
            CHECK_EQ_U64(in.port[0].buttons, MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2);
            break;
        case MUHTARI_PORT_AMIGA_MOUSE:
            CHECK(in.port[0].dx == INT32_MIN && in.port[0].dy == INT32_MIN);
            CHECK_EQ_U64(in.port[0].buttons, MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2);
            /* 37 + 37 + four delimiters + five switches = 83. */
            CHECK_EQ_U64(strlen(strchr(text, '|')) - 2u, 83u);
            break;
        case MUHTARI_PORT_KEYPAD:
            CHECK_EQ_U64(in.port[0].keypad, 4095u);
            break;
        case MUHTARI_PORT_SNES2ATARI:
        case MUHTARI_PORT_MEGA7800:
            CHECK_EQ_U64(in.port[0].buttons, 4095u);
            break;
        default:
            break;
        }
        muhtari_input_ex last = in;
        CHECK_EQ_U64(muhtari__movie_next_ex((const uint8_t *)text, strlen(text), &h,
                                           &cursor, &in, &err), 0);
        CHECK_EQ_U64(in.switches, 24u);
        for (unsigned p = 0u; p < 2u; p++) {
            CHECK_EQ_U64(in.port[p].buttons | in.port[p].keypad, 0u);
            CHECK(in.port[p].dx == 0 && in.port[p].dy == 0);
            CHECK_EQ_U64(in.port[p].paddle[0], last.port[p].paddle[0]);
            CHECK_EQ_U64(in.port[p].paddle[1], last.port[p].paddle[1]);
            CHECK_EQ_U64(in.port[p].gun_x, last.port[p].gun_x);
            CHECK_EQ_U64(in.port[p].gun_y, last.port[p].gun_y);
            CHECK_EQ_U64(in.port[p].flags, last.port[p].flags);
        }
    }
    static const struct { const char *d, *v; uint32_t col; } faults[] = {
        {"driving", "+1,0,0", 10}, {"driving", "01,0,0", 11},
        {"driving", "-0,0,0", 10}, {"driving", " 1,0,0", 10},
        {"driving", "1 ,0,0", 11}, {"driving", "2147483648,0,0", 19},
        {"driving", "-2147483649,0,0", 20}, {"driving", "0,1,0", 12},
        {"driving", "0,-0,0", 12}, {"driving", "0,0,2", 14},
        {"driving", "0,0", 13}, {"driving", "0,0,0,0", 15},
        {"driving", ",0,0", 10}, {"driving", "0,,0", 12},
        {"lightgun", "320,0,2", 13}, {"lightgun", "0,242,2", 15},
        {"lightgun", "0,0,4", 15}, {"paddle", "65536,0,0", 13},
        {"paddle", "0,0,4", 13}, {"keypad", "4096", 12},
        {"snes2atari", "4096", 16}, {"mega7800", "-1", 11},
        {"none", "0", 7}, {"proline", "D.....", 10},
        {"trakball", "0,0,2", 15}, {"amiga-mouse", "0,0,4", 18},
    };
    for (size_t i = 0u; i < sizeof faults / sizeof faults[0]; i++) {
        snprintf(text, sizeof text, "muhtari-movie 2\nport1 %s\nport2 none\n|%s:%s|none:|...BB|\n",
                 faults[i].d, faults[i].d, faults[i].v);
        CHECK_EQ_U64(muhtari__movie_validate_ex((const uint8_t *)text, strlen(text), &h,
                                               &count, &cursor, &err), -1);
        if (err.line != 4u || err.column != faults[i].col) {
            fprintf(stderr, "%s:%s refused %u:%u expected 4:%u\n", faults[i].d, faults[i].v,
                    (unsigned)err.line, (unsigned)err.column, (unsigned)faults[i].col);
            check_failures++;
        }
    }
    snprintf(text, sizeof text, "muhtari-movie 2\nport1 amiga-mouse\nport2 amiga-mouse\n"
             "|amiga-mouse:-2147483648,-2147483648,3|amiga-mouse:-2147483648,-2147483648,3|RSPAA|x\n");
    CHECK_EQ_U64(muhtari__movie_validate_ex((const uint8_t *)text, strlen(text), &h,
                                           &count, &cursor, &err), -1);
    CHECK_EQ_U64(err.line, 4u);
    CHECK_EQ_U64(err.column, 84u);
    CHECK(strcmp(err.what, "line over 83 bytes") == 0);
    static const char empty[] = "muhtari-movie 2\nport1 lightgun\nport2 paddle\n";
    CHECK_EQ_U64(muhtari__movie_validate_ex((const uint8_t *)empty, strlen(empty), &h,
                                           &count, &cursor, &err), 0);
    CHECK_EQ_U64(count, 0u);
    CHECK_EQ_U64(muhtari__movie_next_ex((const uint8_t *)empty, strlen(empty), &h,
                                       &cursor, &in, &err), 0);
    CHECK_EQ_U64(in.port[0].flags, MUHTARI_INPUT_GUN_OFFSCREEN);
}

int main(void)
{
    for (size_t i = 0; i < sizeof bads / sizeof bads[0]; i++) {
        const bad *b = &bads[i];
        char name[32];
        snprintf(name, sizeof name, "row %u", (unsigned)i);
        refused(name, (const uint8_t *)b->text, b->n != 0u ? b->n : strlen(b->text), b->line,
                b->column, b->message);
    }
    good();
    limits();
    typed();
    CHECK_DONE();
}
