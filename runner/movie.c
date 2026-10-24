/* The muhtari-movie 1 reader (docs/runner.md, Movies). Pure: it reads the
   caller's bytes and writes the caller's structs, and allocates nothing.
   Every byte is checked before it is parsed, and a refusal names the first
   fault in file order. */
#include "movie.h"

#include <string.h>

/* One line of the file, without its line ending. */
typedef struct movie_line {
    const uint8_t *s;
    uint32_t len;
    uint32_t number;
} movie_line;

/* The header keys. */
enum { KEY_ROM, KEY_PORT1, KEY_PORT2, KEY_REGION, KEY_BOOT, KEY_BIOS, KEY_BEHAVIOUR, KEY_COUNT };

static const struct {
    const char *name;
    const char *expected; /* the values it takes */
} movie_keys[KEY_COUNT] = {
    {"rom-sha256", "64 hex digits"},
    {"port1", "proline, joystick, none, savekey or atarivox"},
    {"port2", "proline, joystick, none, savekey or atarivox"},
    {"region", "ntsc"},
    {"boot", "fast or bios"},
    {"bios-sha256", "64 hex digits"},
    {"behaviour", "a revision number from 1 to 999999999"},
};

#define MOVIE_FIRST "muhtari-movie 1"
#define MOVIE_HASH_DIGITS 64u

/* What the header has set so far. */
typedef struct movie_state {
    uint8_t seen[KEY_COUNT];
    uint32_t boot_line;
    uint32_t header_lines;
} movie_state;

static int movie_fail(muhtari__movie_error *err, uint32_t line, uint32_t column, const char *what,
                      const char *expected)
{
    err->line = line;
    err->column = column;
    err->what = what;
    err->expected = expected;
    return -1;
}

static int movie_has(const uint8_t *s, uint32_t n, const char *t)
{
    size_t tn = strlen(t);
    return n >= tn && memcmp(s, t, tn) == 0;
}

static int movie_equals(const uint8_t *s, uint32_t n, const char *t)
{
    return strlen(t) == n && movie_has(s, n, t);
}

/* The longest a line may be: 64 bytes, or a hash key, its space and 64
   hex digits, which is longer. */
static uint32_t movie_limit(const uint8_t *s, uint32_t n)
{
    for (uint32_t k = 0; k < KEY_COUNT; k++) {
        if (k != KEY_ROM && k != KEY_BIOS) {
            continue;
        }
        size_t kn = strlen(movie_keys[k].name);
        if (n > kn && movie_has(s, n, movie_keys[k].name) && s[kn] == ' ') {
            return (uint32_t)kn + 1u + MOVIE_HASH_DIGITS;
        }
    }
    return MUHTARI__MOVIE_LINE_MAX;
}

/* Takes the line at *pos: its bytes up to an LF, less a CR right before
   the LF. Each byte must be printable ASCII, and the line within its
   limit. */
static int movie_next_line(const uint8_t *b, size_t n, size_t *pos, uint32_t number, movie_line *l,
                           uint32_t record_limit, muhtari__movie_error *err)
{
    size_t start = *pos;
    size_t end = start;
    while (end < n && b[end] != '\n') {
        end++;
    }
    size_t len = end - start;
    if (len > 0u && end < n && b[end - 1u] == '\r') {
        len--;
    }
    *pos = end < n ? end + 1u : end;
    l->s = b + start;
    l->number = number;
    /* The file is at most 32 MiB, so a line's length fits 32 bits. */
    uint32_t limit = movie_limit(l->s, (uint32_t)len);
    if (len != 0u && l->s[0] == '|') {
        limit = record_limit;
    }
    for (uint32_t i = 0; i < len && i < limit; i++) {
        uint8_t c = l->s[i];
        if (c == 0u) {
            return movie_fail(err, number, i + 1u, "NUL byte", "printable ASCII");
        }
        if (c == '\r') {
            return movie_fail(err, number, i + 1u, "bare CR", "LF after a CR");
        }
        if (c >= 0x80u) {
            return movie_fail(err, number, i + 1u, "byte above $7F", "printable ASCII");
        }
        if (c < 0x20u || c == 0x7Fu) {
            return movie_fail(err, number, i + 1u, "control character", "printable ASCII");
        }
    }
    if (len > limit) {
        if (limit == MUHTARI__MOVIE_LINE_V2_MAX) {
            return movie_fail(err, number, limit + 1u, "line over 83 bytes", "at most 83 bytes");
        }
        if (limit != MUHTARI__MOVIE_LINE_MAX) {
            return movie_fail(err, number, limit + 1u, "too many hex digits", "64 hex digits");
        }
        return movie_fail(err, number, limit + 1u, "line over 64 bytes", "at most 64 bytes");
    }
    l->len = (uint32_t)len;
    return 0;
}

