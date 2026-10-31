/* Actual helper with deterministic permission/events/own-child/time, never real input. */
#include <time.h>
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
typedef int keyboard_clockid_t;
#else
typedef clockid_t keyboard_clockid_t;
#endif
int keyboard_clock(keyboard_clockid_t, struct timespec *);
#include <stdio.h>
#include <stdarg.h>
int keyboard_fprintf(FILE *, const char *, ...);
int preflight_main(int argc, char **argv);
#define main preflight_main
#define fork keyboard_fork
#define execv keyboard_exec
#define _exit keyboard_exit
#define waitpid keyboard_wait
#define kill keyboard_kill
#define nanosleep keyboard_sleep
#define clock_gettime keyboard_clock
#define opendir keyboard_open
#define readdir keyboard_read
#define closedir keyboard_close
#define fprintf keyboard_fprintf
#include <dirent.h>
#include <errno.h>
#include "../retroarch/keyboard_preflight.c"
#undef main
#undef fprintf
#include <stdlib.h>

struct keyboard_event {
    CGKeyCode key;
    int down;
};
static struct keyboard_event event;
static int permitted, calls, forks, ticks, finish, child_status, fail_fork, fail_wait, fail_event;
static int fail_sleep, allocations, posts, frees, downs, ups, kills, bad, shots_down, shots_up;
static int child_mode, execs, waits;
static int ready, capture_tick, shot_fault, stale, dir_index, dir_shots, clock_calls, fail_clock;
static int step_ms;
static char diagnostic[256];
static int reports;
struct keyboard_directory {
    int unused;
};
static struct keyboard_directory directory;
static struct dirent entry;

int keyboard_fprintf(FILE *stream, const char *format, ...)
{
    bad |= stream != stderr;
    va_list args;
    va_start(args, format);
    int size = vsnprintf(diagnostic, sizeof diagnostic, format, args);
    va_end(args);
    reports++;
    return size;
}

DIR *keyboard_open(const char *path)
{
    bad |= strcmp(path, "/fixture/raw") != 0;
    if (shot_fault == 5 || (shot_fault == 8 && ticks >= 40)) {
        errno = EACCES;
        return NULL;
    }
    dir_index = 0;
    dir_shots = stale || (capture_tick >= 0 && ticks >= capture_tick);
    if (shot_fault == 2 && dir_shots)
        dir_shots = 2;
    if (shot_fault == 3 && ticks >= 40)
        dir_shots = 2;
    if (shot_fault == 4 && ticks >= 40)
        dir_shots = 0;
    return &directory;
}
struct dirent *keyboard_read(DIR *dir)
{
    bad |= dir != &directory;
    if (shot_fault == 6 || (shot_fault == 9 && ticks >= 40)) {
        errno = EIO;
        return NULL;
    }
    if (dir_index++ > dir_shots)
        return NULL;
    snprintf(entry.d_name, sizeof entry.d_name, "%s", dir_index == 1 ? "note.txt" : "capture.png");
    return &entry;
}
int keyboard_close(DIR *dir)
{
    bad |= dir != &directory;
    errno = EBADF; /* A successful close may also change errno after a read failure. */
    return shot_fault == 7 || (shot_fault == 10 && ticks >= 40) ? -1 : 0;
}
int keyboard_clock(keyboard_clockid_t id, struct timespec *value)
{
    bad |= id != CLOCK_MONOTONIC;
    value->tv_sec = ticks * step_ms / 1000;
    value->tv_nsec = (ticks * step_ms % 1000) * 1000000L;
    if (++clock_calls == fail_clock) {
        errno = EIO;
        return -1;
    }
    return 0;
}

