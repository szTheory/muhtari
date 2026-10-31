/* Only synthetic children and mocked platform APIs. No frontend or input device. */
#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>
static int sync_pipe[2], ticks, posts, bad;
static pid_t owned;
static pid_t native_fork(void)
{
    return fork();
}
static int real_sleep(const struct timespec *delay)
{
    return nanosleep(delay, NULL);
}
static pid_t helper_fork(void);
static int helper_clock(clockid_t id, struct timespec *now);
static int helper_sleep(const struct timespec *delay, struct timespec *remaining);
int helper_main(int argc, char **argv);
#define main helper_main
#define fork helper_fork
#define clock_gettime helper_clock
#define nanosleep helper_sleep
#define MUHTARI_INPUT_TRACE
#include "keyboard_preflight.c"
#undef main
#undef fork
#undef clock_gettime
#undef nanosleep
#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "trace helper line%d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
struct keyboard_event {
    CGKeyCode key;
    int down;
};
static struct keyboard_event event;
int CGPreflightPostEventAccess(void)
{
    return 1;
}
CGEventRef CGEventCreateKeyboardEvent(void *source, CGKeyCode key, int down)
{
    bad |= source != NULL || (key != 126 && key != 6 && key != 100);
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
    bad |= pid != owned || e != &event;
    posts++;
}
void CFRelease(CGEventRef e)
{
    bad |= e != &event;
}
static pid_t helper_fork(void)
{
    pid_t pid = native_fork();
    if (pid > 0) {
        close(sync_pipe[1]);
        char ack;
        if (read(sync_pipe[0], &ack, 1) != 1)
            bad = 1;
        owned = pid;
    } else if (pid == 0)
        close(sync_pipe[0]);
    return pid;
}
static int helper_clock(clockid_t id, struct timespec *now)
{
    bad |= id != CLOCK_MONOTONIC;
    now->tv_sec = ticks / 10;
    now->tv_nsec = (ticks % 10) * 100000000L;
    return 0;
}
static int helper_sleep(const struct timespec *delay, struct timespec *remaining)
{
    bad |= delay->tv_sec != 0 || delay->tv_nsec != 100000000 || remaining != NULL;
    ticks++;
    const struct timespec short_delay = {0, 1000000};
    return real_sleep(&short_delay);
}
static int directory_template(char *dir, size_t capacity, const char *root)
{
    int n = snprintf(dir, capacity, "%s/helper-trace-XXXXXX", root);
    return n >= 0 && (size_t)n < capacity;
}
int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--probe") == 0) {
        REQUIRE(pipe(sync_pipe) == 0);
        char value[32];
        snprintf(value, sizeof value, "%d", sync_pipe[1]);
        REQUIRE(setenv("MUHTARI_TRACE_SYNC_FD", value, 1) == 0);
        return helper_main(argc, argv);
    }
    if ((argc == 3 && strcmp(argv[1], "--fixture") == 0) ||
        (argc > 1 && strcmp(argv[1], "--verbose") == 0)) {
        const char *text = getenv("MUHTARI_INPUT_TRACE_FD");
        REQUIRE(text != NULL);
        int fd = atoi(text);
        const char *ack_text =
            strcmp(argv[1], "--fixture") == 0 ? argv[2] : getenv("MUHTARI_TRACE_SYNC_FD");
        REQUIRE(ack_text != NULL);
        /* Helper file and directory cannot leak through exec. */
        struct stat info, trace_info;
        REQUIRE(fstat(fd, &trace_info) == 0);
        for (int i = 3; i < 128; i++)
            if (i != fd && i != atoi(ack_text) && fstat(i, &info) == 0 && S_ISREG(info.st_mode))
                REQUIRE(info.st_ino != trace_info.st_ino && (fcntl(i, F_GETFD) & FD_CLOEXEC));
        it_writer w;
        REQUIRE(it_begin(&w, fd, IT_CORE, (uint32_t)getppid()));
        if (getenv("MUHTARI_TRACE_FIXTURE_WRITE_FAILURE") != NULL)
            close(fd); /* A real EBADF write after a valid INIT, on a record boundary. */
        it_emit(&w, IT_POLL, 1, 0, 0);
        it_emit(&w, IT_STATE, 4, 1, 0);
        REQUIRE(write(atoi(ack_text), "!", 1) == 1);
        for (;;)
            pause();
    }
    REQUIRE(argc == 3);
    char dir[4096];
    char helper[sizeof dir + sizeof "/helper.trace" - 1];
    char core[sizeof dir + sizeof "/core.trace" - 1];
    char raw[sizeof dir + sizeof "/raw" - 1];
    /* Formatting-only neighbors never reach the filesystem with long paths. */
    char root[sizeof dir];
    size_t fit = sizeof dir - sizeof "/helper-trace-XXXXXX";
    memset(root, 'x', sizeof root);
    root[fit] = '\0';
    REQUIRE(directory_template(dir, sizeof dir, root) && strlen(dir) == sizeof dir - 1);
    REQUIRE(snprintf(helper, sizeof helper, "%s/helper.trace", dir) == (int)sizeof helper - 1);
    REQUIRE(snprintf(core, sizeof core, "%s/core.trace", dir) == (int)sizeof core - 1);
    REQUIRE(snprintf(raw, sizeof raw, "%s/raw", dir) == (int)sizeof raw - 1);
    root[fit] = 'x';
    root[fit + 1] = '\0';
    REQUIRE(!directory_template(dir, sizeof dir, root));
    REQUIRE(directory_template(dir, sizeof dir, "") && strcmp(dir, "/helper-trace-XXXXXX") == 0);
    REQUIRE(directory_template(dir, sizeof dir, "x") && strcmp(dir, "x/helper-trace-XXXXXX") == 0);
    REQUIRE(directory_template(dir, sizeof dir, argv[1]));
    REQUIRE(mkdtemp(dir) != NULL);
    REQUIRE(snprintf(helper, sizeof helper, "%s/helper.trace", dir) ==
            (int)(strlen(dir) + sizeof "/helper.trace" - 1));
    REQUIRE(snprintf(core, sizeof core, "%s/core.trace", dir) ==
            (int)(strlen(dir) + sizeof "/core.trace" - 1));
    REQUIRE(snprintf(raw, sizeof raw, "%s/raw", dir) == (int)(strlen(dir) + sizeof "/raw" - 1));
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_DIR", dir, 1) == 0);
    REQUIRE(keyboard_trace_setup());
    REQUIRE((fcntl(keyboard_trace.fd, F_GETFD) & FD_CLOEXEC) != 0);
    REQUIRE((fcntl(keyboard_trace_child, F_GETFD) & FD_CLOEXEC) == 0);
    int child_fd = keyboard_trace_child;
    keyboard_trace_parent(0);
    REQUIRE(fcntl(child_fd, F_GETFD) == -1);
    keyboard_trace_finish(0, 0);
    REQUIRE(!keyboard_trace_setup()); /* existing files never overwritten */
    it_trace h, c;
    it_read(helper, IT_HELPER, &h);
    REQUIRE(h.status == IT_COMPLETE && h.count == 2);
    REQUIRE(unlink(helper) == 0 && unlink(core) == 0);
    REQUIRE(symlink("sentinel", helper) == 0);
    REQUIRE(!keyboard_trace_setup());
    REQUIRE(unlink(helper) == 0);
    REQUIRE(chmod(dir, 0755) == 0 && !keyboard_trace_setup());
    REQUIRE(chmod(dir, 0700) == 0);
    /* Child path collision retains the first exclusive helper file and sentinel. */
    int fd = open(core, O_CREAT | O_EXCL | O_WRONLY, 0600);
    REQUIRE(fd >= 3 && write(fd, "sentinel", 8) == 8);
    close(fd);
    REQUIRE(!keyboard_trace_setup());
    char bytes[8];
    fd = open(core, O_RDONLY);
    REQUIRE(read(fd, bytes, sizeof bytes) == 8 && memcmp(bytes, "sentinel", 8) == 0);
    close(fd);
    REQUIRE(unlink(helper) == 0 && unlink(core) == 0);
    REQUIRE(mkdir(raw, 0700) == 0 && pipe(sync_pipe) == 0);
    char sync_fd[32];
    snprintf(sync_fd, sizeof sync_fd, "%d", sync_pipe[1]);
    char *args[] = {"synthetic-helper", "--probe", raw, argv[0], "--fixture", sync_fd, NULL};
    REQUIRE(helper_main(6, args) == 1 && !bad && posts > 0);
    REQUIRE(waitpid(owned, NULL, WNOHANG) == -1); /* helper reaped exactly its own killed child */
    it_read(helper, IT_HELPER, &h);
    it_read(core, IT_CORE, &c);
    REQUIRE(h.status == IT_FULL && c.status == IT_UNFINISHED && c.count == 3);
    REQUIRE(it_pair(&h, &c));
    /* The collection command itself rejects a helper interrupted before a terminal record. */
    fd = open(helper, O_RDWR | O_CLOEXEC);
    REQUIRE(fd >= 3);
    uint8_t complete[IT_LIMIT * IT_BYTES];
    REQUIRE(read(fd, complete, sizeof complete) == (ssize_t)sizeof complete);
    REQUIRE(ftruncate(fd, 3 * IT_BYTES) == 0);
    pid_t collector = native_fork();
    REQUIRE(collector >= 0);
    if (collector == 0) {
        execl(argv[2], argv[2], "--collect", dir, (char *)NULL);
        _exit(127);
    }
    int status;
    REQUIRE(waitpid(collector, &status, 0) == collector && WIFEXITED(status) &&
            WEXITSTATUS(status) == 1);
    REQUIRE(pwrite(fd, complete, sizeof complete, 0) == (ssize_t)sizeof complete);
    close(fd);
    c.records[0].pid++;
    REQUIRE(!it_pair(&h, &c));
    c.records[0].pid--;
    c.clock_ok = 0;
    REQUIRE(!it_pair(&h, &c));
    close(sync_pipe[0]);
    REQUIRE(unsetenv("MUHTARI_INPUT_TRACE_DIR") == 0);
    puts("exclusive descriptors, no overwrite, synthetic kill survival and paired collection "
         "passed");
    printf("trace_dir=%s\n", dir);
    return 0;
}
