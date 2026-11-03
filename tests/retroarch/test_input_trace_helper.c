/* Only synthetic children and mocked platform APIs. No frontend or input device. */
#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
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
static uint32_t mock_posts[3][2];
static int capture_after_tick = -1;
static char capture_path[4096];
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
    unsigned key_index = event.key == 126 ? 0u : event.key == 6 ? 1u : event.key == 100 ? 2u : 3u;
    if (key_index >= 3u)
        bad = 1;
    else
        mock_posts[key_index][event.down != 0]++;
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
    if (ticks == capture_after_tick) {
        int fd = open(capture_path, O_CREAT | O_EXCL | O_WRONLY, 0600);
        if (fd < 0 || close(fd) != 0)
            bad = 1;
    }
    const struct timespec short_delay = {0, 1000000};
    return real_sleep(&short_delay);
}
static int directory_template(char *dir, size_t capacity, const char *root)
{
    int n = snprintf(dir, capacity, "%s/helper-trace-XXXXXX", root);
    return n >= 0 && (size_t)n < capacity;
}
static int same_file(const struct stat *left, const struct stat *right)
{
    return left->st_dev == right->st_dev && left->st_ino == right->st_ino;
}
static int descriptor_check(int core_fd, int ack_fd)
{
    const char *dir = getenv("MUHTARI_INPUT_TRACE_DIR");
    char path[4096 + sizeof "/helper.trace"];
    struct stat core, helper, directory, info;
    REQUIRE(dir != NULL);
    int n = snprintf(path, sizeof path, "%s/helper.trace", dir);
    REQUIRE(n >= 0 && (size_t)n < sizeof path);
    /* stat does not add an inspector descriptor to the observed fd3..127 set. */
    REQUIRE(fstat(core_fd, &core) == 0 && stat(path, &helper) == 0 && stat(dir, &directory) == 0);
    for (int i = 3; i < 128; i++) {
        if (i == core_fd || i == ack_fd || fstat(i, &info) != 0)
            continue;
        const char *kind = same_file(&info, &helper)      ? "helper file"
                           : same_file(&info, &directory) ? "trace directory"
                           : same_file(&info, &core)      ? "extra core alias"
                                                          : NULL;
        if (kind != NULL) {
            fprintf(stderr, "trace helper leaked %s descriptor\n", kind);
            return 1;
        }
    }
    return 0;
}
struct child_calls {
    pid_t (*wait)(pid_t, int *, int);
    int (*signal)(pid_t, int);
    int (*clock)(clockid_t, struct timespec *);
};
static pid_t descriptor_reap(pid_t child, int *status, const struct timespec *deadline,
                             const struct child_calls *calls)
{
    const struct timespec delay = {0, 1000000};
    for (;;) {
        pid_t result = calls->wait(child, status, WNOHANG);
        if (result == child)
            return child;
        if (result < 0 && errno != EINTR) {
            fprintf(stderr, "descriptor child wait failed: %d\n", errno);
            return -1;
        }
        struct timespec now;
        if (calls->clock(CLOCK_MONOTONIC, &now) != 0 || now.tv_sec > deadline->tv_sec ||
            (now.tv_sec == deadline->tv_sec && now.tv_nsec >= deadline->tv_nsec)) {
            int sent;
            do {
                sent = calls->signal(child, SIGKILL);
            } while (sent < 0 && errno == EINTR);
            /* ESRCH can mean the exact owned child exited between wait and kill. */
            if (sent < 0 && errno != ESRCH) {
                fprintf(stderr, "descriptor child kill failed: %d\n", errno);
                return -1;
            }
            /* The unchanged enclosing five-second consumer bounds a stalled kernel wait.
             * SIGKILL alone is not reap confirmation; only this exact PID is success. */
            do {
                result = calls->wait(child, status, 0);
            } while (result < 0 && errno == EINTR);
            if (result != child) {
                fprintf(stderr, "descriptor child reap unconfirmed: %ld\n", (long)result);
                return -1;
            }
            return child;
        }
        real_sleep(&delay);
    }
}
enum reap_case {
    REAP_NORMAL,
    REAP_BEFORE,
    REAP_DEADLINE,
    REAP_CLOCK_ERROR,
    REAP_POLL_INTERRUPTED,
    REAP_KILL_INTERRUPTED,
    REAP_KILL_ABSENT,
    REAP_KILL_ERROR,
    REAP_WAIT_INTERRUPTED,
    REAP_WAIT_ERROR,
    REAP_WAIT_ZERO,
    REAP_WAIT_OTHER,
    REAP_POLL_ERROR
};
static enum reap_case reap_case;
static int reap_waits, reap_signals, reap_clocks, reap_bad;
static pid_t reap_wait(pid_t child, int *status, int options)
{
    reap_waits++;
    reap_bad |= child != 42 || status == NULL;
    if (reap_waits == 1) {
        reap_bad |= options != WNOHANG;
        if (reap_case == REAP_NORMAL)
            return child;
        if (reap_case == REAP_POLL_ERROR || reap_case == REAP_POLL_INTERRUPTED) {
            errno = reap_case == REAP_POLL_ERROR ? ECHILD : EINTR;
            return -1;
        }
        return 0;
    }
    reap_bad |= options != (reap_case == REAP_BEFORE ? WNOHANG : 0);
    if (reap_case == REAP_WAIT_INTERRUPTED && reap_waits == 2) {
        errno = EINTR;
        return -1;
    }
    if (reap_case == REAP_WAIT_ERROR) {
        errno = ECHILD;
        return -1;
    }
    if (reap_case == REAP_WAIT_ZERO)
        return 0;
    if (reap_case == REAP_WAIT_OTHER)
        return child + 1;
    *status = SIGKILL;
    return child;
}
static int reap_signal(pid_t child, int signal)
{
    reap_signals++;
    reap_bad |= child != 42 || signal != SIGKILL;
    if (reap_case == REAP_KILL_INTERRUPTED && reap_signals == 1) {
        errno = EINTR;
        return -1;
    }
    if (reap_case == REAP_KILL_ABSENT || reap_case == REAP_KILL_ERROR) {
        errno = reap_case == REAP_KILL_ABSENT ? ESRCH : EPERM;
        return -1;
    }
    return 0;
}
static int reap_clock(clockid_t id, struct timespec *now)
{
    reap_clocks++;
    reap_bad |= id != CLOCK_MONOTONIC;
    now->tv_sec = 4;
    now->tv_nsec = reap_case == REAP_BEFORE ? 6 : 7;
    return reap_case == REAP_CLOCK_ERROR ? -1 : 0;
}
static int reap_controls(void)
{
    const struct child_calls calls = {reap_wait, reap_signal, reap_clock};
    const struct timespec deadline = {4, 7};
    for (reap_case = REAP_NORMAL; reap_case <= REAP_POLL_ERROR; reap_case++) {
        reap_waits = reap_signals = reap_clocks = reap_bad = 0;
        int status = 0;
        pid_t result = descriptor_reap(42, &status, &deadline, &calls);
        int failure = reap_case == REAP_KILL_ERROR || reap_case == REAP_WAIT_ERROR ||
                      reap_case == REAP_WAIT_ZERO || reap_case == REAP_WAIT_OTHER ||
                      reap_case == REAP_POLL_ERROR;
        int no_kill =
            reap_case == REAP_NORMAL || reap_case == REAP_BEFORE || reap_case == REAP_POLL_ERROR;
        REQUIRE(result == (failure ? -1 : 42) && !reap_bad);
        REQUIRE(reap_signals == (no_kill ? 0 : reap_case == REAP_KILL_INTERRUPTED ? 2 : 1));
        REQUIRE(reap_waits == (reap_case == REAP_NORMAL || reap_case == REAP_POLL_ERROR ||
                                       reap_case == REAP_KILL_ERROR
                                   ? 1
                               : reap_case == REAP_WAIT_INTERRUPTED ? 3
                                                                    : 2));
        REQUIRE(reap_clocks == (reap_case == REAP_NORMAL || reap_case == REAP_POLL_ERROR ? 0 : 1));
        if (!failure && reap_case != REAP_NORMAL)
            REQUIRE(WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL);
    }
    puts("owned child deadline, interrupt and terminal-error reap controls passed");
    return 0;
}
static int descriptor_control(const char *exe, const char *root, const char *mode)
{
    char dir[4096], path[sizeof dir + sizeof "/helper.trace"];
    REQUIRE(directory_template(dir, sizeof dir, root) && mkdtemp(dir) != NULL);
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_DIR", dir, 1) == 0);
    REQUIRE(snprintf(path, sizeof path, "%s/core.trace", dir) > 0);
    int core = open(path, O_CREAT | O_EXCL | O_RDWR, 0600);
    REQUIRE(core >= 3 && fcntl(core, F_SETFD, 0) == 0);
    REQUIRE(snprintf(path, sizeof path, "%s/helper.trace", dir) > 0);
    int helper = open(path, O_CREAT | O_EXCL | O_RDWR, 0600);
    REQUIRE(helper >= 3 && close(helper) == 0);
    REQUIRE(snprintf(path, sizeof path, "%s/sentinel", dir) > 0);
    int sentinel = open(path, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, 0600);
    REQUIRE(sentinel >= 3 && write(sentinel, "keep", 4) == 4);
    int alias = -1;
    const char *diagnostic = NULL;
    if (strcmp(mode, "inherit") == 0) {
        REQUIRE(fcntl(sentinel, F_SETFD, 0) == 0);
    } else if (strcmp(mode, "helper") == 0) {
        REQUIRE(snprintf(path, sizeof path, "%s/helper.trace", dir) > 0);
        alias = open(path, O_RDONLY);
        diagnostic = "trace helper leaked helper file descriptor\n";
    } else if (strcmp(mode, "directory") == 0) {
        alias = open(dir, O_RDONLY);
        diagnostic = "trace helper leaked trace directory descriptor\n";
    } else if (strcmp(mode, "core") == 0) {
        alias = dup(core);
        diagnostic = "trace helper leaked extra core alias descriptor\n";
    } else {
        REQUIRE(strcmp(mode, "clean") == 0 || strcmp(mode, "cloexec") == 0);
    }
    REQUIRE((diagnostic == NULL || alias >= 3) && sentinel < 128 && alias < 128);
    int ack[2], errors[2];
    REQUIRE(pipe(ack) == 0 && pipe(errors) == 0);
    char core_text[32], ack_text[32];
    snprintf(core_text, sizeof core_text, "%d", core);
    snprintf(ack_text, sizeof ack_text, "%d", ack[1]);
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_FD", core_text, 1) == 0);
    struct timespec deadline;
    REQUIRE(clock_gettime(CLOCK_MONOTONIC, &deadline) == 0);
    deadline.tv_sec += 4;
    pid_t child = native_fork();
    REQUIRE(child >= 0);
    if (child == 0) {
        close(ack[0]);
        close(errors[0]);
        if (strcmp(mode, "clean") == 0)
            close(sentinel);
        if (dup2(errors[1], STDERR_FILENO) == -1)
            _exit(126);
        close(errors[1]);
        execl(exe, exe, "--fixture", ack_text, (char *)NULL);
        _exit(127);
    }
    close(ack[1]);
    close(errors[1]);
    close(core);
    struct pollfd ready = {ack[0], POLLIN, 0};
    char byte = 0;
    ssize_t count = poll(&ready, 1, 3000) > 0 ? read(ack[0], &byte, 1) : -1;
    int killed = 0;
    if (count != 0)
        killed = kill(child, SIGKILL) == 0;
    int status = 0;
    const struct child_calls calls = {waitpid, kill, clock_gettime};
    pid_t reaped = descriptor_reap(child, &status, &deadline, &calls);
    char error[256] = {0}, bytes[4];
    ssize_t length = -1;
    if (reaped == child)
        length = read(errors[0], error, sizeof error - 1);
    int retained = lseek(sentinel, 0, SEEK_SET) == 0 && read(sentinel, bytes, 4) == 4 &&
                   memcmp(bytes, "keep", 4) == 0;
    close(ack[0]);
    close(errors[0]);
    close(sentinel);
    if (alias >= 0)
        close(alias);
    REQUIRE(reaped == child && retained);
    if (diagnostic != NULL) {
        REQUIRE(count == 0 && WIFEXITED(status) && WEXITSTATUS(status) == 1 &&
                length == (ssize_t)strlen(diagnostic) && strcmp(error, diagnostic) == 0);
    } else {
        REQUIRE(count == 1 && byte == '!' && killed && WIFSIGNALED(status) &&
                WTERMSIG(status) == SIGKILL && length == 0);
    }
    printf("descriptor %s: exact oracle, child reaped, sentinel retained\n", mode);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--identity") == 0) {
        REQUIRE(reap_controls() == 0);
        struct stat left = {0}, right = {0};
        left.st_dev = 1;
        left.st_ino = 7;
        right = left;
        REQUIRE(same_file(&left, &right));
        right.st_dev = 2;
        REQUIRE(!same_file(&left, &right));
        right = left;
        right.st_ino = 8;
        REQUIRE(!same_file(&left, &right));
        puts("device and inode identity passed");
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "--descriptor-control") == 0)
        return descriptor_control(argv[0], argv[3], argv[2]);
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
        /* Unrelated inherited descriptors are allowed; owned trace aliases are not. */
        if (descriptor_check(fd, atoi(ack_text)) != 0)
            return 1;
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
    REQUIRE(snprintf(capture_path, sizeof capture_path, "%s/capture.png", raw) > 0);
    capture_after_tick = -1;
    char sync_fd[32];
    snprintf(sync_fd, sizeof sync_fd, "%d", sync_pipe[1]);
    char *args[] = {"synthetic-helper", "--probe", raw, argv[0], "--fixture", sync_fd, NULL};
    REQUIRE(helper_main(6, args) == 1 && !bad && posts == 150);
    REQUIRE(waitpid(owned, NULL, WNOHANG) == -1); /* helper reaped exactly its own killed child */
    it_read(helper, IT_HELPER, &h);
    it_read(core, IT_CORE, &c);
    REQUIRE(h.status == IT_COMPLETE && h.count == 7 && c.status == IT_UNFINISHED && c.count == 3);
    for (unsigned i = 0; i < 3u; i++) {
        REQUIRE(keyboard_trace_keys[i].down_count == mock_posts[i][1]);
        REQUIRE(keyboard_trace_keys[i].up_count == mock_posts[i][0]);
        REQUIRE(mock_posts[i][1] == 50u && mock_posts[i][0] == 0u);
        REQUIRE(keyboard_trace_keys[i].first_down_ms == 0 &&
                keyboard_trace_keys[i].last_down_ms == 4900);
    }
    REQUIRE(h.records[3].type == IT_POST && h.records[3].a == 126u && h.records[3].b == 1u &&
            h.records[3].c == (uint32_t)owned);
    REQUIRE(h.records[4].type == IT_POST && h.records[4].a == 6u && h.records[4].b == 1u &&
            h.records[4].c == (uint32_t)owned);
    REQUIRE(h.records[5].type == IT_POST && h.records[5].a == 100u && h.records[5].b == 1u &&
            h.records[5].c == (uint32_t)owned);
    REQUIRE(h.records[6].type == IT_END);
    REQUIRE(it_pair(&h, &c));
    /* The collection command itself rejects a helper interrupted before a terminal record. */
    fd = open(helper, O_RDWR | O_CLOEXEC);
    REQUIRE(fd >= 3);
    uint8_t complete[IT_LIMIT * IT_BYTES];
    size_t complete_bytes = (size_t)h.count * IT_BYTES;
    REQUIRE(complete_bytes <= sizeof complete &&
            read(fd, complete, complete_bytes) == (ssize_t)complete_bytes);
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
    REQUIRE(pwrite(fd, complete, complete_bytes, 0) == (ssize_t)complete_bytes);
    close(fd);
    c.records[0].pid++;
    REQUIRE(!it_pair(&h, &c));
    c.records[0].pid--;
    c.clock_ok = 0;
    REQUIRE(!it_pair(&h, &c));
    close(sync_pipe[0]);

    char raw_release[4096], capture_release[4096];
    REQUIRE(snprintf(raw_release, sizeof raw_release, "%s/raw-release", argv[1]) > 0);
    REQUIRE(mkdir(raw_release, 0700) == 0 && pipe(sync_pipe) == 0);
    REQUIRE(snprintf(capture_release, sizeof capture_release, "%s/capture.png", raw_release) > 0);
    char release_sync_fd[32];
    snprintf(release_sync_fd, sizeof release_sync_fd, "%d", sync_pipe[1]);
    REQUIRE(setenv("MUHTARI_TRACE_SYNC_FD", release_sync_fd, 1) == 0);
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_DIR", raw_release, 1) == 0);
    REQUIRE(snprintf(capture_path, sizeof capture_path, "%s", capture_release) > 0);
    capture_after_tick = 3;
    ticks = posts = bad = 0;
    memset(mock_posts, 0, sizeof mock_posts);
    char *release_args[] = {"synthetic-helper", "--probe", raw_release, argv[0], "--fixture",
                            release_sync_fd, NULL};
    REQUIRE(helper_main(6, release_args) == 1 && !bad && ticks == 450 && posts == 904);
    REQUIRE(waitpid(owned, NULL, WNOHANG) == -1);
    char helper_release[sizeof raw_release + sizeof "/helper.trace"];
    char core_release[sizeof raw_release + sizeof "/core.trace"];
    REQUIRE(snprintf(helper_release, sizeof helper_release, "%s/helper.trace", raw_release) > 0);
    REQUIRE(snprintf(core_release, sizeof core_release, "%s/core.trace", raw_release) > 0);
    it_read(helper_release, IT_HELPER, &h);
    it_read(core_release, IT_CORE, &c);
    REQUIRE(h.status == IT_COMPLETE && h.count == 10u && c.status == IT_UNFINISHED && c.count == 3u);
    REQUIRE(keyboard_trace_keys[0].down_count == 50u && keyboard_trace_keys[0].up_count == 400u);
    REQUIRE(keyboard_trace_keys[1].down_count == 50u && keyboard_trace_keys[1].up_count == 400u);
    REQUIRE(keyboard_trace_keys[2].down_count == 3u && keyboard_trace_keys[2].up_count == 1u);
    REQUIRE(keyboard_trace_keys[0].first_down_ms == 0 && keyboard_trace_keys[0].last_down_ms == 4900 &&
            keyboard_trace_keys[0].first_up_ms == 5000 && keyboard_trace_keys[0].last_up_ms == 44900);
    REQUIRE(keyboard_trace_keys[1].first_down_ms == 0 && keyboard_trace_keys[1].last_down_ms == 4900 &&
            keyboard_trace_keys[1].first_up_ms == 5000 && keyboard_trace_keys[1].last_up_ms == 44900);
    REQUIRE(keyboard_trace_keys[2].first_down_ms == 0 && keyboard_trace_keys[2].last_down_ms == 200 &&
            keyboard_trace_keys[2].first_up_ms == 300 && keyboard_trace_keys[2].last_up_ms == 300);
    REQUIRE(h.records[3].type == IT_POST && h.records[3].a == 126u && h.records[3].b == 1u);
    REQUIRE(h.records[4].type == IT_POST && h.records[4].a == 6u && h.records[4].b == 1u);
    REQUIRE(h.records[5].type == IT_POST && h.records[5].a == 100u && h.records[5].b == 1u);
    REQUIRE(h.records[6].type == IT_POST && h.records[6].a == 100u && h.records[6].b == 0u);
    REQUIRE(h.records[7].type == IT_POST && h.records[7].a == 126u && h.records[7].b == 0u);
    REQUIRE(h.records[8].type == IT_POST && h.records[8].a == 6u && h.records[8].b == 0u);
    REQUIRE(h.records[9].type == IT_END);
    REQUIRE(close(sync_pipe[0]) == 0);
    REQUIRE(unsetenv("MUHTARI_INPUT_TRACE_DIR") == 0);
    puts("exclusive descriptors, no overwrite, synthetic kill survival and paired collection "
         "passed");
    printf("trace_dir=%s\n", dir);
    return 0;
}