static const struct {
    const char *name;
    uint32_t device;
} movie_devices[] = {
    {"proline", MUHTARI_PORT_PROLINE}, {"joystick", MUHTARI_PORT_JOYSTICK},
    {"none", MUHTARI_PORT_UNPLUGGED}, {"savekey", MUHTARI_PORT_SAVEKEY},
    {"atarivox", MUHTARI_PORT_ATARIVOX}, {"lightgun", MUHTARI_PORT_LIGHTGUN},
    {"paddle", MUHTARI_PORT_PADDLE}, {"trakball", MUHTARI_PORT_TRAKBALL},
    {"driving", MUHTARI_PORT_DRIVING}, {"keypad", MUHTARI_PORT_KEYPAD},
    {"st-mouse", MUHTARI_PORT_ST_MOUSE}, {"amiga-mouse", MUHTARI_PORT_AMIGA_MOUSE},
    {"snes2atari", MUHTARI_PORT_SNES2ATARI}, {"mega7800", MUHTARI_PORT_MEGA7800},
};

static int movie_device(const uint8_t *s, uint32_t n, uint32_t *device)
{
    for (size_t i = 0; i < sizeof movie_devices / sizeof movie_devices[0]; i++) {
        if (movie_equals(s, n, movie_devices[i].name)) {
            *device = movie_devices[i].device;
            return 1;
        }
    }
    return 0;
}

