/* SingleStepTests JSON to an N65V chunk. Integer-only; the parser never
   reads at or past len. */
#include "sst_json.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n65v.h"

#define KEY_MAX 16u

/* --- Output buffer ------------------------------------------------------ */

struct buf {
    uint8_t *data;
    size_t len;
    size_t cap;
    int oom; /* set when an allocation failed; later puts do nothing */
};

static void put8(struct buf *b, uint32_t v)
{
    if (b->oom) {
        return;
    }
    if (b->len == b->cap) {
        size_t cap = b->cap != 0u ? b->cap * 2u : 65536u;
        uint8_t *data = (uint8_t *)realloc(b->data, cap);
        if (data == NULL) {
            b->oom = 1;
            return;
        }
        b->data = data;
        b->cap = cap;
    }
    b->data[b->len++] = (uint8_t)(v & 0xFFu);
}

static void put16(struct buf *b, uint32_t v)
{
    put8(b, v & 0xFFu);
    put8(b, (v >> 8) & 0xFFu);
}

static void put_state(struct buf *b, const struct n65v_state *s)
{
    uint32_t i;
    put16(b, s->pc);
    put8(b, s->s);
    put8(b, s->a);
    put8(b, s->x);
    put8(b, s->y);
    put8(b, s->p);
    put8(b, s->ram_count);
    for (i = 0; i < (uint32_t)s->ram_count; i++) {
        put16(b, s->ram_addr[i]);
        put8(b, s->ram_value[i]);
    }
}

static void put_test(struct buf *b, const struct n65v_test *t)
{
    uint32_t i;
    put16(b, t->index);
    put_state(b, &t->initial);
    put_state(b, &t->final);
    put8(b, t->cycle_count);
    for (i = 0; i < (uint32_t)t->cycle_count; i++) {
        put16(b, t->cycles[i].addr);
        put8(b, t->cycles[i].value);
        put8(b, t->cycles[i].kind);
    }
}

static void put_header(struct buf *b, uint8_t opcode)
{
    put8(b, 'N');
    put8(b, '6');
    put8(b, '5');
    put8(b, 'V');
    put8(b, N65V_VERSION);
    put8(b, opcode);
    put16(b, 0u); /* count, patched at the end */
}

/* --- Schema tokenizer ----------------------------------------------------- */

struct parser {
    const uint8_t *buf;
    size_t len;
    size_t pos;
    char *err;
    size_t err_size;
    int failed;
};

/* Records the first error only; returns 0 so callers can return its value. */
static int fail(struct parser *p, size_t off, const char *fmt, ...)
{
    char msg[120];
    va_list ap;
    if (p->failed) {
        return 0;
    }
    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);
    if (p->err_size != 0u) {
        snprintf(p->err, p->err_size, "offset %zu: %s", off, msg);
    }
    p->failed = 1;
    return 0;
}

/* The next byte, or -1 at the end of the input. */
static int peek(const struct parser *p)
{
    return p->pos < p->len ? (int)p->buf[p->pos] : -1;
}

static void ws(struct parser *p)
{
    int c = peek(p);
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        p->pos++;
        c = peek(p);
    }
}

static int expect(struct parser *p, int ch)
{
    int c;
    ws(p);
    c = peek(p);
    if (c < 0)
        return fail(p, p->pos, "input ends, expected '%c'", ch);
    if (c != ch)
        return fail(p, p->pos, "expected '%c'", ch);
    p->pos++;
    return 1;
}

/* After a list or object member: ',' continues (returns 1), the closing
   character ends (returns 2), anything else fails (returns 0). */
static int next_member(struct parser *p, int close)
{
    int c;
    ws(p);
    c = peek(p);
    if (c == ',') {
        p->pos++;
        return 1;
    }
    if (c == close) {
        p->pos++;
        return 2;
    }
    if (c < 0)
        return fail(p, p->pos, "input ends, expected ',' or '%c'", close);
    return fail(p, p->pos, "expected ',' or '%c'", close);
}

/* An unsigned decimal integer no greater than max. */
static int parse_uint(struct parser *p, uint32_t max, uint32_t *out)
{
    size_t start;
    uint32_t v = 0;
    int c;
    ws(p);
    start = p->pos;
    c = peek(p);
    if (c == '-' || c == '+')
        return fail(p, start, "signed number");
    if (c < '0' || c > '9')
        return fail(p, start, c < 0 ? "input ends, expected a number" : "expected a number");
    /* JSON (RFC 8259 section 6) allows no leading zero. */
    if (c == '0' && p->pos + 1u < p->len && p->buf[p->pos + 1u] >= '0' &&
        p->buf[p->pos + 1u] <= '9')
        return fail(p, start, "number with a leading zero");
    while (c >= '0' && c <= '9') {
        if (v <= 65536u) /* saturates; anything above 65536 is out of range */
            v = v * 10u + (uint32_t)(c - '0');
        p->pos++;
        c = peek(p);
    }
    if (c == '.')
        return fail(p, p->pos, "fractional number");
    if (c == 'e' || c == 'E')
        return fail(p, p->pos, "number with an exponent");
    if (v > max)
        return fail(p, start, "number above %lu", (unsigned long)max);
    *out = v;
    return 1;
}

