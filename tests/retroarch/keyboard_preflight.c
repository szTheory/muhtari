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
    int saved_error = errno;
    if (closedir(dir) != 0 && saved_error == 0)
        saved_error = errno;
    errno = saved_error;
    return saved_error ? -1 : count;
}

static void report(const char *reason, int64_t elapsed, int shots, int error, int observed,
                   int status)
{
    char child_status[32] = "unknown";
    if (observed)
        snprintf(child_status, sizeof child_status, "%d", status);
    fprintf(stderr, "keyboard: reason=%s elapsed_ms=%lld capture=%d errno=%d child_status=%s\n",
            reason, (long long)elapsed, shots, error, child_status);
}

int main(int argc, char **argv)
{
    int preflight = argc == 2 && strcmp(argv[1], "--preflight") == 0;
    int held = argc >= 3 && strcmp(argv[1], "--held") == 0;
    int probe = argc >= 4 && strcmp(argv[1], "--probe") == 0;
    int released = argc >= 3 && (strcmp(argv[1], "--released") == 0 || probe);
    if (!preflight && !held && !released) {
        report("arguments", -1, -1, 0, 0, 0);
        return 2;
    }
    int shots = -1;
    if (probe) {
        shots = captures(argv[2]);
        if (shots != 0) {
            report(shots < 0 ? "capture-directory" : "initial-capture-count", -1, shots,
                   shots < 0 ? errno : 0, 0, 0);
            return 2;
        }
    }
    int permitted = CGPreflightPostEventAccess();
    printf("post-event-access=%s\n", permitted ? "permitted" : "denied");
    if (!permitted || preflight)
        return permitted ? 0 : 77;
    fflush(stdout);
    struct timespec start, now;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        report("clock", -1, shots, errno, 0, 0);
        return 1;
    }
    pid_t child = fork();
    if (child < 0) {
        report("fork", -1, shots, errno, 0, 0);
        return 1;
    }
    if (child == 0) {
        execv(argv[2 + probe], argv + 2 + probe);
        _exit(127);
    }
    int status = 0, acknowledged = 0, holding = 1;
    int64_t elapsed = -1;
    const char *reason = "iteration-limit";
    int error = 0;
    /* Repeat during startup; Up/Z are HIToolbox's Up Arrow/ANSI Z keycodes. */
    const CGKeyCode keys[] = {126, 6, 100};
    for (unsigned tick = 0; tick < 450; tick++) {
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
            reason = "clock";
            error = errno;
            break;
        }
        elapsed =
            (int64_t)(now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
        int count = probe ? captures(argv[2]) : 0;
        int capture_error = count < 0 ? errno : 0;
        if (count >= 0)
            shots = count;
        if (elapsed >= 45000) {
            reason = "deadline";
            break;
        }
        if (count < 0) {
            reason = "capture-directory";
            error = capture_error;
            break;
        }
        if (shots > 1) {
            reason = "capture-count";
            break;
        }
        if (probe && acknowledged && shots != 1) {
            reason = "capture-lost";
            break;
        }
        if (probe && !acknowledged && elapsed >= 5000) {
            reason = "acknowledgment-deadline";
            break;
        }
        pid_t result = waitpid(child, &status, WNOHANG);
        if (result == child) {
            printf("keyboard target=%ld elapsed_ms=%lld child-exit\n", (long)child,
                   (long long)elapsed);
            report("child-completion", elapsed, probe ? shots : -1, 0, 1, status);
            return !WIFEXITED(status) || WEXITSTATUS(status) != 0 || tick == 0 ||
                   (released && elapsed <= 5000) || (probe && (!acknowledged || shots != 1));
        }
        if (result < 0) {
            reason = "wait";
            error = errno;
            break;
        }
        int down = !released || elapsed < 5000;
        for (unsigned i = 0; i < 3; i++) {
            if (i == 2 && (!probe || acknowledged))
                continue;
            CGEventRef event =
                CGEventCreateKeyboardEvent(NULL, keys[i], i == 2 ? shots == 0 : down != 0);
            if (event == NULL) {
                reason = "event";
                goto stop;
            }
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
        if (nanosleep(&delay, NULL) != 0) {
            reason = "sleep";
            error = errno;
            break;
        }
    }
stop:
    kill(child, SIGKILL);
    waitpid(child, &status, 0);
    /* Cleanup status is not an observation of the child's state at the stop. */
    report(reason, elapsed, probe ? shots : -1, error, 0, 0);
    return 1;
}