int CGPreflightPostEventAccess(void)
{
    calls++;
    return permitted;
}
CGEventRef CGEventCreateKeyboardEvent(void *source, CGKeyCode key, int down)
{
    bad |= source != NULL || (key != 126 && key != 6 && key != 100);
    allocations++;
    if (allocations == fail_event)
        return NULL;
    event.key = key;
    event.down = down;
    return &event;
}
void CGEventSetFlags(CGEventRef e, uint64_t flags)
{
    bad |= e != &event || flags != 0;
}
void CGEventPostToPid(int pid, CGEventRef e)
{
    bad |= pid != 100 || e != &event;
    posts++;
    if (e->key == 100) {
        shots_down += e->down;
        shots_up += !e->down;
        bad |= e->down ? shots_up != 0 || ticks * step_ms >= 5000
                       : capture_tick < 0 || ticks < capture_tick || shots_up != 1;
        if (e->down && ticks >= ready && shot_fault != 1 && capture_tick < 0)
            capture_tick = ticks + 1;
    } else {
        downs += e->down;
        ups += !e->down;
    }
}
void CFRelease(CGEventRef e)
{
    bad |= e != &event;
    frees++;
}
pid_t keyboard_fork(void)
{
    forks++;
    if (fail_fork)
        errno = EAGAIN;
    return fail_fork ? -1 : child_mode ? 0 : 100;
}
int keyboard_exec(const char *path, char *const *args)
{
    (void)path;
    (void)args;
    execs++;
    if (child_mode)
        bad |= strcmp(path, "/fixture/retroarch") != 0 ||
               strcmp(args[0], "/fixture/retroarch") != 0 ||
               strcmp(args[1], "--max-frames=600") != 0 || strcmp(args[2], "--config") != 0 ||
               strcmp(args[3], "isolated.cfg") != 0 || args[4] != NULL;
    /* Failed exec returns; the helper's following _exit must remain reachable. */
    return -1;
}
void keyboard_exit(int code)
{
    if (child_mode) {
        if (code != 127 || execs != 1 || posts != 0 || waits != 0 || kills != 0 || bad)
            exit(1);
        puts("child exec failure: argv preserved, no input or wait");
        exit(127);
    }
    abort();
}
pid_t keyboard_wait(pid_t pid, int *status, int flags)
{
    bad |= pid != 100;
    waits++;
    *status = child_status;
    if (fail_wait)
        errno = ECHILD;
    return flags == 0 || ticks >= finish ? 100 : fail_wait ? -1 : 0;
}
int keyboard_kill(int pid, int sig)
{
    bad |= pid != 100 || sig != SIGKILL;
    kills++;
    errno = EBADF; /* Cleanup must not replace the stopping operation's error. */
    return 0;
}
int keyboard_sleep(const struct timespec *delay, struct timespec *remaining)
{
    bad |= delay->tv_sec != 0 || delay->tv_nsec != 100000000 || remaining != NULL;
    ticks++;
    if (fail_sleep)
        errno = EINTR;
    return fail_sleep ? -1 : 0;
}
static void reset(void)
{
    permitted = 1;
    calls = forks = ticks = child_status = fail_fork = fail_wait = fail_event = fail_sleep = 0;
    allocations = posts = frees = downs = ups = kills = bad = 0;
    shots_down = shots_up = 0;
    child_mode = execs = waits = 0;
    finish = 20;
    ready = 20;
    capture_tick = -1;
    shot_fault = stale = dir_index = dir_shots = clock_calls = fail_clock = 0;
    step_ms = 100;
    diagnostic[0] = '\0';
    reports = 0;
}
#define CHECK(test)                                                                                \
    do {                                                                                           \
        if (!(test)) {                                                                             \
            fprintf(stderr, "keyboard control failed at line %d\n", __LINE__);                     \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int reported(const char *reason, int64_t elapsed, int shots, int error, const char *status)
{
    char expected[256];
    snprintf(expected, sizeof expected,
             "keyboard: reason=%s elapsed_ms=%lld capture=%d errno=%d child_status=%s\n", reason,
             (long long)elapsed, shots, error, status);
    if (strcmp(diagnostic, expected) != 0) {
        fprintf(stderr, "expected %sobserved %s", expected, diagnostic);
        return 0;
    }
    return reports == 1;
}
int main(int argc, char **argv)
{
    char *preflight[] = {"keyboard", "--preflight", NULL};
    char *unknown[] = {"keyboard", "--request", NULL};
    char *held[] = {"keyboard", "--held", "/fixture/retroarch", NULL};
    char *released[] = {"keyboard", "--released", "/fixture/retroarch", NULL};
    char *probe[] = {"keyboard", "--probe", "/fixture/raw", "/fixture/retroarch", NULL};
    CHECK(keyboard_exec(held[2], held + 2) == -1);
    reset();
    if (argc != 1) {
        if (argc != 2 || strcmp(argv[1], "--child-exec-failure") != 0)
            return 2;
        char *child[] = {"keyboard",         "--probe",  "/fixture/raw", "/fixture/retroarch",
                         "--max-frames=600", "--config", "isolated.cfg", NULL};
        child_mode = 1;
        return preflight_main(7, child);
    }
    CHECK(preflight_main(0, NULL) == 2 && reported("arguments", -1, -1, 0, "unknown"));
    reset();
    CHECK(preflight_main(0, NULL) == 2 && preflight_main(1, held) == 2 &&
          preflight_main(2, held) == 2 && preflight_main(3, unknown) == 2 && calls == 0);
    permitted = 0;
    CHECK(preflight_main(2, preflight) == 77 && preflight_main(3, held) == 77 && calls == 2 &&
          forks == 0 && allocations == 0);
    reset();
    CHECK(preflight_main(2, preflight) == 0 && calls == 1 && forks == 0 && allocations == 0 &&
          reports == 0);
    reset();
    CHECK(preflight_main(3, held) == 0 && downs == 40 && ups == 0 && frees == posts && !bad);
    CHECK(reported("child-completion", 2000, -1, 0, "0"));
    for (int boundary = 49; boundary <= 51; boundary++) {
        reset();
        finish = boundary;
        CHECK(preflight_main(3, released) == (boundary > 50 ? 0 : 1) &&
              downs == 2 * boundary - ups && ups == (boundary > 50 ? 2 : 0) && frees == posts &&
              !bad);
        CHECK(reported("child-completion", boundary * 100, -1, 0, "0"));
    }
    reset();
    finish = 60;
    CHECK(preflight_main(3, released) == 0 && downs == 100 && ups == 20 && !bad);
    reset();
    finish = 60;
    CHECK(preflight_main(4, probe) == 0 && downs == 100 && ups == 20 && shots_down == 21 &&
          shots_up == 1 && posts == frees && forks == 1 && !bad);
    CHECK(reported("child-completion", 6000, 1, 0, "0"));
    /* Readiness can arrive anywhere before the existing held cutoff, not just 2s. */
    for (int boundary = 0; boundary <= 49; boundary++) {
        reset();
        ready = boundary;
        finish = 60;
        CHECK(preflight_main(4, probe) == (boundary >= 49) && shots_down == boundary + 1 &&
              shots_up == (boundary < 49) && kills == (boundary >= 49) && posts == frees && !bad);
        CHECK(boundary < 49 ? reported("child-completion", 6000, 1, 0, "0")
                            : reported("acknowledgment-deadline", 5000, 1, 0, "unknown"));
    }
    const int directory_errors[] = {EACCES, EIO, EBADF};
    for (int fault = 1; fault <= 7; fault++) {
        reset();
        finish = 60;
        shot_fault = fault;
        CHECK(preflight_main(4, probe) == (fault >= 5 ? 2 : 1) && kills == (fault < 5) &&
              forks == (fault < 5) && posts == frees && !bad);
        if (fault == 1)
            CHECK(reported("acknowledgment-deadline", 5000, 0, 0, "unknown"));
        else if (fault <= 3)
            CHECK(reported("capture-count", fault == 2 ? 2100 : 4000, 2, 0, "unknown"));
        else if (fault == 4)
            CHECK(reported("capture-lost", 4000, 0, 0, "unknown"));
        else
            CHECK(reported("capture-directory", -1, -1, directory_errors[fault - 5], "unknown"));
    }
    for (int fault = 8; fault <= 10; fault++) {
        reset();
        finish = 60;
        shot_fault = fault;
        CHECK(preflight_main(4, probe) == 1 && kills == 1 && posts == frees && !bad);
        CHECK(reported("capture-directory", 4000, 1, directory_errors[fault - 8], "unknown"));
    }
    reset();
    stale = 1;
    CHECK(preflight_main(4, probe) == 2 && forks == 0 && posts == 0);
    CHECK(reported("initial-capture-count", -1, 1, 0, "unknown"));
    for (int boundary = 0; boundary <= 51; boundary++) {
        reset();
        finish = boundary;
        CHECK(preflight_main(4, probe) == (boundary <= 50) && posts == frees && !bad);
    }
    for (int boundary = 1; boundary <= 2; boundary++) {
        reset();
        fail_clock = boundary;
        CHECK(preflight_main(4, probe) == 1 && kills == (boundary == 2) && posts == 0 && !bad);
        CHECK(reported("clock", -1, 0, EIO, "unknown"));
    }
    reset();
    fail_clock = 12;
    CHECK(preflight_main(4, probe) == 1 && kills == 1 && ticks == 10 && !bad);
    CHECK(reported("clock", 900, 0, EIO, "unknown"));
    reset();
    step_ms = 200;
    ready = 10;
    finish = 30;
    CHECK(preflight_main(4, probe) == 0 && downs == 50 && ups == 10 && shots_down == 11 &&
          shots_up == 1 && posts == frees && !bad);
    reset();
    step_ms = 0;
    finish = 500;
    CHECK(preflight_main(4, probe) == 1 && ticks == 450 && kills == 1 && !bad);
    CHECK(reported("iteration-limit", 0, 1, 0, "unknown"));
    reset();
    fail_event = 123; /* First release event, after100 Up/Z and22 F8 posts. */
    finish = 60;
    CHECK(preflight_main(4, probe) == 1 && kills == 1 && ups == 0 && posts == frees && !bad);
    CHECK(reported("event", 5000, 1, 0, "unknown"));
    for (int fault = 0; fault < 7; fault++) {
        reset();
        fail_fork = fault == 0;
        fail_wait = fault == 1;
        fail_event = fault == 2 ? 2 : 0;
        fail_sleep = fault == 3;
        child_status = fault == 4 ? 9 : fault == 5 ? 256 : 0;
        finish = fault == 6 ? 500 : 20;
        CHECK(preflight_main(3, held) == 1 && !bad &&
              kills == (fault <= 3 ? fault != 0 : fault == 6));
        CHECK(frees == posts && ticks <= 450);
        const char *reasons[] = {
            "fork",           "wait", "event", "sleep", "child-completion", "child-completion",
            "iteration-limit"};
        const int64_t elapsed[] = {-1, 0, 0, 0, 2000, 2000, 44900};
        const int errors[] = {EAGAIN, ECHILD, 0, EINTR, 0, 0, 0};
        const char *statuses[] = {"unknown", "unknown", "unknown", "unknown",
                                  "9",       "256",     "unknown"};
        CHECK(reported(reasons[fault], elapsed[fault], -1, errors[fault], statuses[fault]));
    }
    reset();
    finish = 0;
    CHECK(preflight_main(3, held) == 1 && posts == 0 && !bad);
    CHECK(reported("child-completion", 0, -1, 0, "0"));
    reset();
    step_ms = 1000;
    shot_fault = 1;
    /* The acknowledgment guard still precedes a simultaneously completed child. */
    finish = 5;
    CHECK(preflight_main(4, probe) == 1 && waits == 6 && kills == 1 && !bad);
    CHECK(reported("acknowledgment-deadline", 5000, 0, 0, "unknown"));
    reset();
    step_ms = 1125;
    ready = 0;
    finish = 40;
    shot_fault = 8;
    /* The total deadline still wins over a simultaneous directory failure/child exit. */
    CHECK(preflight_main(4, probe) == 1 && waits == 41 && kills == 1 && !bad);
    CHECK(reported("deadline", 45000, 1, 0, "unknown"));
    return 0;
}