/* A string without escapes. Up to KEY_MAX - 1 bytes are copied to out
   (when out is not NULL); *long_out is set if there were more. */
static int parse_string(struct parser *p, char *out, int *long_out)
{
    size_t n = 0;
    int c;
    if (!expect(p, '"'))
        return 0;
    for (;;) {
        c = peek(p);
        if (c < 0)
            return fail(p, p->pos, "input ends inside a string");
        if (c == '"')
            break;
        if (c == '\\')
            return fail(p, p->pos, "escape in a string");
        if (c < 0x20)
            return fail(p, p->pos, "control character in a string");
        if (out != NULL && n + 1u < KEY_MAX)
            out[n] = (char)c;
        else if (long_out != NULL)
            *long_out = 1;
        n++;
        p->pos++;
    }
    p->pos++;
    if (out != NULL)
        out[n < KEY_MAX ? n : KEY_MAX - 1u] = '\0';
    return 1;
}

/* A key and its colon. Returns the key's index in names, or -1 after
   reporting an error. *seen tracks repeats. */
static int parse_key(struct parser *p, const char *const *names, int count, uint32_t *seen)
{
    char key[KEY_MAX];
    int too_long = 0;
    int i;
    size_t at;
    ws(p);
    at = p->pos;
    if (!parse_string(p, key, &too_long))
        return -1;
    for (i = 0; i < count && (too_long || strcmp(key, names[i]) != 0); i++)
        ;
    if (i == count) {
        fail(p, at, "unknown key \"%s%s\"", key, too_long ? "..." : "");
        return -1;
    }
    if (*seen & (1u << i)) {
        fail(p, at, "repeated key \"%s\"", key);
        return -1;
    }
    *seen |= 1u << i;
    if (!expect(p, ':'))
        return -1;
    return i;
}

static int missing_key(struct parser *p, size_t at, const char *const *names, int count,
                       uint32_t seen)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!(seen & (1u << i)))
            return fail(p, at, "missing key \"%s\"", names[i]);
    }
    return 1;
}

/* '[' and, if the list is empty, ']'. Returns 1 for a non-empty list, 2 for
   an empty one, 0 on error. */
static int open_list(struct parser *p)
{
    if (!expect(p, '['))
        return 0;
    ws(p);
    if (peek(p) == ']') {
        p->pos++;
        return 2;
    }
    return 1;
}

static int parse_ram(struct parser *p, struct n65v_state *s)
{
    int r = open_list(p);
    s->ram_count = 0;
    while (r == 1) {
        uint32_t addr, value;
        ws(p);
        if (s->ram_count == 255u)
            return fail(p, p->pos, "ram list longer than 255");
        if (!expect(p, '[') || !parse_uint(p, 0xFFFFu, &addr) || !expect(p, ',') ||
            !parse_uint(p, 0xFFu, &value) || !expect(p, ']'))
            return 0;
        s->ram_addr[s->ram_count] = (uint16_t)addr;
        s->ram_value[s->ram_count] = (uint8_t)value;
        s->ram_count++;
        r = next_member(p, ']');
    }
    return r != 0;
}

static int parse_cycles(struct parser *p, struct n65v_test *t)
{
    int r = open_list(p);
    t->cycle_count = 0;
    while (r == 1) {
        uint32_t addr, value;
        char kind[KEY_MAX];
        int too_long = 0;
        size_t at;
        ws(p);
        if (t->cycle_count == 255u)
            return fail(p, p->pos, "cycle list longer than 255");
        if (!expect(p, '[') || !parse_uint(p, 0xFFFFu, &addr) || !expect(p, ',') ||
            !parse_uint(p, 0xFFu, &value) || !expect(p, ','))
            return 0;
        ws(p);
        at = p->pos;
        if (!parse_string(p, kind, &too_long))
            return 0;
        if (!too_long && strcmp(kind, "read") == 0)
            t->cycles[t->cycle_count].kind = N65V_KIND_READ;
        else if (!too_long && strcmp(kind, "write") == 0)
            t->cycles[t->cycle_count].kind = N65V_KIND_WRITE;
        else
            return fail(p, at, "unknown cycle kind");
        if (!expect(p, ']'))
            return 0;
        t->cycles[t->cycle_count].addr = (uint16_t)addr;
        t->cycles[t->cycle_count].value = (uint8_t)value;
        t->cycle_count++;
        r = next_member(p, ']');
    }
    return r != 0;
}

static const char *const STATE_KEYS[] = {"pc", "s", "a", "x", "y", "p", "ram"};
#define STATE_KEY_COUNT 7

