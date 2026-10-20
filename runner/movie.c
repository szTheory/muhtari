/* The muhtari-movie 1 reader (docs/runner.md, Movies). Pure: it reads the
   caller's bytes and writes the caller's structs, and allocates nothing. */
#include "movie.h"

#include <string.h>

/* One line of the file, without its line ending. */
typedef struct movie_line {
    const uint8_t *s;
    uint32_t len;
    uint32_t number;
} movie_line;

static int movie_fail(muhtari__movie_error *err, uint32_t line, uint32_t column, const char *what,
                      const char *expected)
{
    err->line = line;
    err->column = column;
    err->what = what;
    err->expected = expected;
    return -1;
}

/* Whether the line is exactly the text t. */
static int movie_is(const movie_line *l, const char *t)
{
    size_t n = strlen(t);
    return l->len == n && memcmp(l->s, t, n) == 0;
}

/* The devices a port key names. */
static int movie_device(const uint8_t *v, uint32_t n, uint32_t *device)
{
    static const struct {
        const char *name;
        uint32_t device;
    } names[] = {
        {"proline", MUHTARI_PORT_PROLINE},
        {"joystick", MUHTARI_PORT_JOYSTICK},
        {"none", MUHTARI_PORT_UNPLUGGED},
    };
    for (size_t i = 0; i < sizeof names / sizeof names[0]; i++) {
        if (strlen(names[i].name) == n && memcmp(names[i].name, v, n) == 0) {
            *device = names[i].device;
            return 1;
        }
    }
    return 0;
}

/* A header line: key, one space, value. */
static int movie_header_line(const movie_line *l, muhtari__movie_header *h, uint8_t *seen,
                             muhtari__movie_error *err)
{
    uint32_t key = 0u;
    while (key < l->len && l->s[key] != ' ') {
        key++;
    }
    const uint8_t *v = l->s + key + 1u;
    uint32_t vn = key < l->len ? l->len - key - 1u : 0u;
    if (key == 5u && (memcmp(l->s, "port1", 5) == 0 || memcmp(l->s, "port2", 5) == 0)) {
        uint32_t p = l->s[4] == '1' ? 0u : 1u;
        if (!movie_device(v, vn, &h->device[p])) {
            return movie_fail(err, l->number, key + 2u, "unknown device",
                              "proline, joystick or none");
        }
        seen[p] = 1u;
        return 0;
    }
    return movie_fail(err, l->number, 1u, "unknown key", "port1 or port2");
}

/* The letters each field takes, by position. */
static const char movie_proline[] = "UDLR12";
static const char movie_joystick[] = "UDLR1";
static const char movie_console[] = "RSP";

static const uint32_t movie_bits[6] = {MUHTARI_BUTTON_UP,   MUHTARI_BUTTON_DOWN,
                                       MUHTARI_BUTTON_LEFT, MUHTARI_BUTTON_RIGHT,
                                       MUHTARI_BUTTON_1,    MUHTARI_BUTTON_2};

/* Reads one port field at *at; sets *buttons. */
static int movie_port_field(const movie_line *l, uint32_t *at, uint32_t device, uint32_t *buttons,
                            muhtari__movie_error *err)
{
    const char *letters = device == MUHTARI_PORT_PROLINE    ? movie_proline
                          : device == MUHTARI_PORT_JOYSTICK ? movie_joystick
                                                            : "";
    uint32_t n = (uint32_t)strlen(letters);
    *buttons = 0u;
    for (uint32_t i = 0; i < n; i++) {
        uint32_t c = *at + i;
        if (c >= l->len || l->s[c] == '|') {
            return movie_fail(err, l->number, c + 1u, "field too short", "a letter or .");
        }
        if (l->s[c] == (uint8_t)letters[i]) {
            *buttons |= movie_bits[i];
        } else if (l->s[c] != '.') {
            return movie_fail(err, l->number, c + 1u, "letter out of place", "a letter or .");
        }
    }
    *at += n;
    if (*at >= l->len || l->s[*at] != '|') {
        return movie_fail(err, l->number, *at + 1u, "field too long", "|");
    }
    *at += 1u;
    return 0;
}

