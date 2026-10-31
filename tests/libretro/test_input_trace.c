#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "input_trace.h"
static int fault, writes;
static ssize_t trace_write(int fd, const void *p, size_t n)
{
    writes++;
    if (fault == 1) {
        errno = EIO;
        return -1;
    }
    return write(fd, p, fault == 2 ? n - 1u : n);
}
static int trace_clock(clockid_t id, struct timespec *now)
{
    if (fault == 3) {
        errno = EIO;
        return -1;
    }
    return clock_gettime(id, now);
}
#define write trace_write
#define clock_gettime trace_clock
#include "input_trace.c"
#undef write
#undef clock_gettime
#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "trace check line%d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int create(const char *path)
{
    return open(path, O_CREAT | O_EXCL | O_RDWR, 0600);
}
static int collect(const char *dir, int dump)
{
    char path[4096];
    it_trace h, c;
    snprintf(path, sizeof path, "%s/helper.trace", dir);
    it_read(path, IT_HELPER, &h);
    snprintf(path, sizeof path, "%s/core.trace", dir);
    it_read(path, IT_CORE, &c);
    printf("helper=%s records=%u; core=%s records=%u", it_status_name(h.status), h.count,
           it_status_name(c.status), c.count);
    if (c.count) {
        const it_record *last = &c.records[c.count - 1u];
        printf(" last_type=%u sequence=%u pid=%u time=%llu.%09u clock_errno=%u", last->type,
               last->sequence, last->pid, (unsigned long long)last->sec, last->nsec, last->error);
    }
    int paired = it_pair(&h, &c);
    printf(" paired_identity_clock_ok=%s; unfinished_writer_health=unknown; "
           "same_host_clock_domain=caller-required; external_writer_errors=caller-required\n",
           paired ? "yes" : "no");
    if (dump) {
        static const char *const types[] = {"",     "INIT",  "FORK",        "START", "POST",
                                            "POLL", "STATE", "UNAVAILABLE", "END",   "SATURATED"};
        const it_trace *streams[] = {&h, &c};
        for (unsigned stream = 0; stream < 2; stream++) {
            const it_trace *t = streams[stream];
            printf("stream=%s status=%s valid_prefix_records=%u clock_ok=%s\n",
                   stream ? "core" : "helper", it_status_name(t->status), t->count,
                   t->clock_ok ? "yes" : "no");
            for (uint32_t i = 0; i < t->count; i++) {
                const it_record *r = &t->records[i];
                printf("stream=%s type=%s sequence=%u pid=%u peer=%u time=%llu.%09u "
                       "clock_errno=%u a=%u b=%u c=%u\n",
                       stream ? "core" : "helper", types[r->type], r->sequence, r->pid, r->peer,
                       (unsigned long long)r->sec, r->nsec, r->error, r->a, r->b, r->c);
            }
        }
        puts("limits: saturated=prefix-only; unfinished=writer-health-unknown; "
             "invalid-or-truncated=not-successful-evidence; missing-records=inconclusive; "
             "pairing-does-not-establish-same-host-clock-domain-or-input-delivery");
    }
    return !paired;
}
static int dump_fixtures(const char *dir)
{
    static const char *const cases[] = {"positive",      "killed",        "saturated", "malformed",
                                        "write-failure", "clock-failure", "invalid"};
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        char path[4096], folder[4000];
        snprintf(folder, sizeof folder, "%s/dump-%s", dir, cases[i]);
        REQUIRE(mkdir(folder, 0700) == 0 || errno == EEXIST);
        for (unsigned role = IT_HELPER; role <= IT_CORE; role++) {
            snprintf(path, sizeof path, "%s/%s.trace", folder, role == IT_CORE ? "core" : "helper");
            unlink(path);
            int fd = create(path);
            REQUIRE(fd >= 3);
            it_writer w;
            REQUIRE(it_begin(&w, fd, role, role == IT_CORE ? (uint32_t)getpid() : 0));
            if (role == IT_HELPER) {
                it_emit(&w, IT_FORK, (uint32_t)getpid(), 0, 0);
                it_emit(&w, IT_START, 0, 0, 0);
                if (i == 2)
                    for (unsigned n = 0; n < IT_LIMIT; n++)
                        it_emit(&w, IT_POST, 126, 1, (uint32_t)getpid());
            } else {
                fault = i == 4 ? 1 : i == 5 ? 3 : 0;
                it_emit(&w, IT_POLL, 1, 0, 0);
                it_emit(&w, IT_STATE, 4, 1, 0);
                it_emit(&w, IT_STATE, 0, 1, 0);
                fault = 0;
            }
            if (role == IT_HELPER || i != 1)
                it_emit(&w, IT_END, 0, 0, 0);
            if (role == IT_CORE && i == 3)
                REQUIRE(write(fd, "bad", 3) == 3);
            if (role == IT_CORE && i == 6) {
                uint8_t invalid[IT_BYTES] = {0};
                REQUIRE(write(fd, invalid, sizeof invalid) == (ssize_t)sizeof invalid);
            }
            close(fd);
        }
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "--collect") == 0)
        return collect(argv[2], 0);
    if (argc == 3 && strcmp(argv[1], "--dump") == 0)
        return collect(argv[2], 1);
    REQUIRE(argc == 2);
    char path[4096];
    snprintf(path, sizeof path, "%s/trace-XXXXXX", argv[1]);
    int fd = mkstemp(path);
    REQUIRE(fd >= 3);
    it_trace t;
    it_writer w;
    it_read("/missing-input-trace", IT_CORE, &t);
    REQUIRE(t.status == IT_ABSENT);
    it_read(path, IT_CORE, &t);
    REQUIRE(t.status == IT_EMPTY);
    REQUIRE(it_begin(&w, fd, IT_CORE, (uint32_t)getppid()));
    it_emit(&w, IT_POLL, 1, 0, 0);
    it_emit(&w, IT_STATE, 4, 1, 0);
    it_emit(&w, IT_STATE, 0, 1, 0);
    it_emit(&w, IT_STATE, 4, 0, 0);
    it_emit(&w, IT_END, 0, 0, 0);
    it_read(path, IT_CORE, &t);
    REQUIRE(t.status == IT_COMPLETE && t.count == 6 && t.clock_ok);
    uint8_t good[IT_BYTES * 6];
    REQUIRE(pread(fd, good, sizeof good, 0) == (ssize_t)sizeof good);
    /* Exact parser consumers reject corruption, removed poll/initial state and tail. */
    const unsigned corrupt[] = {0,
                                8,
                                12,
                                16,
                                20,
                                24,
                                28,
                                40,
                                44,
                                48,
                                60,
                                IT_BYTES + 48,
                                2 * IT_BYTES + 48,
                                2 * IT_BYTES + 56};
    for (unsigned i = 0; i < sizeof corrupt / sizeof corrupt[0]; i++) {
        uint8_t bad[sizeof good];
        memcpy(bad, good, sizeof bad);
        bad[corrupt[i]] ^= 0x80;
        if (corrupt[i] == 40)
            bad[43] = 0xff; /* outside the nanosecond range, not a valid nearby instant */
        REQUIRE(pwrite(fd, bad, sizeof bad, 0) == (ssize_t)sizeof bad);
        it_read(path, IT_CORE, &t);
        REQUIRE(t.status == IT_INVALID);
    }
    REQUIRE(pwrite(fd, good, sizeof good, 0) == (ssize_t)sizeof good);
    REQUIRE(ftruncate(fd, sizeof good - 1u) == 0);
    it_read(path, IT_CORE, &t);
    REQUIRE(t.status == IT_TRUNCATED && t.count == 5);
    REQUIRE(ftruncate(fd, IT_BYTES * 5) == 0);
    it_read(path, IT_CORE, &t);
    REQUIRE(t.status == IT_UNFINISHED && t.count == 5);
    /* Slot neighbors:62 ordinary records,63, then explicit64th saturation. */
    REQUIRE(ftruncate(fd, 0) == 0 && lseek(fd, 0, SEEK_SET) == 0);
    REQUIRE(it_begin(&w, fd, IT_HELPER, 0));
    it_emit(&w, IT_FORK, 42, 0, 0);
    it_emit(&w, IT_START, 0, 0, 0);
    for (unsigned i = 3; i < 62; i++)
        it_emit(&w, IT_POST, 126, 1, 42);
    it_read(path, IT_HELPER, &t);
    REQUIRE(t.status == IT_UNFINISHED && t.count == 62);
    it_emit(&w, IT_POST, 6, 1, 42);
    it_read(path, IT_HELPER, &t);
    REQUIRE(t.status == IT_UNFINISHED && t.count == 63);
    it_emit(&w, IT_POST, 100, 1, 42);
    int before = writes;
    it_emit(&w, IT_END, 0, 0, 0);
    REQUIRE(writes == before);
    it_read(path, IT_HELPER, &t);
    REQUIRE(t.status == IT_FULL && t.count == IT_LIMIT);
    uint8_t end_type[4];
    it_put(end_type, IT_END);
    REQUIRE(pwrite(fd, end_type, 4, (IT_LIMIT - 1u) * IT_BYTES + 12u) == 4);
    it_read(path, IT_HELPER, &t);
    REQUIRE(t.status == IT_INVALID); /* omitted saturation cannot look like a clean close */
    REQUIRE(ftruncate(fd, IT_BYTES * (IT_LIMIT - 1u)) == 0);
    it_read(path, IT_HELPER, &t);
    REQUIRE(t.status == IT_UNFINISHED);
    for (fault = 1; fault <= 3; fault++) {
        REQUIRE(ftruncate(fd, 0) == 0 && lseek(fd, 0, SEEK_SET) == 0);
        int ok = it_begin(&w, fd, IT_CORE, (uint32_t)getppid());
        before = writes;
        it_emit(&w, IT_END, 0, 0, 0);
        if (fault < 3)
            REQUIRE(!ok && writes == before);
        it_read(path, IT_CORE, &t);
        REQUIRE(t.status == (fault == 1 ? IT_EMPTY : fault == 2 ? IT_TRUNCATED : IT_COMPLETE));
        if (fault == 3)
            REQUIRE(!t.clock_ok && t.records[0].error == EIO && t.records[0].sec == 0);
    }
    fault = 0;
    REQUIRE(!it_begin(&w, fd, IT_CORE, 1)); /* nonempty descriptor refused, never adopted */
    REQUIRE(w.fd == -1 && fcntl(fd, F_GETFD) >= 0);
    REQUIRE(!it_begin(&w, 1, IT_CORE, 1) && w.fd == -1);
    close(fd);
    unlink(path);
    REQUIRE(dump_fixtures(argv[1]) == 0);
    /* A killed synthetic child retains its last complete record without END. */
    fd = create(path);
    REQUIRE(fd >= 3);
    int sync[2];
    REQUIRE(pipe(sync) == 0);
    pid_t child = fork();
    REQUIRE(child >= 0);
    if (child == 0) {
        close(sync[0]);
        (void)it_begin(&w, fd, IT_CORE, (uint32_t)getppid());
        it_emit(&w, IT_POLL, 1, 0, 0);
        it_emit(&w, IT_STATE, 4, 1, 0);
        (void)write(sync[1], "!", 1);
        for (;;)
            pause();
    }
    close(sync[1]);
    char ack;
    REQUIRE(read(sync[0], &ack, 1) == 1);
    REQUIRE(kill(child, SIGKILL) == 0);
    int status;
    REQUIRE(waitpid(child, &status, 0) == child && WIFSIGNALED(status));
    it_read(path, IT_CORE, &t);
    REQUIRE(t.status == IT_UNFINISHED && t.count == 3 && t.records[2].b == 1);
    close(sync[0]);
    close(fd);
    unlink(path);
    puts("bounded encoding/parser/write/clock/saturation/kill controls passed");
    return 0;
}