static int parse_state(struct parser *p, struct n65v_state *s)
{
    uint32_t seen = 0, v = 0;
    size_t start;
    int r;
    ws(p);
    start = p->pos;
    if (!expect(p, '{'))
        return 0;
    ws(p);
    r = 1;
    if (peek(p) == '}') {
        p->pos++;
        r = 2;
    }
    while (r == 1) {
        int k = parse_key(p, STATE_KEYS, STATE_KEY_COUNT, &seen);
        int ok;
        if (k < 0)
            return 0;
        switch (k) {
        case 0:
            ok = parse_uint(p, 0xFFFFu, &v);
            s->pc = (uint16_t)v;
            break;
        case 6:
            ok = parse_ram(p, s);
            break;
        default:
            ok = parse_uint(p, 0xFFu, &v);
            if (k == 1)
                s->s = (uint8_t)v;
            else if (k == 2)
                s->a = (uint8_t)v;
            else if (k == 3)
                s->x = (uint8_t)v;
            else if (k == 4)
                s->y = (uint8_t)v;
            else
                s->p = (uint8_t)v;
            break;
        }
        if (!ok)
            return 0;
        r = next_member(p, '}');
    }
    return r != 0 && missing_key(p, start, STATE_KEYS, STATE_KEY_COUNT, seen);
}

static const char *const TEST_KEYS[] = {"name", "initial", "final", "cycles"};
#define TEST_KEY_COUNT 4

static int parse_test(struct parser *p, struct n65v_test *t, uint8_t opcode)
{
    uint32_t seen = 0;
    size_t start, cycles_at = 0;
    int r;
    ws(p);
    start = p->pos;
    if (!expect(p, '{'))
        return 0;
    ws(p);
    r = 1;
    if (peek(p) == '}') {
        p->pos++;
        r = 2;
    }
    while (r == 1) {
        int k = parse_key(p, TEST_KEYS, TEST_KEY_COUNT, &seen);
        int ok;
        if (k < 0)
            return 0;
        if (k == 0) {
            ok = parse_string(p, NULL, NULL);
        } else if (k == 1) {
            ok = parse_state(p, &t->initial);
        } else if (k == 2) {
            ok = parse_state(p, &t->final);
        } else {
            ws(p);
            cycles_at = p->pos;
            ok = parse_cycles(p, t);
        }
        if (!ok)
            return 0;
        r = next_member(p, '}');
    }
    if (r == 0 || !missing_key(p, start, TEST_KEYS, TEST_KEY_COUNT, seen))
        return 0;
    /* Every test starts by fetching its opcode from the initial pc. */
    if (t->cycle_count == 0u || t->cycles[0].kind != N65V_KIND_READ ||
        t->cycles[0].addr != t->initial.pc || t->cycles[0].value != opcode)
        return fail(p, cycles_at, "first cycle is not a read of 0x%02x at the initial pc",
                    (unsigned)opcode);
    return 1;
}

/* Parses the array into out, header included. *count gets the number of
   tests. first is 0 for a whole file. */
static int convert(struct parser *p, uint8_t opcode, uint32_t first, struct buf *out,
                   uint32_t *count)
{
    static struct n65v_test t;
    uint32_t n = 0;
    uint32_t limit = first != 0u ? first : SST_JSON_FULL_COUNT;
    int r;

    put_header(out, opcode);
    r = open_list(p);
    while (r == 1) {
        ws(p);
        if (n == limit)
            return fail(p, p->pos, "more than %lu tests", (unsigned long)limit);
        memset(&t, 0, sizeof t);
        if (!parse_test(p, &t, opcode))
            return 0;
        t.index = (uint16_t)n;
        put_test(out, &t);
        n++;
        if (n == first)
            break; /* a prefix: whatever follows is not read */
        r = next_member(p, ']');
    }
    if (r == 0)
        return 0;
    if (n != limit)
        return fail(p, p->pos, "the array holds %lu tests, expected %lu", (unsigned long)n,
                    (unsigned long)limit);
    if (first == 0u) {
        ws(p);
        if (p->pos != p->len)
            return fail(p, p->pos, "data after the array");
    }
    if (out->oom)
        return fail(p, p->pos, "out of memory");
    out->data[6] = (uint8_t)(n & 0xFFu);
    out->data[7] = (uint8_t)((n >> 8) & 0xFFu);
    *count = n;
    return 1;
}

int sst_json_convert(const uint8_t *buf, size_t len, uint8_t opcode, uint32_t first, uint8_t **out,
                     size_t *out_len, uint32_t *count, char *err, size_t err_size)
{
    struct parser p;
    struct buf b = {NULL, 0u, 0u, 0};
    uint32_t n = 0u;

    memset(&p, 0, sizeof p);
    p.buf = buf;
    p.len = buf != NULL ? len : 0u;
    p.err = err;
    p.err_size = err_size;
    if (err_size != 0u) {
        err[0] = '\0';
    }
    *out = NULL;
    *out_len = 0u;
    *count = 0u;
    if (!convert(&p, opcode, first, &b, &n)) {
        free(b.data);
        return 0;
    }
    *out = b.data;
    *out_len = b.len;
    *count = n;
    return 1;
}
