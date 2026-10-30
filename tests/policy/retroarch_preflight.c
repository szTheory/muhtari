/* Actual helper with deterministic permission/events/own-child/time, never real input. */
int preflight_main(int argc, char **argv);
#define main preflight_main
#define fork keyboard_fork
#define execv keyboard_exec
#define _exit keyboard_exit
#define waitpid keyboard_wait
#define kill keyboard_kill
#define nanosleep keyboard_sleep
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
        bad |= ticks != (e->down ? 20 : 21);
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
    return fail_fork ? -1 : 100;
}
int keyboard_exec(const char *path, char *const *args)
{
    (void)path;
    (void)args;
    /* Failed exec returns; the helper's following _exit must remain reachable. */
    return -1;
}
void keyboard_exit(int code)
{
    (void)code;
    abort();
}
pid_t keyboard_wait(pid_t pid, int *status, int flags)
{
    bad |= pid != 100;
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
    finish = 20;
}
#define CHECK(test)                                                                                \
    do {                                                                                           \
        if (!(test)) {                                                                             \
            fprintf(stderr, "keyboard control failed at line %d\n", __LINE__);                     \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void)
{
    char *preflight[] = {"keyboard", "--preflight", NULL};
    char *unknown[] = {"keyboard", "--request", NULL};
    char *held[] = {"keyboard", "--held", "/fixture/retroarch", NULL};
    char *released[] = {"keyboard", "--released", "/fixture/retroarch", NULL};
    char *probe[] = {"keyboard", "--probe", "/fixture/retroarch", NULL};
    CHECK(keyboard_exec(held[2], held + 2) == -1);
    reset();
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
    CHECK(preflight_main(3, probe) == 0 && downs == 100 && ups == 20 && shots_down == 1 &&
          shots_up == 1 && posts == frees && forks == 1 && !bad);
    for (int boundary = 20; boundary <= 21; boundary++) {
        reset();
        finish = boundary;
        CHECK(preflight_main(3, probe) == 1 && shots_down == (boundary == 21) && shots_up == 0 &&
              !bad);
    }
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
