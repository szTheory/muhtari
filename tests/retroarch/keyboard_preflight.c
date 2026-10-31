/* Nonprompting access gate, then bounded events to our own frontend child only. */
#define _POSIX_C_SOURCE 200809L
#include <ApplicationServices/ApplicationServices.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static int captures(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
        return -1;
    int count = 0;
    struct dirent *entry;
    errno = 0;
    while ((entry = readdir(dir)) != NULL) {
        const char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".png") == 0 && ++count > 1)
            break;
    }
    int failed = errno != 0;
    return closedir(dir) != 0 || failed ? -1 : count;
}

int main(int argc, char **argv)
{
    int preflight = argc == 2 && strcmp(argv[1], "--preflight") == 0;
    int held = argc >= 3 && strcmp(argv[1], "--held") == 0;
    int probe = argc >= 4 && strcmp(argv[1], "--probe") == 0;
    int released = argc >= 3 && (strcmp(argv[1], "--released") == 0 || probe);
    if (!preflight && !held && !released)
        return 2;
    if (probe && captures(argv[2]) != 0)
        return 2;
    int permitted = CGPreflightPostEventAccess();
    printf("post-event-access=%s\n", permitted ? "permitted" : "denied");
    if (!permitted || preflight)
        return permitted ? 0 : 77;
    fflush(stdout);
    struct timespec start, now;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0)
        return 1;
    pid_t child = fork();
    if (child < 0)
        return 1;
    if (child == 0) {
        execv(argv[2 + probe], argv + 2 + probe);
        _exit(127);
    }
    int status = 0, acknowledged = 0, holding = 1;
    /* Repeat during startup; Up/Z are HIToolbox's Up Arrow/ANSI Z keycodes. */
    const CGKeyCode keys[] = {126, 6, 100};
    for (unsigned tick = 0; tick < 450; tick++) {
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
            break;
        int64_t elapsed =
            (int64_t)(now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
        int shots = probe ? captures(argv[2]) : 0;
        if (elapsed >= 45000 || shots < 0 || shots > 1 || (probe && acknowledged && shots != 1) ||
            (probe && !acknowledged && elapsed >= 5000))
            break;
        pid_t result = waitpid(child, &status, WNOHANG);
        if (result == child) {
            printf("keyboard target=%ld elapsed_ms=%lld child-exit\n", (long)child,
                   (long long)elapsed);
            return !WIFEXITED(status) || WEXITSTATUS(status) != 0 || tick == 0 ||
                   (released && elapsed <= 5000) || (probe && (!acknowledged || shots != 1));
        }
        if (result < 0)
            break;
        int down = !released || elapsed < 5000;
        for (unsigned i = 0; i < 3; i++) {
            if (i == 2 && (!probe || acknowledged))
                continue;
            CGEventRef event =
                CGEventCreateKeyboardEvent(NULL, keys[i], i == 2 ? shots == 0 : down != 0);
            if (event == NULL)
                goto stop;
            CGEventSetFlags(event, 0);
            CGEventPostToPid(child, event);
            CFRelease(event);
            if (i == 2 || tick == 0 || (released && holding && !down))
                printf("keyboard target=%ld elapsed_ms=%lld key=%u down=%d capture=%d\n",
                       (long)child, (long long)elapsed, (unsigned)keys[i],
                       i == 2 ? shots == 0 : down, shots);
        }
        acknowledged |= shots == 1;
        holding = down;
        fflush(stdout);
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
