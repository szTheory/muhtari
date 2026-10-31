#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "input_trace.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static void it_put(uint8_t *p, uint32_t n)
{
    for (unsigned i = 0; i < 4u; i++)
        p[i] = (uint8_t)(n >> (8u * i));
}
static uint32_t it_get(const uint8_t *p)
{
    uint32_t n = 0;
    for (unsigned i = 0; i < 4u; i++)
        n |= (uint32_t)p[i] << (8u * i);
    return n;
}
void it_emit(it_writer *w, uint32_t type, uint32_t a, uint32_t b, uint32_t c)
{
    if (w->disabled || w->count >= IT_LIMIT)
        return;
    if (w->count == IT_LIMIT - 1u) {
        type = IT_SATURATED;
        a = b = c = 0;
    }
    uint8_t bytes[IT_BYTES] = {0};
    memcpy(bytes, "MHTRACE1", 8);
    it_put(bytes + 8, 1);
    it_put(bytes + 12, type);
    it_put(bytes + 16, w->count);
    it_put(bytes + 20, w->role);
    it_put(bytes + 24, w->pid);
    it_put(bytes + 28, w->peer);
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        it_put(bytes + 44, (uint32_t)(errno ? errno : EIO));
    } else {
        uint64_t seconds = (uint64_t)now.tv_sec;
        it_put(bytes + 32, (uint32_t)seconds);
        it_put(bytes + 36, (uint32_t)(seconds >> 32));
        it_put(bytes + 40, (uint32_t)now.tv_nsec);
    }
    it_put(bytes + 48, a);
    it_put(bytes + 52, b);
    it_put(bytes + 56, c);
    /* One write, no retry: a short/error write disables this stream forever. */
    if (write(w->fd, bytes, sizeof bytes) != (ssize_t)sizeof bytes) {
        w->disabled = 1;
        fprintf(stderr, "input trace: write failed or short; stream disabled\n");
        return;
    }
    w->count++;
    if (type == IT_END || type == IT_SATURATED)
        w->disabled = 1;
}
int it_begin(it_writer *w, int fd, uint32_t role, uint32_t peer)
{
    memset(w, 0, sizeof *w);
    w->fd = fd;
    w->role = role;
    w->pid = (uint32_t)getpid();
    w->peer = peer;
    struct stat info;
    w->disabled = fd < 3 || fstat(fd, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size != 0;
    if (w->disabled)
        w->fd = -1; /* Never adopt an unrelated or invalid environment descriptor. */
    if (!w->disabled)
        it_emit(w, IT_INIT, 0, 0, 0);
    return !w->disabled;
}
const char *it_status_name(enum it_status s)
{
    static const char *const names[] = {"absent",     "empty",    "truncated", "invalid",
                                        "unfinished", "complete", "saturated"};
    return names[s];
}
void it_read(const char *path, uint32_t role, it_trace *t)
{
    memset(t, 0, sizeof *t);
    t->clock_ok = 1;
    int fd = open(path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (fd < 0) {
        t->status = errno == ENOENT ? IT_ABSENT : IT_INVALID;
        return;
    }
    struct stat info;
    t->status = IT_INVALID;
    if (fstat(fd, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size > IT_BYTES * IT_LIMIT ||
        info.st_size < 0) {
        close(fd);
        return;
    }
    uint8_t bytes[IT_BYTES * IT_LIMIT];
    ssize_t n = read(fd, bytes, sizeof bytes);
    close(fd);
    if (n != info.st_size)
        return;
    t->status = n == 0 ? IT_EMPTY : IT_UNFINISHED;
    size_t full = (size_t)n / IT_BYTES;
    int poll = -1, forked = 0, started = 0;
    uint32_t child = 0, values[2] = {0}, seen = 0;
    for (size_t i = 0; i < full; i++) {
        const uint8_t *p = bytes + i * IT_BYTES;
        it_record r = {0};
        r.type = it_get(p + 12);
        r.sequence = it_get(p + 16);
        r.role = it_get(p + 20);
        r.pid = it_get(p + 24);
        r.peer = it_get(p + 28);
        r.sec = (uint64_t)it_get(p + 32) | ((uint64_t)it_get(p + 36) << 32);
        r.nsec = it_get(p + 40);
        r.error = it_get(p + 44);
        r.a = it_get(p + 48);
        r.b = it_get(p + 52);
        r.c = it_get(p + 56);
        int terminal = t->status == IT_COMPLETE || t->status == IT_FULL;
        int helper_type = r.type == IT_FORK || r.type == IT_START || r.type == IT_POST;
        int core_type = r.type == IT_POLL || r.type == IT_STATE || r.type == IT_UNAVAILABLE;
        int payload_bad = 0;
        if (r.type == IT_INIT)
            payload_bad = r.a || r.b || r.c || (role == IT_HELPER ? r.peer != 0 : r.peer == 0);
        else if (r.type == IT_FORK) {
            payload_bad = forked || r.a == 0 || r.b || r.c;
            child = r.a;
            forked = 1;
        } else if (r.type == IT_START) {
            payload_bad = !forked || started || r.a || r.b || r.c;
            started = 1;
        } else if (r.type == IT_POST)
            payload_bad =
                !started || (r.a != 126 && r.a != 6 && r.a != 100) || r.b > 1 || r.c != child;
        else if (r.type == IT_POLL) {
            payload_bad = r.a > 1 || r.b || r.c || (int)r.a == poll;
            poll = (int)r.a;
        } else if (r.type == IT_STATE) {
            unsigned slot = r.a == 4u ? 0u : 1u;
            payload_bad = poll == -1 || (r.a != 0 && r.a != 4) || r.b > 65535u || r.c != 0 ||
                          ((seen & (1u << slot)) && values[slot] == r.b);
            values[slot] = r.b;
            seen |= 1u << slot;
        } else if (r.type == IT_UNAVAILABLE) {
            payload_bad = poll == -1 || r.a < 1 || r.a > 3 || r.c || (r.a < 3 && r.b);
            seen = 0;
        } else if (r.type == IT_SATURATED)
            payload_bad = r.a || r.b || r.c;
        if (memcmp(p, "MHTRACE1", 8) != 0 || it_get(p + 8) != 1 || it_get(p + 60) != 0 ||
            r.sequence != i || r.role != role || r.pid == 0 || r.type < IT_INIT ||
            r.type > IT_SATURATED || terminal || payload_bad || (i == 0 && r.type != IT_INIT) ||
            (i != 0 &&
             (r.type == IT_INIT || r.pid != t->records[0].pid || r.peer != t->records[0].peer)) ||
            (helper_type && role != IT_HELPER) || (core_type && role != IT_CORE) ||
            r.nsec >= 1000000000u || (r.error != 0 && (r.sec != 0 || r.nsec != 0)) ||
            (r.type == IT_SATURATED && i != IT_LIMIT - 1u)) {
            t->status = IT_INVALID;
            return;
        }
        if (r.error)
            t->clock_ok = 0;
        if (i != 0 && r.error == 0 && t->records[i - 1u].error == 0 &&
            (r.sec < t->records[i - 1u].sec ||
             (r.sec == t->records[i - 1u].sec && r.nsec < t->records[i - 1u].nsec))) {
            t->status = IT_INVALID;
            return;
        }
        t->records[t->count++] = r;
        if (r.type == IT_END)
            t->status = IT_COMPLETE;
        if (r.type == IT_SATURATED)
            t->status = IT_FULL;
    }
    if ((size_t)n % IT_BYTES != 0)
        t->status = IT_TRUNCATED;
    else if (full == IT_LIMIT && t->status != IT_FULL)
        t->status = IT_INVALID;
}
int it_pair(const it_trace *h, const it_trace *c)
{
    if ((h->status != IT_COMPLETE && h->status != IT_FULL) || c->status < IT_UNFINISHED ||
        !h->clock_ok || !c->clock_ok || c->records[0].peer != h->records[0].pid)
        return 0;
    for (uint32_t i = 0; i < h->count; i++)
        if (h->records[i].type == IT_FORK && h->records[i].a == c->records[0].pid)
            return 1;
    return 0;
}