/* The console field: RSP then each difficulty as A or B. */
static int movie_console_field(const movie_line *l, uint32_t *at, uint32_t *switches,
                               muhtari__movie_error *err)
{
    static const uint32_t bits[3] = {MUHTARI_SWITCH_RESET, MUHTARI_SWITCH_SELECT,
                                     MUHTARI_SWITCH_PAUSE};
    *switches = 0u;
    for (uint32_t i = 0; i < 5u; i++) {
        uint32_t c = *at + i;
        if (c >= l->len || l->s[c] == '|') {
            return movie_fail(err, l->number, c + 1u, "field too short", "a letter or .");
        }
        uint8_t ch = l->s[c];
        if (i < 3u && ch == (uint8_t)movie_console[i]) {
            *switches |= bits[i];
        } else if (i >= 3u && ch == 'A') {
            *switches |=
                i == 3u ? MUHTARI_SWITCH_LEFT_DIFFICULTY_A : MUHTARI_SWITCH_RIGHT_DIFFICULTY_A;
        } else if (!(i < 3u && ch == '.') && !(i >= 3u && ch == 'B')) {
            return movie_fail(err, l->number, c + 1u, "letter out of place", "a letter or .");
        }
    }
    *at += 5u;
    if (*at >= l->len || l->s[*at] != '|') {
        return movie_fail(err, l->number, *at + 1u, "field too long", "|");
    }
    *at += 1u;
    return 0;
}

static int movie_frame_line(const movie_line *l, const muhtari__movie_header *h, muhtari_input *in,
                            muhtari__movie_error *err)
{
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    uint32_t at = 1u;
    for (uint32_t p = 0; p < 2u; p++) {
        in->device[p] = h->device[p];
        if (movie_port_field(l, &at, h->device[p], &in->buttons[p], err) != 0) {
            return -1;
        }
    }
    if (movie_console_field(l, &at, &in->switches, err) != 0) {
        return -1;
    }
    if (at != l->len) {
        return movie_fail(err, l->number, at + 1u, "text after the last field", "end of line");
    }
    return 0;
}

int muhtari__movie_parse(const uint8_t *bytes, size_t n, muhtari__movie_header *h,
                         muhtari_input *frames, uint32_t cap, uint32_t *count,
                         muhtari__movie_error *err)
{
    memset(h, 0, sizeof *h);
    *count = 0u;
    uint8_t seen[2] = {0u, 0u};
    int in_frames = 0;
    size_t pos = 0;
    uint32_t number = 0u;
    while (pos < n || number == 0u) {
        movie_line l;
        size_t end = pos;
        while (end < n && bytes[end] != '\n') {
            end++;
        }
        l.s = bytes + pos;
        l.len = (uint32_t)(end - pos);
        if (l.len > 0u && l.s[l.len - 1u] == '\r') {
            l.len--;
        }
        l.number = ++number;
        pos = end < n ? end + 1u : end;
        if (number == 1u) {
            if (!movie_is(&l, "muhtari-movie 1")) {
                return movie_fail(err, 1u, 1u, "not a movie", "muhtari-movie 1");
            }
            h->version = 1u;
        } else if (l.len > 0u && l.s[0] == '|') {
            if (!in_frames) {
                if (!seen[0] || !seen[1]) {
                    return movie_fail(err, number, 1u, "a frame before both ports",
                                      "port1 and port2");
                }
                in_frames = 1;
            }
            muhtari_input in;
            if (movie_frame_line(&l, h, &in, err) != 0) {
                return -1;
            }
            if (frames != NULL) {
                if (*count >= cap) {
                    return movie_fail(err, number, 1u, "more frames than the buffer holds",
                                      "at most cap frames");
                }
                frames[*count] = in;
            }
            *count += 1u;
        } else if (movie_header_line(&l, h, seen, err) != 0) {
            return -1;
        }
    }
    return 0;
}
