/* Nonprompting access gate, then bounded events to our own frontend child only. */
#define _POSIX_C_SOURCE 200809L
#include <ApplicationServices/ApplicationServices.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int preflight = argc == 2 && strcmp(argv[1], "--preflight") == 0;
    int held = argc >= 3 && strcmp(argv[1], "--held") == 0;
    int probe = argc >= 3 && strcmp(argv[1], "--probe") == 0;
    int released = argc >= 3 && (strcmp(argv[1], "--released") == 0 || probe);
    if (!preflight && !held && !released)
        return 2;
    int permitted = CGPreflightPostEventAccess();
    printf("post-event-access=%s\n", permitted ? "permitted" : "denied");
    if (!permitted || preflight)
        return permitted ? 0 : 77;
    fflush(stdout);
    pid_t child = fork();
    if (child < 0)
        return 1;
    if (child == 0) {
        execv(argv[2], argv + 2);
        _exit(127);
    }
    int status = 0;
    /* Repeat during startup; Up/Z are HIToolbox's Up Arrow/ANSI Z keycodes. */
    const CGKeyCode keys[] = {126, 6, 100};
    for (unsigned tick = 0; tick < 450; tick++) {
        pid_t result = waitpid(child, &status, WNOHANG);
        if (result == child)
            return !WIFEXITED(status) || WEXITSTATUS(status) != 0 || tick == 0 ||
                   (released && tick <= 50);
        if (result < 0)
            break;
        int down = !released || tick < 50;
        if (tick == 0 || (released && tick == 50)) {
            printf("keyboard target=%ld tick=%u Up/Z=%s\n", (long)child, tick,
                   down ? "down" : "up");
            fflush(stdout);
        }
        for (unsigned i = 0; i < 3; i++) {
            if (i == 2 && (!probe || (tick != 20 && tick != 21)))
                continue;
            CGEventRef event =
                CGEventCreateKeyboardEvent(NULL, keys[i], i == 2 ? tick == 20 : down != 0);
            if (event == NULL)
                goto stop;
            CGEventSetFlags(event, 0);
            CGEventPostToPid(child, event);
            CFRelease(event);
        }
        const struct timespec delay = {0, 100000000};
        if (nanosleep(&delay, NULL) != 0)
            break;
    }
stop:
    kill(child, SIGKILL);
    waitpid(child, &status, 0);
    fprintf(stderr, "keyboard: child, event or 45s deadline failure\n");
    return 1;
}
