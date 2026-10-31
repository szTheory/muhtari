/* Actual helper with deterministic permission/events/own-child/time, never real input. */
#include <time.h>
int keyboard_clock(int, struct timespec *);
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
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
#include <dirent.h>
#include <errno.h>
#include "../retroarch/keyboard_preflight.c"
#undef main
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
struct keyboard_directory {
    int unused;
};
static struct keyboard_directory directory;
static struct dirent entry;

DIR *keyboard_open(const char *path)
{
    bad |= strcmp(path, "/fixture/raw") != 0;
    if (shot_fault == 5)
        return NULL;
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
    if (shot_fault == 6) {
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
    return shot_fault == 7 ? -1 : 0;
}
int keyboard_clock(int id, struct timespec *value)
{
    bad |= id != CLOCK_MONOTONIC;
    value->tv_sec = ticks * step_ms / 1000;
    value->tv_nsec = (ticks * step_ms % 1000) * 1000000L;
    return ++clock_calls == fail_clock ? -1 : 0;
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
    return flags == 0 || ticks >= finish ? 100 : fail_wait ? -1 : 0;
}
int keyboard_kill(int pid, int sig)
{
    bad |= pid != 100 || sig != SIGKILL;
    kills++;
    return 0;
}
int keyboard_sleep(const struct timespec *delay, struct timespec *remaining)
{
    bad |= delay->tv_sec != 0 || delay->tv_nsec != 100000000 || remaining != NULL;
    ticks++;
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
}
#define CHECK(test)                                                                                \
    do {                                                                                           \
        if (!(test)) {                                                                             \
            fprintf(stderr, "keyboard control failed at line %d\n", __LINE__);                     \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
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
    CHECK(preflight_main(0, NULL) == 2 && preflight_main(1, held) == 2 &&
          preflight_main(2, held) == 2 && preflight_main(3, unknown) == 2 && calls == 0);
    permitted = 0;
    CHECK(preflight_main(2, preflight) == 77 && preflight_main(3, held) == 77 && calls == 2 &&
          forks == 0 && allocations == 0);
    reset();
    CHECK(preflight_main(2, preflight) == 0 && calls == 1 && forks == 0 && allocations == 0);
    reset();
    CHECK(preflight_main(3, held) == 0 && downs == 40 && ups == 0 && frees == posts && !bad);
    for (int boundary = 49; boundary <= 51; boundary++) {
        reset();
        finish = boundary;
        CHECK(preflight_main(3, released) == (boundary > 50 ? 0 : 1) &&
              downs == 2 * boundary - ups && ups == (boundary > 50 ? 2 : 0) && frees == posts &&
              !bad);
    }
    reset();
    finish = 60;
    CHECK(preflight_main(3, released) == 0 && downs == 100 && ups == 20 && !bad);
    reset();
    finish = 60;
    CHECK(preflight_main(4, probe) == 0 && downs == 100 && ups == 20 && shots_down == 21 &&
          shots_up == 1 && posts == frees && forks == 1 && !bad);
    /* Readiness can arrive anywhere before the existing held cutoff, not just 2s. */
    for (int boundary = 0; boundary <= 49; boundary++) {
        reset();
        ready = boundary;
        finish = 60;
        CHECK(preflight_main(4, probe) == (boundary >= 49) && shots_down == boundary + 1 &&
              shots_up == (boundary < 49) && kills == (boundary >= 49) && posts == frees && !bad);
    }
    for (int fault = 1; fault <= 7; fault++) {
        reset();
        finish = 60;
        shot_fault = fault;
        CHECK(preflight_main(4, probe) == (fault >= 5 ? 2 : 1) && kills == (fault < 5) &&
              forks == (fault < 5) && posts == frees && !bad);
    }
    reset();
    stale = 1;
    CHECK(preflight_main(4, probe) == 2 && forks == 0 && posts == 0);
    for (int boundary = 0; boundary <= 51; boundary++) {
        reset();
        finish = boundary;
        CHECK(preflight_main(4, probe) == (boundary <= 50) && posts == frees && !bad);
    }
    for (int boundary = 1; boundary <= 2; boundary++) {
        reset();
        fail_clock = boundary;
        CHECK(preflight_main(4, probe) == 1 && kills == (boundary == 2) && posts == 0 && !bad);
    }
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
    reset();
    fail_event = 123; /* First release event, after100 Up/Z and22 F8 posts. */
    finish = 60;
    CHECK(preflight_main(4, probe) == 1 && kills == 1 && ups == 0 && posts == frees && !bad);
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
    }
    reset();
    finish = 0;
    CHECK(preflight_main(3, held) == 1 && posts == 0 && !bad);
    return 0;
}