static int movie_hex(uint8_t c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

/* 64 hex digits into 32 bytes; column is the first digit's. */
static int movie_hash(const movie_line *l, const uint8_t *v, uint32_t vn, uint32_t column,
                      uint8_t out[32], muhtari__movie_error *err)
{
    for (uint32_t i = 0; i < vn; i++) {
        if (movie_hex(v[i]) < 0) {
            return movie_fail(err, l->number, column + i, "not a hex digit", "64 hex digits");
        }
    }
    if (vn < MOVIE_HASH_DIGITS) {
        return movie_fail(err, l->number, column + vn, "too few hex digits", "64 hex digits");
    }
    for (uint32_t i = 0; i < 32u; i++) {
        out[i] = (uint8_t)((movie_hex(v[2u * i]) << 4) | movie_hex(v[2u * i + 1u]));
    }
    return 0;
}

/* A revision: 1 to 9 decimal digits, no leading zero. */
static int movie_revision(const uint8_t *v, uint32_t vn, uint32_t *out)
{
    if (vn == 0u || vn > 9u || v[0] == '0') {
        return 0;
    }
    uint32_t r = 0u;
    for (uint32_t i = 0; i < vn; i++) {
        if (v[i] < '0' || v[i] > '9') {
            return 0;
        }
        r = r * 10u + (uint32_t)(v[i] - '0');
    }
    *out = r;
    return 1;
}

/* A header line: key, one space, value. */
static int movie_header_line(const movie_line *l, muhtari__movie_header *h, movie_state *st,
                             muhtari__movie_error *err)
{
    uint32_t kn = 0u;
    while (kn < l->len && l->s[kn] != ' ') {
        kn++;
    }
    uint32_t k = 0u;
    while (k < KEY_COUNT && !movie_equals(l->s, kn, movie_keys[k].name)) {
        k++;
    }
    if (k == KEY_COUNT) {
        return movie_fail(err, l->number, 1u, "unknown key",
                          "rom-sha256, port1, port2, region, boot, bios-sha256 or behaviour");
    }
    if (st->seen[k] != 0u) {
        return movie_fail(err, l->number, 1u, "duplicate key", "each key once");
    }
    if (kn + 1u >= l->len) {
        return movie_fail(err, l->number, l->len + 1u, "missing value", movie_keys[k].expected);
    }
    st->seen[k] = 1u;
    const uint8_t *v = l->s + kn + 1u;
    uint32_t vn = l->len - kn - 1u;
    uint32_t col = kn + 2u;
    const char *expected = movie_keys[k].expected;
    switch (k) {
    case KEY_ROM:
        h->has_rom_sha256 = 1u;
        return movie_hash(l, v, vn, col, h->rom_sha256, err);
    case KEY_BIOS:
        h->has_bios_sha256 = 1u;
        return movie_hash(l, v, vn, col, h->bios_sha256, err);
    case KEY_PORT1:
    case KEY_PORT2: {
        uint32_t p = k == KEY_PORT1 ? 0u : 1u;
        if (h->version == 2u) {
            if (!movie_device(v, vn, &h->device[p])) {
                return movie_fail(err, l->number, col, "unknown device", "an explicit movie2 device");
            }
            return 0;
        }
        if (movie_equals(v, vn, "proline")) {
            h->device[p] = MUHTARI_PORT_PROLINE;
        } else if (movie_equals(v, vn, "joystick")) {
            h->device[p] = MUHTARI_PORT_JOYSTICK;
        } else if (movie_equals(v, vn, "none")) {
            h->device[p] = MUHTARI_PORT_UNPLUGGED;
        } else if (movie_equals(v, vn, "savekey")) {
            h->device[p] = MUHTARI_PORT_SAVEKEY;
        } else if (movie_equals(v, vn, "atarivox")) {
            h->device[p] = MUHTARI_PORT_ATARIVOX;
        } else if (movie_equals(v, vn, "auto")) {
            /* A movie names its devices; auto is the command line's. */
            return movie_fail(err, l->number, col, "auto", expected);
        } else {
            return movie_fail(err, l->number, col, "unknown device", expected);
        }
        return 0;
    }
    case KEY_REGION:
        if (movie_equals(v, vn, "pal")) {
            return movie_fail(err, l->number, col, "pal is not supported", expected);
        }
        if (!movie_equals(v, vn, "ntsc")) {
            return movie_fail(err, l->number, col, "unknown region", expected);
        }
        h->has_region = 1u;
        h->region = MUHTARI_REGION_NTSC;
        return 0;
    case KEY_BOOT:
        if (movie_equals(v, vn, "fast")) {
            h->boot = MUHTARI__MOVIE_BOOT_FAST;
        } else if (movie_equals(v, vn, "bios")) {
            h->boot = MUHTARI__MOVIE_BOOT_BIOS;
        } else {
            return movie_fail(err, l->number, col, "unknown boot", expected);
        }
        h->has_boot = 1u;
        st->boot_line = l->number;
        return 0;
    default:
        if (!movie_revision(v, vn, &h->behaviour)) {
            return movie_fail(err, l->number, col, "not a revision number", expected);
        }
        h->has_behaviour = 1u;
        return 0;
    }
}

/* The header ends at the first frame line, or at the end of the file,
   whose line is given: both ports must be named, and boot bios needs
   bios-sha256. */
static int movie_header_end(const muhtari__movie_header *h, const movie_state *st, uint32_t line,
                            muhtari__movie_error *err)
{
    if (st->seen[KEY_PORT1] == 0u) {
        return movie_fail(err, line, 1u, "port1 missing", "a port1 line");
    }
    if (st->seen[KEY_PORT2] == 0u) {
        return movie_fail(err, line, 1u, "port2 missing", "a port2 line");
    }
    if (h->has_boot != 0u && h->boot == MUHTARI__MOVIE_BOOT_BIOS && st->seen[KEY_BIOS] == 0u) {
        return movie_fail(err, st->boot_line, 6u, "boot bios without bios-sha256",
                          "a bios-sha256 line");
    }
    return 0;
}

/* What each position of a field takes. */
static const char *movie_expected(char letter)
{
    switch (letter) {
    case 'U':
        return "U or .";
    case 'D':
        return "D or .";
    case 'L':
        return "L or .";
    case 'R':
        return "R or .";
    case '1':
        return "1 or .";
    case '2':
        return "2 or .";
    case 'S':
        return "S or .";
    case 'P':
        return "P or .";
    default:
        return "A or B";
    }
}

/* One field at *at, then its closing |: position i holds letters[i],
   which sets bits[i], or others[i], which sets nothing. */
static int movie_field(const movie_line *l, uint32_t *at, const char *letters, const char *others,
                       const uint32_t *bits, uint32_t *value, muhtari__movie_error *err)
{
    uint32_t n = (uint32_t)strlen(letters);
    *value = 0u;
    for (uint32_t i = 0; i < n; i++) {
        uint32_t c = *at + i;
        const char *expected = movie_expected(letters[i]);
        if (c >= l->len) {
            return movie_fail(err, l->number, c + 1u, "end of line", expected);
        }
        if (l->s[c] == '|') {
            return movie_fail(err, l->number, c + 1u, "field too short", expected);
        }
        if (l->s[c] == (uint8_t)letters[i]) {
            *value |= bits[i];
        } else if (l->s[c] != (uint8_t)others[i]) {
            return movie_fail(err, l->number, c + 1u, "letter out of place", expected);
        }
    }
    *at += n;
    if (*at >= l->len) {
        return movie_fail(err, l->number, *at + 1u, "end of line", "|");
    }
    if (l->s[*at] != '|') {
        return movie_fail(err, l->number, *at + 1u, "field too long", "|");
    }
    *at += 1u;
    return 0;
}

/* A frame line: |port 1|port 2|console|. A ProLine field is UDLR12, a
   joystick's UDLR1 and none's empty, each letter or a dot; the console's
   is RSP, each letter or a dot, then the left and right difficulty as A or
   B. */
static int movie_frame_line(const movie_line *l, const muhtari__movie_header *h, muhtari_input *in,
                            muhtari__movie_error *err)
{
    static const uint32_t buttons[6] = {MUHTARI_BUTTON_UP,   MUHTARI_BUTTON_DOWN,
                                        MUHTARI_BUTTON_LEFT, MUHTARI_BUTTON_RIGHT,
                                        MUHTARI_BUTTON_1,    MUHTARI_BUTTON_2};
    static const uint32_t switches[5] = {MUHTARI_SWITCH_RESET, MUHTARI_SWITCH_SELECT,
                                         MUHTARI_SWITCH_PAUSE, MUHTARI_SWITCH_LEFT_DIFFICULTY_A,
                                         MUHTARI_SWITCH_RIGHT_DIFFICULTY_A};
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    uint32_t at = 1u;
    for (uint32_t p = 0; p < 2u; p++) {
        uint32_t d = h->device[p];
        const char *letters = d == MUHTARI_PORT_PROLINE    ? "UDLR12"
                              : d == MUHTARI_PORT_JOYSTICK ? "UDLR1"
                                                           : "";
        in->device[p] = d;
        if (movie_field(l, &at, letters, "......", buttons, &in->buttons[p], err) != 0) {
            return -1;
        }
    }
    if (movie_field(l, &at, "RSPAA", "...BB", switches, &in->switches, err) != 0) {
        return -1;
    }
    if (at != l->len) {
        return movie_fail(err, l->number, at + 1u, "text after the last field", "end of line");
    }
    return 0;
}

/* Line 1: exactly muhtari-movie 1. */
static int movie_first_line(const movie_line *l, muhtari__movie_error *err)
{
    static const char prefix[] = "muhtari-movie ";
    if (movie_equals(l->s, l->len, MOVIE_FIRST)) {
        return 0;
    }
    if (l->len > sizeof prefix - 1u && movie_has(l->s, l->len, prefix)) {
        return movie_fail(err, 1u, (uint32_t)sizeof prefix, "unknown version", "1");
    }
    return movie_fail(err, 1u, 1u, "not a movie", MOVIE_FIRST);
}

/* A line after the first frame line must be a frame line too. */
static int movie_frames_only(const movie_line *l, muhtari__movie_error *err)
{
    if (l->len == 0u) {
        return movie_fail(err, l->number, 1u, "blank line after the frames", "a frame line");
    }
    if (l->s[0] == '#') {
        return movie_fail(err, l->number, 1u, "comment after the frames", "a frame line");
    }
    if (l->s[0] != '|') {
        return movie_fail(err, l->number, 1u, "not a frame line", "|");
    }
    return 0;
}

int muhtari__movie_parse(const uint8_t *bytes, size_t n, muhtari__movie_header *h,
                         muhtari_input *frames, uint32_t cap, uint32_t *count,
                         muhtari__movie_error *err)
{
    memset(h, 0, sizeof *h);
    *count = 0u;
    if (n > MUHTARI__MOVIE_BYTES_MAX) {
        return movie_fail(err, 1u, 1u, "file over 32 MiB", "at most 33554432 bytes");
    }
    if (n == 0u) {
        return movie_fail(err, 1u, 1u, "empty file", MOVIE_FIRST);
    }
    if (n >= 3u && bytes[0] == 0xEFu && bytes[1] == 0xBBu && bytes[2] == 0xBFu) {
        return movie_fail(err, 1u, 1u, "byte order mark", MOVIE_FIRST);
    }
    movie_state st;
    memset(&st, 0, sizeof st);
    int in_frames = 0;
    size_t pos = 0u;
    uint32_t number = 0u;
    while (pos < n) {
        movie_line l;
        number++;
        if (movie_next_line(bytes, n, &pos, number, &l, MUHTARI__MOVIE_LINE_MAX, err) != 0) {
            return -1;
        }
        if (number == 1u) {
            if (movie_first_line(&l, err) != 0) {
                return -1;
            }
            h->version = 1u;
            st.header_lines = 1u;
            continue;
        }
        if (!in_frames) {
            if (l.len == 0u || l.s[0] != '|') {
                st.header_lines++;
                if (st.header_lines > MUHTARI__MOVIE_HEADER_LINES_MAX) {
                    return movie_fail(err, number, 1u, "too many header lines",
                                      "at most 32 header lines");
                }
                if (l.len == 0u || l.s[0] == '#') {
                    continue;
                }
                if (movie_header_line(&l, h, &st, err) != 0) {
                    return -1;
                }
                continue;
            }
            if (movie_header_end(h, &st, number, err) != 0) {
                return -1;
            }
            in_frames = 1;
        } else if (movie_frames_only(&l, err) != 0) {
            return -1;
        }
        if (*count >= MUHTARI__MOVIE_FRAMES_MAX) {
            return movie_fail(err, number, 1u, "too many frames", "at most 1000000 frames");
        }
        muhtari_input in;
        if (movie_frame_line(&l, h, &in, err) != 0) {
            return -1;
        }
        if (frames != NULL) {
            if (*count >= cap) {
                return movie_fail(err, number, 1u, "more frames than the buffer holds",
                                  "a larger buffer");
            }
            frames[*count] = in;
        }
        *count += 1u;
    }
    if (!in_frames) {
        return movie_header_end(h, &st, number + 1u, err);
    }
    return 0;
}

/* Decimal fields are canonical and bounded before multiplication. Signed
   magnitudes include INT32_MIN without negating it or overflowing int32. */
static int movie_decimal(const movie_line *l, uint32_t *at, int is_signed, uint32_t max,
                         int32_t *value, muhtari__movie_error *err)
{
    uint32_t start = *at, i = start, magnitude = 0u;
    int negative = i < l->len && l->s[i] == '-';
    if (negative && is_signed) {
        i++;
    }
    uint32_t digits = i;
    uint32_t limit = is_signed ? (negative ? 2147483648u : 2147483647u) : max;
    if (i >= l->len || l->s[i] < '0' || l->s[i] > '9') {
        return movie_fail(err, l->number, i + 1u, "not a decimal", "a canonical decimal integer");
    }
    while (i < l->len && l->s[i] != ',' && l->s[i] != '|') {
        uint8_t c = l->s[i];
        if (c < '0' || c > '9' || (i > digits && l->s[digits] == '0')) {
            return movie_fail(err, l->number, i + 1u, "not a canonical decimal",
                              "no padding, plus sign or leading zero");
        }
        uint32_t digit = (uint32_t)(c - '0');
        if (digit > limit || magnitude > (limit - digit) / 10u) {
            return movie_fail(err, l->number, i + 1u, "decimal out of range", "the device's range");
        }
        magnitude = magnitude * 10u + digit;
        i++;
    }
    if (negative && magnitude == 0u) {
        return movie_fail(err, l->number, start + 1u, "negative zero", "0");
    }
    *value = negative ? -(int32_t)(magnitude - 1u) - 1 : (int32_t)magnitude;
    *at = i;
    return 0;
}

static int movie_separator(const movie_line *l, uint32_t *at, uint8_t c,
                           muhtari__movie_error *err)
{
    if (*at >= l->len || l->s[*at] != c) {
        return movie_fail(err, l->number, *at + 1u, "missing delimiter", c == ',' ? "," : "|");
    }
    *at += 1u;
    return 0;
}

static int movie_frame_ex(const movie_line *l, const muhtari__movie_header *h,
                          muhtari_input_ex *in, muhtari__movie_error *err)
{
    static const uint32_t buttons[6] = {MUHTARI_BUTTON_UP, MUHTARI_BUTTON_DOWN,
        MUHTARI_BUTTON_LEFT, MUHTARI_BUTTON_RIGHT, MUHTARI_BUTTON_1, MUHTARI_BUTTON_2};
    static const uint32_t switches[5] = {MUHTARI_SWITCH_RESET, MUHTARI_SWITCH_SELECT,
        MUHTARI_SWITCH_PAUSE, MUHTARI_SWITCH_LEFT_DIFFICULTY_A, MUHTARI_SWITCH_RIGHT_DIFFICULTY_A};
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    uint32_t at = 1u;
    for (uint32_t p = 0u; p < 2u; p++) {
        muhtari_port_input *port = &in->port[p];
        uint32_t start = at;
        while (at < l->len && l->s[at] != ':' && l->s[at] != '|') {
            at++;
        }
        if (at == l->len || l->s[at] != ':') {
            return movie_fail(err, l->number, at + 1u, "missing device tag", "device:payload");
        }
        if (!movie_device(l->s + start, at - start, &port->device)) {
            return movie_fail(err, l->number, start + 1u, "unknown device tag", "the header's device");
        }
        if (port->device != h->device[p]) {
            return movie_fail(err, l->number, start + 1u, "device tag mismatch", "the header's device");
        }
        at++;
        uint32_t d = port->device;
        if (d == MUHTARI_PORT_PROLINE || d == MUHTARI_PORT_JOYSTICK ||
            d == MUHTARI_PORT_UNPLUGGED || d == MUHTARI_PORT_SAVEKEY || d == MUHTARI_PORT_ATARIVOX) {
            const char *letters = d == MUHTARI_PORT_PROLINE ? "UDLR12" :
                                  d == MUHTARI_PORT_JOYSTICK ? "UDLR1" : "";
            if (movie_field(l, &at, letters, "......", buttons, &port->buttons, err) != 0) {
                return -1;
            }
            continue;
        }
        int32_t v[3] = {0, 0, 0};
        int motion = d == MUHTARI_PORT_DRIVING || d == MUHTARI_PORT_TRAKBALL ||
                     d == MUHTARI_PORT_ST_MOUSE || d == MUHTARI_PORT_AMIGA_MOUSE;
        uint32_t fields = d == MUHTARI_PORT_KEYPAD || d == MUHTARI_PORT_SNES2ATARI ||
                          d == MUHTARI_PORT_MEGA7800 ? 1u : 3u;
        for (uint32_t k = 0u; k < fields; k++) {
            uint32_t max = fields == 1u ? 4095u : k == 2u ? 3u : 65535u;
            if (d == MUHTARI_PORT_LIGHTGUN && k < 2u) {
                max = k == 0u ? 319u : 241u;
            } else if ((d == MUHTARI_PORT_DRIVING || d == MUHTARI_PORT_TRAKBALL) && k == 2u) {
                max = 1u;
            } else if (d == MUHTARI_PORT_DRIVING && k == 1u) {
                max = 0u;
            }
            if (movie_decimal(l, &at, motion && k < 2u && !(d == MUHTARI_PORT_DRIVING && k == 1u),
                              max, &v[k], err) != 0 ||
                movie_separator(l, &at, k + 1u < fields ? ',' : '|', err) != 0) {
                return -1;
            }
        }
        if (d == MUHTARI_PORT_LIGHTGUN) {
            port->gun_x = (uint32_t)v[0];
            port->gun_y = (uint32_t)v[1];
            port->buttons = (v[2] & 1) != 0 ? MUHTARI_BUTTON_1 : 0u;
            port->flags = (v[2] & 2) != 0 ? MUHTARI_INPUT_GUN_OFFSCREEN : 0u;
        } else if (d == MUHTARI_PORT_PADDLE) {
            port->paddle[0] = (uint32_t)v[0];
            port->paddle[1] = (uint32_t)v[1];
        } else if (d == MUHTARI_PORT_KEYPAD) {
            port->keypad = (uint32_t)v[0];
        } else if (fields == 1u) {
            port->buttons = (uint32_t)v[0];
        } else {
            port->dx = v[0];
            port->dy = v[1];
        }
        if (d == MUHTARI_PORT_PADDLE || motion) {
            port->buttons = ((v[2] & 1) != 0 ? MUHTARI_BUTTON_1 : 0u) |
                            ((v[2] & 2) != 0 ? MUHTARI_BUTTON_2 : 0u);
        }
    }
    if (movie_field(l, &at, "RSPAA", "...BB", switches, &in->switches, err) != 0) {
        return -1;
    }
    if (at != l->len) {
        return movie_fail(err, l->number, at + 1u, "text after the last field", "end of line");
    }
    return 0;
}

int muhtari__movie_validate_ex(const uint8_t *bytes, size_t n, muhtari__movie_header *h,
                              uint32_t *count, muhtari__movie_cursor *cursor,
                              muhtari__movie_error *err)
{
    memset(h, 0, sizeof *h);
    memset(cursor, 0, sizeof *cursor);
    *count = 0u;
    if (n > MUHTARI__MOVIE_BYTES_MAX) {
        return movie_fail(err, 1u, 1u, "file over 32 MiB", "at most 33554432 bytes");
    }
    if (n == 0u) {
        return movie_fail(err, 1u, 1u, "empty file", "muhtari-movie 2");
    }
    if (n >= 3u && bytes[0] == 0xEFu && bytes[1] == 0xBBu && bytes[2] == 0xBFu) {
        return movie_fail(err, 1u, 1u, "byte order mark", "muhtari-movie 2");
    }
    movie_state st = {0};
    size_t pos = 0u;
    uint32_t number = 0u;
    int in_frames = 0;
    while (pos < n) {
        size_t start = pos;
        movie_line l;
        number++;
        if (movie_next_line(bytes, n, &pos, number, &l, MUHTARI__MOVIE_LINE_V2_MAX, err) != 0) {
            return -1;
        }
        if (number == 1u) {
            if (!movie_equals(l.s, l.len, "muhtari-movie 2")) {
                return movie_fail(err, 1u, 1u, "not a movie2", "muhtari-movie 2");
            }
            h->version = 2u;
            st.header_lines = 1u;
            continue;
        }
        if (!in_frames) {
            if (l.len == 0u || l.s[0] != '|') {
                if (++st.header_lines > MUHTARI__MOVIE_HEADER_LINES_MAX) {
                    return movie_fail(err, number, 1u, "too many header lines", "at most 32 header lines");
                }
                if (l.len != 0u && l.s[0] != '#' && movie_header_line(&l, h, &st, err) != 0) {
                    return -1;
                }
                continue;
            }
            if (movie_header_end(h, &st, number, err) != 0) {
                return -1;
            }
            cursor->pos = start;
            cursor->line = number;
            in_frames = 1;
        } else if (movie_frames_only(&l, err) != 0) {
            return -1;
        }
        if (*count >= MUHTARI__MOVIE_FRAMES_MAX) {
            return movie_fail(err, number, 1u, "too many frames", "at most 1000000 frames");
        }
        muhtari_input_ex in;
        if (movie_frame_ex(&l, h, &in, err) != 0) {
            return -1;
        }
        *count += 1u;
    }
    if (!in_frames) {
        if (movie_header_end(h, &st, number + 1u, err) != 0) {
            return -1;
        }
        cursor->pos = n;
        cursor->line = number + 1u;
    }
    cursor->last.size = (uint32_t)sizeof cursor->last;
    for (uint32_t p = 0u; p < 2u; p++) {
        cursor->last.port[p].device = h->device[p];
        if (h->device[p] == MUHTARI_PORT_LIGHTGUN) {
            cursor->last.port[p].flags = MUHTARI_INPUT_GUN_OFFSCREEN;
        }
    }
    return 0;
}

int muhtari__movie_next_ex(const uint8_t *bytes, size_t n, const muhtari__movie_header *h,
                          muhtari__movie_cursor *cursor, muhtari_input_ex *in,
                          muhtari__movie_error *err)
{
    if (cursor->pos < n) {
        movie_line l;
        if (movie_next_line(bytes, n, &cursor->pos, cursor->line, &l, MUHTARI__MOVIE_LINE_V2_MAX, err) != 0 ||
            movie_frame_ex(&l, h, in, err) != 0) {
            return -1;
        }
        cursor->last = *in;
        cursor->line++;
        cursor->frame++;
        return 1;
    }
    *in = cursor->last;
    in->switches &= MUHTARI_SWITCH_LEFT_DIFFICULTY_A | MUHTARI_SWITCH_RIGHT_DIFFICULTY_A;
    for (uint32_t p = 0u; p < 2u; p++) {
        in->port[p].buttons = 0u;
        in->port[p].keypad = 0u;
        in->port[p].dx = 0;
        in->port[p].dy = 0;
    }
    return 0;
}
