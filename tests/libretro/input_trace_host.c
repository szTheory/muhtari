/* Synthetic in-process frontend: no platform/input-device APIs or content launch. */
#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "libretro.h"
#include "input_trace.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "trace host line%d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
typedef struct {
    void (*set_environment)(retro_environment_t);
    void (*set_input_poll)(retro_input_poll_t);
    void (*set_input_state)(retro_input_state_t);
    void (*set_video_refresh)(retro_video_refresh_t);
    void (*set_audio_sample_batch)(retro_audio_sample_batch_t);
    void (*set_controller_port_device)(unsigned, unsigned);
    void (*init)(void), (*deinit)(void), (*run)(void);
    bool (*load_game)(const struct retro_game_info *);
} api;
typedef struct {
    uint64_t calls, video, audio, queries, polls, frames, samples;
} result;
static result observed;
static unsigned scenario, frame;
static volatile sig_atomic_t manual_alarm_count;

static void manual_alarm_handler(int signal_number)
{
    (void)signal_number;
    manual_alarm_count = 1;
}

enum { MANUAL_WALL_MS = 75000u, MANUAL_GRACE_MS = 2000u };
enum manual_outcome {
    MANUAL_EXITED = 1,
    MANUAL_EXEC_FAILED,
    MANUAL_TIMED_OUT,
    MANUAL_WAIT_FAILED,
    MANUAL_UNREAPED
};
typedef struct {
    pid_t child;
    int outcome, status, exec_error, wait_error, signals_sent;
} manual_result;

static uint64_t manual_now_ms(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0;
    return (uint64_t)now.tv_sec * 1000u + (uint64_t)now.tv_nsec / 1000000u;
}

static int manual_wait_once(pid_t child, int *status, int *error)
{
    pid_t result = waitpid(child, status, WNOHANG);
    if (result == child)
        return 1;
    if (result == 0)
        return 0;
    *error = errno;
    if (errno == EINTR)
        return -2;
    return -1;
}

static int manual_sleep_10ms(void)
{
    struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
    return nanosleep(&delay, NULL) == 0 || errno == EINTR;
}

static int manual_wait_retry(pid_t child, int *status, int *error, uint64_t start_ms,
                             uint64_t budget_ms)
{
    for (;;) {
        int result = manual_wait_once(child, status, error);
        if (result != -2)
            return result;
        uint64_t now = manual_now_ms();
        if (now == 0) {
            *error = EIO;
            return -1;
        }
        if (now - start_ms >= budget_ms)
            return -2;
        if (!manual_sleep_10ms()) {
            *error = errno;
            return -1;
        }
    }
}

static int manual_child_reaped(pid_t child)
{
    uint64_t start = manual_now_ms();
    int status = 0, error = 0;
    int result = manual_wait_retry(child, &status, &error, start, 1000u);
    return result < 0 && error == ECHILD;
}

static int manual_test_reap_killed(pid_t child)
{
    uint64_t start = manual_now_ms();
    int status = 0, error = 0;
    for (;;) {
        int wait_result = manual_wait_retry(child, &status, &error, start, 1000u);
        if (wait_result > 0)
            return WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL;
        if (wait_result < 0)
            return 0;
        uint64_t now = manual_now_ms();
        if (now == 0 || now - start >= 1000u || !manual_sleep_10ms())
            return 0;
    }
}

static int manual_open_traces(const char *trace_dir, int *dir_fd, int *helper_fd, int *core_fd,
                              it_writer *helper)
{
    *dir_fd = *helper_fd = *core_fd = -1;
    *dir_fd = open(trace_dir, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (*dir_fd < 0)
        return 0;
    struct stat dir_info;
    if (fstat(*dir_fd, &dir_info) != 0 || !S_ISDIR(dir_info.st_mode) ||
        (dir_info.st_mode & 0777) != 0700 || dir_info.st_uid != getuid())
        return 0;
    *helper_fd =
        openat(*dir_fd, "helper.trace", O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (*helper_fd < 3)
        return 0;
    *core_fd =
        openat(*dir_fd, "core.trace", O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (*core_fd < 3)
        return 0;
    struct stat helper_info, core_info;
    if (fstat(*helper_fd, &helper_info) != 0 || fstat(*core_fd, &core_info) != 0 ||
        !S_ISREG(helper_info.st_mode) || !S_ISREG(core_info.st_mode) ||
        (helper_info.st_mode & 0777) != 0600 || (core_info.st_mode & 0777) != 0600 ||
        helper_info.st_uid != getuid() || core_info.st_uid != getuid() ||
        !it_begin(helper, *helper_fd, IT_HELPER, 0))
        return 0;
    return 1;
}

static int manual_exec_error(int fd, int *exec_error)
{
    ssize_t count = read(fd, exec_error, sizeof *exec_error);
    if (count == 0)
        return 0;
    if (count < 0)
        return errno == EINTR ? -2 : -1;
    return count == (ssize_t)sizeof *exec_error ? 1 : -1;
}

static int manual_wait_for_exec(int fd, uint64_t start_ms, uint64_t wall_ms, int *exec_error)
{
    struct pollfd status_pipe = {.fd = fd, .events = POLLIN | POLLHUP};
    for (;;) {
        status_pipe.revents = 0;
        int ready = poll(&status_pipe, 1, 10);
        if (ready < 0) {
            if (errno == EINTR) {
                uint64_t now = manual_now_ms();
                if (now == 0 || now - start_ms >= wall_ms)
                    return -2;
                continue;
            }
            return -1;
        }
        if (ready > 0 && (status_pipe.revents & (POLLIN | POLLHUP | POLLERR)) != 0) {
            int result = manual_exec_error(fd, exec_error);
            if (result != -2)
                return result;
        }
        uint64_t now = manual_now_ms();
        if (now == 0 || now - start_ms >= wall_ms)
            return -2;
    }
}

static int manual_launch(const char *trace_dir, char *const app_argv[], uint64_t wall_ms,
                         uint64_t grace_ms, int force_unreaped, manual_result *result)
{
    memset(result, 0, sizeof *result);
    int dir_fd, helper_fd, core_fd;
    it_writer helper;
    if (!manual_open_traces(trace_dir, &dir_fd, &helper_fd, &core_fd, &helper)) {
        fprintf(stderr, "manual launcher: trace directory or exclusive trace files are invalid\n");
        if (core_fd >= 0)
            close(core_fd);
        if (helper_fd >= 0)
            close(helper_fd);
        if (dir_fd >= 0)
            close(dir_fd);
        return 2;
    }
    int exec_pipe[2];
    if (pipe(exec_pipe) != 0) {
        int saved = errno;
        close(core_fd);
        close(helper_fd);
        close(dir_fd);
        fprintf(stderr, "manual launcher: exec-status pipe failed: %s\n", strerror(saved));
        return 2;
    }
    if (fcntl(exec_pipe[1], F_SETFD, FD_CLOEXEC) != 0) {
        int saved = errno;
        close(exec_pipe[0]);
        close(exec_pipe[1]);
        close(core_fd);
        close(helper_fd);
        close(dir_fd);
        fprintf(stderr, "manual launcher: exec-status pipe setup failed: %s\n", strerror(saved));
        return 2;
    }
    uint64_t start = manual_now_ms();
    pid_t child = fork();
    if (child < 0) {
        int saved = errno;
        close(exec_pipe[0]);
        close(exec_pipe[1]);
        close(core_fd);
        it_emit(&helper, IT_END, MANUAL_WAIT_FAILED, (uint32_t)saved, 0);
        close(helper_fd);
        close(dir_fd);
        fprintf(stderr, "manual launcher: fork failed: %s\n", strerror(saved));
        return 2;
    }
    if (child == 0) {
        close(exec_pipe[0]);
        close(helper_fd);
        close(dir_fd);
        char trace_fd_text[32];
        snprintf(trace_fd_text, sizeof trace_fd_text, "%d", core_fd);
        if (setenv("MUHTARI_INPUT_TRACE_FD", trace_fd_text, 1) != 0)
            goto exec_failed;
        if (fcntl(core_fd, F_SETFD, 0) != 0)
            goto exec_failed;
        if (getenv("MUHTARI_MANUAL_INPUT_TEST_CHILD") != NULL) {
            char helper_text[32], dir_text[32];
            snprintf(helper_text, sizeof helper_text, "%d", helper_fd);
            snprintf(dir_text, sizeof dir_text, "%d", dir_fd);
            if (setenv("MUHTARI_MANUAL_HELPER_FD", helper_text, 1) != 0 ||
                setenv("MUHTARI_MANUAL_DIR_FD", dir_text, 1) != 0)
                goto exec_failed;
        }
        execv(app_argv[0], app_argv);
    exec_failed: {
        int saved = errno ? errno : EIO;
        (void)write(exec_pipe[1], &saved, sizeof saved);
        _exit(127);
    }
    }
    result->child = child;
    close(exec_pipe[1]);
    close(core_fd);
    it_emit(&helper, IT_FORK, (uint32_t)child, 0, 0);
    it_emit(&helper, IT_START, 0, 0, 0);
    int pipe_error = manual_wait_for_exec(exec_pipe[0], start, wall_ms, &result->exec_error);
    close(exec_pipe[0]);
    int status = 0, wait_error = 0;
    int waited = 0;
    int wait_observation_failed = 0;
    if (pipe_error == -2)
        result->outcome = MANUAL_TIMED_OUT;
    else if (pipe_error < 0) {
        result->outcome = MANUAL_WAIT_FAILED;
        result->wait_error = errno ? errno : EIO;
    }
    if (pipe_error != -2) {
        while (!waited) {
            int observed_child = manual_wait_retry(child, &status, &wait_error, start, wall_ms);
            if (observed_child < 0 && observed_child != -2) {
                result->outcome = MANUAL_WAIT_FAILED;
                result->wait_error = wait_error;
                wait_observation_failed = 1;
                break;
            }
            if (observed_child > 0) {
                waited = 1;
                break;
            }
            uint64_t now = manual_now_ms();
            if (observed_child == -2 || now == 0 || now - start >= wall_ms) {
                result->outcome = MANUAL_TIMED_OUT;
                break;
            }
            if (!manual_sleep_10ms()) {
                result->outcome = MANUAL_WAIT_FAILED;
                result->wait_error = errno;
                break;
            }
        }
    }
    if (!waited && (result->outcome == MANUAL_TIMED_OUT || result->outcome == MANUAL_WAIT_FAILED)) {
        uint64_t term_grace_ms = grace_ms / 2u;
        uint64_t reap_grace_ms = grace_ms - term_grace_ms;
        uint64_t reap_deadline_ms = term_grace_ms + reap_grace_ms;
        uint64_t grace_start = manual_now_ms();
        int observed_child = grace_start == 0 ? -1
                                              : manual_wait_retry(child, &status, &wait_error,
                                                                  grace_start, term_grace_ms);
        if (observed_child > 0) {
            waited = 1;
            if (result->outcome != MANUAL_WAIT_FAILED)
                result->outcome = result->exec_error ? MANUAL_EXEC_FAILED : MANUAL_EXITED;
        } else if (observed_child == -1) {
            result->outcome = MANUAL_WAIT_FAILED;
            result->wait_error = wait_error ? wait_error : EINTR;
            wait_observation_failed = 1;
        } else {
            if (kill(child, SIGTERM) == 0)
                result->signals_sent++;
            else if (errno != ESRCH) {
                result->outcome = MANUAL_WAIT_FAILED;
                result->wait_error = errno;
            }
            while (!waited && !wait_observation_failed) {
                int observed_child2 =
                    manual_wait_retry(child, &status, &wait_error, grace_start, term_grace_ms);
                if (observed_child2 > 0) {
                    waited = 1;
                    break;
                }
                if (observed_child2 < 0 && observed_child2 != -2) {
                    result->outcome = MANUAL_WAIT_FAILED;
                    result->wait_error = wait_error;
                    wait_observation_failed = 1;
                    break;
                }
                if (observed_child2 == -2) {
                    break;
                }
                uint64_t now = manual_now_ms();
                if (now == 0 || now - grace_start >= term_grace_ms)
                    break;
                if (!manual_sleep_10ms()) {
                    result->outcome = MANUAL_WAIT_FAILED;
                    result->wait_error = errno;
                    break;
                }
            }
            if (!waited && !wait_observation_failed) {
                int observed_child3 =
                    manual_wait_retry(child, &status, &wait_error, grace_start, term_grace_ms);
                if (observed_child3 > 0)
                    waited = 1;
                else if (observed_child3 == -1) {
                    result->outcome = MANUAL_WAIT_FAILED;
                    result->wait_error = wait_error ? wait_error : EINTR;
                    wait_observation_failed = 1;
                } else {
                    if (kill(child, SIGKILL) == 0)
                        result->signals_sent++;
                    else if (errno != ESRCH) {
                        result->outcome = MANUAL_WAIT_FAILED;
                        result->wait_error = errno;
                    }
                    if (!wait_observation_failed) {
                        if (force_unreaped) {
                            result->outcome = MANUAL_UNREAPED;
                        } else {
                            for (;;) {
                                int observed_child4 = manual_wait_retry(
                                    child, &status, &wait_error, grace_start, reap_deadline_ms);
                                if (observed_child4 > 0) {
                                    waited = 1;
                                    break;
                                }
                                if (observed_child4 < 0 && observed_child4 != -2) {
                                    result->outcome = MANUAL_WAIT_FAILED;
                                    result->wait_error = wait_error;
                                    wait_observation_failed = 1;
                                    break;
                                }
                                uint64_t now = manual_now_ms();
                                if (observed_child4 == -2 || now == 0 ||
                                    now - grace_start >= reap_deadline_ms) {
                                    if (result->outcome != MANUAL_WAIT_FAILED)
                                        result->outcome = MANUAL_UNREAPED;
                                    break;
                                }
                                if (!manual_sleep_10ms()) {
                                    result->outcome = MANUAL_WAIT_FAILED;
                                    result->wait_error = errno;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (waited && result->outcome == 0)
        result->outcome = result->exec_error ? MANUAL_EXEC_FAILED : MANUAL_EXITED;
    result->status = status;
    it_emit(&helper, IT_END, (uint32_t)result->outcome, (uint32_t)status,
            (uint32_t)result->exec_error);
    close(helper_fd);
    close(dir_fd);
    if (result->outcome == MANUAL_EXEC_FAILED)
        fprintf(stderr, "manual launcher: exec failed: %s\n", strerror(result->exec_error));
    else if (result->outcome == MANUAL_TIMED_OUT)
        fprintf(stderr, "manual launcher: child exceeded the %llu ms wall deadline\n",
                (unsigned long long)wall_ms);
    else if (result->outcome == MANUAL_WAIT_FAILED)
        fprintf(stderr, "manual launcher: child wait failed: %s\n",
                strerror(result->wait_error ? result->wait_error : EIO));
    else if (result->outcome == MANUAL_UNREAPED)
        fprintf(stderr, "manual launcher: child %ld was not reaped before the cleanup bound\n",
                (long)child);
    else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        fprintf(stderr, "manual launcher: child exited with status %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        fprintf(stderr, "manual launcher: child ended from signal %d\n", WTERMSIG(status));
    else
        fprintf(stderr, "manual launcher: child exited normally\n");
    if (result->outcome == MANUAL_EXEC_FAILED)
        return 127;
    if (result->outcome == MANUAL_TIMED_OUT)
        return 124;
    if (result->outcome == MANUAL_UNREAPED)
        return 125;
    if (result->outcome != MANUAL_EXITED || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return 1;
    return 0;
}

static int manual_same(const char *actual, const char *expected)
{
    return actual != NULL && expected != NULL && strcmp(actual, expected) == 0;
}

static int manual_config_valid(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return 0;
    char line[256];
    int found = 0;
    while (fgets(line, sizeof line, file) != NULL) {
        if (strcmp(line, "pause_nonactive = \"false\"\n") == 0)
            found = 1;
        if (strstr(line, "pause_nonactive") != NULL &&
            strcmp(line, "pause_nonactive = \"false\"\n") != 0)
            found = 0;
    }
    int failed = ferror(file);
    int close_failed = fclose(file) != 0;
    return !failed && !close_failed && found;
}

static int manual_test_descriptors(int trace_fd)
{
    const char *helper_text = getenv("MUHTARI_MANUAL_HELPER_FD");
    const char *dir_text = getenv("MUHTARI_MANUAL_DIR_FD");
    if (helper_text == NULL || dir_text == NULL)
        return 0;
    char *end = NULL;
    errno = 0;
    long helper_fd = strtol(helper_text, &end, 10);
    if (errno || end == helper_text || *end || helper_fd < 3 || helper_fd > INT_MAX)
        return 0;
    errno = 0;
    long dir_fd = strtol(dir_text, &end, 10);
    if (errno || end == dir_text || *end || dir_fd < 3 || dir_fd > INT_MAX || helper_fd == dir_fd ||
        helper_fd == trace_fd || dir_fd == trace_fd)
        return 0;
    errno = 0;
    int helper_open = fcntl((int)helper_fd, F_GETFD);
    int helper_error = errno;
    errno = 0;
    int dir_open = fcntl((int)dir_fd, F_GETFD);
    int dir_error = errno;
    return helper_open == -1 && helper_error == EBADF && dir_open == -1 && dir_error == EBADF;
}

static int manual_fake_app(int argc, char **argv)
{
    const char *expected_config = getenv("MUHTARI_MANUAL_INPUT_EXPECT_CONFIG");
    const char *expected_core = getenv("MUHTARI_MANUAL_INPUT_EXPECT_CORE");
    const char *expected_content = getenv("MUHTARI_MANUAL_INPUT_EXPECT_CONTENT");
    if (argc != 8 || strcmp(argv[1], "--verbose") != 0 || strcmp(argv[2], "--config") != 0 ||
        strcmp(argv[4], "-L") != 0 || strcmp(argv[6], "--max-frames=3600") != 0 ||
        !manual_same(argv[3], expected_config) || !manual_same(argv[5], expected_core) ||
        !manual_same(argv[7], expected_content) || !manual_config_valid(argv[3])) {
        fprintf(stderr, "synthetic app rejected the RetroArch argv/config contract\n");
        return 1;
    }
    struct stat content_info;
    if (stat(argv[7], &content_info) != 0 || !S_ISREG(content_info.st_mode)) {
        fprintf(stderr, "synthetic app content path is absent or not a regular file\n");
        return 1;
    }
    const char *trace_text = getenv("MUHTARI_INPUT_TRACE_FD");
    char *end = NULL;
    errno = 0;
    long trace_value = trace_text == NULL ? -1 : strtol(trace_text, &end, 10);
    if (errno || trace_text == NULL || end == trace_text || *end || trace_value < 3 ||
        trace_value > INT_MAX || !manual_test_descriptors((int)trace_value) ||
        (fcntl((int)trace_value, F_GETFD) & FD_CLOEXEC) != 0) {
        fprintf(stderr, "synthetic app did not receive only the dedicated core trace fd\n");
        return 1;
    }
    it_writer writer;
    if (!it_begin(&writer, (int)trace_value, IT_CORE, (uint32_t)getppid()))
        return 1;
    it_emit(&writer, IT_POLL, 1, 0, 0);
    it_emit(&writer, IT_STATE, 4, 0, 0);
    it_emit(&writer, IT_STATE, 0, 0, 0);
    it_emit(&writer, IT_STATE, 4, 1, 0);
    it_emit(&writer, IT_STATE, 0, 1, 0);
    it_emit(&writer, IT_STATE, 4, 0, 0);
    it_emit(&writer, IT_STATE, 0, 0, 0);
    it_emit(&writer, IT_END, 0, 0, 0);
    close((int)trace_value);
    return 0;
}

static int manual_hang_app(void)
{
    const char *trace_text = getenv("MUHTARI_INPUT_TRACE_FD");
    char *end = NULL;
    errno = 0;
    long trace_value = trace_text == NULL ? -1 : strtol(trace_text, &end, 10);
    if (errno || trace_text == NULL || end == trace_text || *end || trace_value < 3 ||
        trace_value > INT_MAX)
        return 1;
    it_writer writer;
    if (!it_begin(&writer, (int)trace_value, IT_CORE, (uint32_t)getppid()))
        return 1;
    it_emit(&writer, IT_POLL, 1, 0, 0);
    const char *test_child = getenv("MUHTARI_MANUAL_INPUT_TEST_CHILD");
    if (test_child != NULL && strcmp(test_child, "interrupt") == 0) {
        (void)signal(SIGTERM, SIG_IGN);
        for (;;) {
            (void)kill(getppid(), SIGALRM);
            struct timespec delay = {.tv_sec = 0, .tv_nsec = 5000000L};
            (void)nanosleep(&delay, NULL);
        }
    } else {
        (void)signal(SIGTERM, SIG_IGN);
    }
    for (;;)
        pause();
}

static int manual_write_file(const char *path, const char *text)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (fd < 0)
        return 0;
    size_t length = strlen(text), offset = 0;
    while (offset < length) {
        ssize_t count = write(fd, text + offset, length - offset);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0) {
            close(fd);
            return 0;
        }
        offset += (size_t)count;
    }
    return close(fd) == 0;
}

static int manual_file_equals(const char *path, const char *expected)
{
    char actual[128];
    size_t expected_size = strlen(expected);
    int fd = open(path, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0 || expected_size >= sizeof actual) {
        if (fd >= 0)
            close(fd);
        return 0;
    }
    ssize_t count;
    do {
        count = read(fd, actual, sizeof actual);
    } while (count < 0 && errno == EINTR);
    int close_error = close(fd);
    return count == (ssize_t)expected_size && close_error == 0 &&
           memcmp(actual, expected, expected_size) == 0;
}

static int manual_test_case(const char *work, char *app_path, char *core_path, const char *fault,
                            uint64_t wall_ms, uint64_t grace_ms, manual_result *result,
                            char *trace_dir, size_t trace_size, const char *expected_outcome)
{
    char dir_template[4096], trace_path[4096], config[4096];
    char cart_dir_template[] = "/tmp/muhtari-manual-XXXXXX";
    char cart[4096], other[4096], wrong[4096], sentinel[4096], trace_conflict[4096];
    struct stat sentinel_before, conflict_before;
    int sentinel_before_valid = 0;
    int conflict_before_valid = 0;
    int count = snprintf(dir_template, sizeof dir_template, "%s/manual-%s-XXXXXX", work, fault);
    if (count < 0 || (size_t)count >= sizeof dir_template || mkdtemp(dir_template) == NULL)
        return 0;
    count = snprintf(trace_path, sizeof trace_path, "%s", dir_template);
    if (count < 0 || (size_t)count >= sizeof trace_path)
        return 0;
    count = snprintf(config, sizeof config, "%s/retroarch.cfg", dir_template);
    if (count < 0 || (size_t)count >= sizeof config ||
        !manual_write_file(config, "pause_nonactive = \"false\"\n"))
        return 0;
    char *cart_dir = mkdtemp(cart_dir_template);
    if (cart_dir == NULL)
        return 0;
    count = snprintf(cart, sizeof cart, "%s/input-history.a78", cart_dir);
    if (count < 0 || (size_t)count >= sizeof cart ||
        !manual_write_file(cart, "synthetic owned content fixture\n"))
        return 0;
    sentinel[0] = trace_conflict[0] = '\0';
    if (strcmp(fault, "existing-file") == 0) {
        count = snprintf(sentinel, sizeof sentinel, "%s/helper.trace", dir_template);
        if (count < 0 || (size_t)count >= sizeof sentinel ||
            !manual_write_file(sentinel, "preserve existing trace\n"))
            return 0;
    } else if (strcmp(fault, "existing-core-file") == 0) {
        count = snprintf(trace_conflict, sizeof trace_conflict, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof trace_conflict ||
            !manual_write_file(trace_conflict, "preserve existing core trace\n"))
            return 0;
        snprintf(sentinel, sizeof sentinel, "%s", trace_conflict);
    } else if (strcmp(fault, "symlink") == 0) {
        count = snprintf(sentinel, sizeof sentinel, "%s/sentinel", dir_template);
        if (count < 0 || (size_t)count >= sizeof sentinel ||
            !manual_write_file(sentinel, "preserve symlink target\n"))
            return 0;
        count = snprintf(trace_path, sizeof trace_path, "%s/traces", dir_template);
        if (count < 0 || (size_t)count >= sizeof trace_path || mkdir(trace_path, 0700) != 0)
            return 0;
        count = snprintf(trace_conflict, sizeof trace_conflict, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof trace_conflict ||
            symlink(sentinel, trace_conflict) != 0)
            return 0;
        if (lstat(trace_conflict, &conflict_before) != 0)
            return 0;
        conflict_before_valid = 1;
    }
    int path_fault = strcmp(fault, "existing-file") == 0 ||
                     strcmp(fault, "existing-core-file") == 0 || strcmp(fault, "symlink") == 0;
    if (path_fault) {
        if (lstat(sentinel, &sentinel_before) != 0)
            return 0;
        sentinel_before_valid = 1;
    }
    char *app[9] = {app_path,  "--verbose",         "--config", config, "-L",
                    core_path, "--max-frames=3600", cart,       NULL};
    if (strcmp(fault, "cap-omitted") == 0) {
        app[6] = app[7];
        app[7] = NULL;
    } else if (strcmp(fault, "cap-altered") == 0) {
        app[6] = "--max-frames=3601";
    } else if (strcmp(fault, "core") == 0) {
        snprintf(wrong, sizeof wrong, "%s/wrong", core_path);
        app[5] = wrong;
    } else if (strcmp(fault, "content") == 0) {
        snprintf(wrong, sizeof wrong, "%s/wrong.a78", cart_dir);
        app[7] = wrong;
    } else if (strcmp(fault, "config") == 0) {
        snprintf(wrong, sizeof wrong, "%s/wrong.cfg", dir_template);
        app[3] = wrong;
    } else if (strcmp(fault, "exec") == 0) {
        snprintf(wrong, sizeof wrong, "%s/does-not-exist", work);
        app[0] = wrong;
    } else if (strcmp(fault, "timeout") == 0 || strcmp(fault, "unreaped") == 0 ||
               strcmp(fault, "interrupt") == 0) {
        app[1] = "--manual-test-hang";
    }
    setenv("MUHTARI_MANUAL_INPUT_TEST_CHILD",
           strcmp(fault, "interrupt") == 0                                   ? "interrupt"
           : strcmp(fault, "timeout") == 0 || strcmp(fault, "unreaped") == 0 ? "hang"
                                                                             : "1",
           1);
    setenv("MUHTARI_MANUAL_INPUT_EXPECT_CONFIG", config, 1);
    setenv("MUHTARI_MANUAL_INPUT_EXPECT_CORE", core_path, 1);
    setenv("MUHTARI_MANUAL_INPUT_EXPECT_CONTENT", cart, 1);
    uint64_t launch_start = manual_now_ms();
    int rc =
        manual_launch(trace_path, app, wall_ms, grace_ms, strcmp(fault, "unreaped") == 0, result);
    uint64_t launch_end = manual_now_ms();
    if (strcmp(fault, "unreaped") == 0 && !manual_test_reap_killed(result->child))
        return 0;
    if (path_fault) {
        int path_ok = rc == 2 && result->child == 0 && sentinel_before_valid;
        if (!path_ok)
            fprintf(stderr, "manual control %s: refusal or child-zero assertion failed\n", fault);
        struct stat sentinel_after;
        const char *expected_sentinel =
            strcmp(fault, "existing-file") == 0        ? "preserve existing trace\n"
            : strcmp(fault, "existing-core-file") == 0 ? "preserve existing core trace\n"
                                                       : "preserve symlink target\n";
        if (!manual_file_equals(sentinel, expected_sentinel) ||
            lstat(sentinel, &sentinel_after) != 0 ||
            sentinel_after.st_dev != sentinel_before.st_dev ||
            sentinel_after.st_ino != sentinel_before.st_ino ||
            (sentinel_after.st_mode & S_IFMT) != (sentinel_before.st_mode & S_IFMT)) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: existing sentinel identity/content changed\n",
                    fault);
        }
        if (strcmp(fault, "symlink") == 0) {
            struct stat conflict_after;
            if (!conflict_before_valid || lstat(trace_conflict, &conflict_after) != 0 ||
                !S_ISLNK(conflict_after.st_mode) ||
                conflict_after.st_dev != conflict_before.st_dev ||
                conflict_after.st_ino != conflict_before.st_ino)
                path_ok = 0;
        }

        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        struct stat helper_info;
        int helper_exists = lstat(other, &helper_info) == 0;
        if (strcmp(fault, "existing-file") == 0) {
            if (!helper_exists || !manual_file_equals(other, "preserve existing trace\n") ||
                helper_info.st_dev != sentinel_before.st_dev ||
                helper_info.st_ino != sentinel_before.st_ino) {
                path_ok = 0;
                fprintf(stderr, "manual control %s: existing helper sentinel changed\n", fault);
            }
        } else if (helper_exists) {
            fprintf(stderr, "manual control %s: helper.trace remained after refused preflight\n",
                    fault);
            path_ok = 0;
        } else if (errno != ENOENT) {
            path_ok = 0;
        }

        if (strcmp(fault, "existing-file") == 0 || strcmp(fault, "existing-core-file") == 0) {
            if (unlink(sentinel) != 0) {
                path_ok = 0;
                fprintf(stderr, "manual control %s: test obstruction removal failed\n", fault);
            }
        } else {
            if (unlink(trace_conflict) != 0 || unlink(sentinel) != 0) {
                path_ok = 0;
                fprintf(stderr, "manual control %s: symlink fixture removal failed\n", fault);
            }
        }
        /* The harness removes a leaked test-owned partial trace only after asserting it existed. */
        if (strcmp(fault, "existing-file") != 0 && helper_exists && unlink(other) != 0)
            path_ok = 0;

        manual_result retry_result;
        int retry_rc = manual_launch(trace_path, app, wall_ms, grace_ms, 0, &retry_result);
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_trace retry_helper, retry_core;
        int retry_complete = retry_rc == 0 && retry_result.child > 0 &&
                             retry_result.outcome == MANUAL_EXITED &&
                             waitpid(retry_result.child, NULL, WNOHANG) == -1 && errno == ECHILD;
        if (retry_complete) {
            it_read(other, IT_HELPER, &retry_helper);
            retry_complete = retry_helper.status == IT_COMPLETE;
        }
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        if (retry_complete) {
            it_read(other, IT_CORE, &retry_core);
            retry_complete =
                retry_core.status == IT_COMPLETE && it_pair(&retry_helper, &retry_core);
        }
        if (!retry_complete) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: synthetic retry failed (rc=%d, outcome=%d)\n",
                    fault, retry_rc, retry_result.outcome);
        }
        printf("manual control %s retry: %s\n", fault,
               retry_complete ? "complete paired traces" : "failed");
        if (unlink(config) != 0) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: config cleanup failed\n", fault);
        }
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        if (access(other, F_OK) == 0 && unlink(other) != 0) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: helper trace cleanup failed\n", fault);
        }
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        if (access(other, F_OK) == 0 && unlink(other) != 0) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: core trace cleanup failed\n", fault);
        }
        if (unlink(cart) != 0 || rmdir(cart_dir) != 0) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: cart fixture cleanup failed\n", fault);
        }
        if ((strcmp(fault, "symlink") == 0 && rmdir(trace_path) != 0) || rmdir(dir_template) != 0) {
            path_ok = 0;
            fprintf(stderr, "manual control %s: trace fixture cleanup failed\n", fault);
        }
        unsetenv("MUHTARI_MANUAL_INPUT_TEST_CHILD");
        unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CONFIG");
        unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CORE");
        unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CONTENT");
        if (strcmp(fault, "existing-file") == 0)
            printf("manual control %s: existing trace sentinel preserved; partial trace absent\n",
                   fault);
        else
            printf("manual control %s: existing trace preserved; partial helper %s\n", fault,
                   path_ok ? "absent" : "check failed");
        if (!path_ok)
            return 0;
    } else if (rc == 0 && strcmp(fault, "valid") == 0) {
        it_trace helper, core;
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_HELPER, &helper);
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_CORE, &core);
        if (helper.status != IT_COMPLETE || core.status != IT_COMPLETE ||
            !it_pair(&helper, &core) || result->signals_sent != 0)
            return 0;
        if (trace_size > 0) {
            int written = snprintf(trace_dir, trace_size, "%s", dir_template);
            if (written < 0 || (size_t)written >= trace_size)
                return 0;
        }
    } else if (rc != 0 && strcmp(expected_outcome, "argv-rejected") == 0) {
        it_trace helper, core;
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_HELPER, &helper);
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_CORE, &core);
        if (result->outcome != MANUAL_EXITED || helper.status != IT_COMPLETE ||
            core.status != IT_EMPTY)
            return 0;
    } else if (strcmp(fault, "exec") == 0) {
        it_trace helper, core;
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_HELPER, &helper);
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_CORE, &core);
        if (rc != 127 || result->outcome != MANUAL_EXEC_FAILED || result->exec_error != ENOENT ||
            helper.status != IT_COMPLETE || core.status != IT_EMPTY)
            return 0;
    } else if (strcmp(fault, "timeout") == 0 || strcmp(fault, "unreaped") == 0 ||
               strcmp(fault, "interrupt") == 0) {
        it_trace helper, core;
        count = snprintf(other, sizeof other, "%s/helper.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_HELPER, &helper);
        count = snprintf(other, sizeof other, "%s/core.trace", trace_path);
        if (count < 0 || (size_t)count >= sizeof other)
            return 0;
        it_read(other, IT_CORE, &core);
        int cleanup_expected = strcmp(fault, "unreaped") != 0
                                   ? rc == 124 && result->outcome == MANUAL_TIMED_OUT
                                   : rc == 125 && result->outcome == MANUAL_UNREAPED;
        if (!cleanup_expected || result->signals_sent != 2 || helper.status != IT_COMPLETE ||
            core.status != IT_UNFINISHED || core.records[0].type != IT_INIT ||
            !it_pair(&helper, &core))
            return 0;
        if (strcmp(fault, "interrupt") == 0 &&
            (!WIFSIGNALED(result->status) || WTERMSIG(result->status) != SIGKILL))
            return 0;
    } else if (rc == 0 || strcmp(fault, "valid") == 0) {
        return 0;
    }
    if (!path_fault &&
        (result->child <= 0 || waitpid(result->child, NULL, WNOHANG) != -1 || errno != ECHILD))
        return 0;
    unsetenv("MUHTARI_MANUAL_INPUT_TEST_CHILD");
    unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CONFIG");
    unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CORE");
    unsetenv("MUHTARI_MANUAL_INPUT_EXPECT_CONTENT");
    if (!path_fault && (unlink(cart) != 0 || rmdir(cart_dir) != 0))
        return 0;
    if (strcmp(fault, "interrupt") == 0 && (manual_alarm_count == 0 || launch_end < launch_start ||
                                            launch_end - launch_start > wall_ms + grace_ms + 200u ||
                                            !manual_child_reaped(result->child)))
        return 0;
    printf("manual control %s: %s, child=%ld, signals=%d\n", fault, expected_outcome,
           (long)result->child, result->signals_sent);
    return 1;
}

static int manual_control(const char *work, char *app_path, char *core_path)
{
    struct sigaction alarm_action = {.sa_handler = manual_alarm_handler};
    sigemptyset(&alarm_action.sa_mask);
    if (sigaction(SIGALRM, &alarm_action, NULL) != 0) {
        fprintf(stderr, "manual control could not install EINTR test handler\n");
        return 1;
    }
    static const struct {
        const char *name, *expected;
        uint64_t wall_ms, grace_ms;
    } cases[] = {
        {"valid", "valid trace", 1000u, 100u},
        {"cap-omitted", "argv-rejected", 1000u, 100u},
        {"cap-altered", "argv-rejected", 1000u, 100u},
        {"core", "argv-rejected", 1000u, 100u},
        {"content", "argv-rejected", 1000u, 100u},
        {"config", "argv-rejected", 1000u, 100u},
        {"existing-file", "existing trace sentinel preserved", 1000u, 100u},
        {"existing-core-file", "existing core trace preserved and partial file removed", 1000u,
         100u},
        {"symlink", "trace symlink sentinel preserved", 1000u, 100u},
        {"exec", "exec error", 1000u, 100u},
        {"timeout", "exact-child SIGTERM/SIGKILL/reap", 150u, 150u},
        {"interrupt", "EINTR returns to bounded deadline and exact-child cleanup", 150u, 150u},
        {"unreaped", "bounded unreaped result; harness reaped exact child", 150u, 150u}};
    char valid_trace[4096] = {0};
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        manual_result result;
        if (!manual_test_case(work, app_path, core_path, cases[i].name, cases[i].wall_ms,
                              cases[i].grace_ms, &result, i == 0 ? valid_trace : NULL,
                              i == 0 ? sizeof valid_trace : 0, cases[i].expected)) {
            fprintf(stderr, "manual control %s failed its lifecycle/argv assertion\n",
                    cases[i].name);
            return 1;
        }
    }
    printf("manual_trace_dir=%s\n", valid_trace);
    return 0;
}
static void hash(uint64_t *h, uint64_t n)
{
    *h = (*h ^ n) * UINT64_C(1099511628211);
}
static bool environment(unsigned command, void *data)
{
    if (command == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT)
        return *(enum retro_pixel_format *)data == RETRO_PIXEL_FORMAT_XRGB8888;
    return false;
}
static void input_poll_callback(void)
{
    hash(&observed.calls, 1);
    observed.polls++;
}
static int16_t state(unsigned port, unsigned device, unsigned index, unsigned id)
{
    int16_t value = 0;
    if (scenario == 0 && port == 0 && (id == 4 || id == 0))
        value = frame < 2 ? 1 : frame < 4 ? 0 : -7;
    if (scenario == 1 && port == 1 && (id == 4 || id == 0))
        value = 1;
    if (scenario == 2 && port == 0 && id == RETRO_DEVICE_ID_JOYPAD_A)
        value = 1;
    hash(&observed.calls, 2);
    hash(&observed.calls, port);
    hash(&observed.calls, device);
    hash(&observed.calls, index);
    hash(&observed.calls, id);
    hash(&observed.calls, (uint16_t)value);
    observed.queries++;
    return value;
}
static void video(const void *pixels, unsigned width, unsigned height, size_t pitch)
{
    hash(&observed.calls, 3);
    hash(&observed.video, width);
    hash(&observed.video, height);
    for (unsigned y = 0; y < height; y++) {
        const uint8_t *row = (const uint8_t *)pixels + y * pitch;
        for (unsigned x = 0; x < width * 4u; x++)
            hash(&observed.video, row[x]);
    }
    observed.frames++;
}
static size_t audio(const int16_t *data, size_t count)
{
    hash(&observed.calls, 4);
    for (size_t i = 0; i < count * 2u; i++)
        hash(&observed.audio, (uint16_t)data[i]);
    observed.samples += count;
    return count;
}
static uint64_t nanoseconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        abort();
    return (uint64_t)now.tv_sec * UINT64_C(1000000000) + (uint64_t)now.tv_nsec;
}
static int load(void *module, api *a)
{
#define LOAD(name)                                                                                 \
    do {                                                                                           \
        void *symbol = dlsym(module, "retro_" #name);                                              \
        REQUIRE(symbol != NULL);                                                                   \
        memcpy(&a->name, &symbol, sizeof a->name);                                                 \
    } while (0)
    LOAD(set_environment);
    LOAD(set_input_poll);
    LOAD(set_input_state);
    LOAD(set_video_refresh);
    LOAD(set_audio_sample_batch);
    LOAD(set_controller_port_device);
    LOAD(init);
    LOAD(deinit);
    LOAD(run);
    LOAD(load_game);
#undef LOAD
    return 0;
}
static int session(const char *module_path, const char *trace_path, result *out, uint64_t *elapsed)
{
    int fd = open(trace_path, O_CREAT | O_EXCL | O_RDWR, 0600);
    REQUIRE(fd >= 3);
    int child_fd = dup(fd);
    REQUIRE(child_fd >= 3);
    char text[32];
    snprintf(text, sizeof text, "%d", child_fd);
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_FD", text, 1) == 0);
    void *module = dlopen(module_path, RTLD_NOW | RTLD_LOCAL);
    REQUIRE(module != NULL);
    api a;
    REQUIRE(load(module, &a) == 0);
    memset(&observed, 0, sizeof observed);
    a.set_environment(environment);
    a.set_input_poll(scenario == 3 || scenario == 7 ? NULL : input_poll_callback);
    a.set_input_state(scenario == 3 ? NULL : state);
    a.set_video_refresh(video);
    a.set_audio_sample_batch(audio);
    a.init();
    if (scenario != 4) {
        REQUIRE(a.load_game(NULL));
        if (scenario == 5)
            a.set_controller_port_device(0, RETRO_DEVICE_NONE);
    }
    uint64_t start = nanoseconds();
    for (frame = 0; frame < (scenario == 6 ? 0u : 6u); frame++) {
        /* First completed nonnull poll occurs after three already-run frames. */
        if (scenario == 7 && frame == 3)
            a.set_input_poll(input_poll_callback);
        a.run();
    }
    *elapsed = nanoseconds() - start;
    a.deinit();
    *out = observed;
    REQUIRE(dlclose(module) == 0);
    if (fcntl(child_fd, F_GETFD) >= 0)
        close(child_fd);
    close(fd);
    REQUIRE(unsetenv("MUHTARI_INPUT_TRACE_FD") == 0);
    return 0;
}
static int oracle(const it_trace *t)
{
    REQUIRE(t->status == IT_COMPLETE && t->clock_ok);
    unsigned polls = 0, states = 0, unavailable = 0, held = 0, released = 0, changed = 0;
    for (uint32_t i = 0; i < t->count; i++) {
        const it_record *r = &t->records[i];
        if (r->type == IT_POLL) {
            REQUIRE(r->a == (scenario == 3 || (scenario == 7 && polls == 0) ? 0u : 1u));
            polls++;
        }
        if (r->type == IT_STATE) {
            states++;
            REQUIRE(r->c == 0 && (r->a == 4 || r->a == 0));
            if (r->b == 1)
                held++;
            else if (r->b == 0)
                released++;
            else if (r->b == (uint16_t)-7)
                changed++;
            else
                REQUIRE(0);
        }
        if (r->type == IT_UNAVAILABLE) {
            REQUIRE(r->a == (scenario == 3 ? 2u : scenario == 4 ? 1u : 3u));
            unavailable++;
        }
    }
    REQUIRE(polls == (scenario == 6 ? 0u : scenario == 7 ? 2u : 1u));
    REQUIRE(unavailable == (scenario >= 3 && scenario <= 5 ? 1u : 0u));
    REQUIRE(states == (scenario == 0                                     ? 6u
                       : scenario == 1 || scenario == 2 || scenario == 7 ? 2u
                                                                         : 0u));
    REQUIRE(held == (scenario == 0 ? 2u : 0u) && changed == held);
    REQUIRE(released == (states ? 2u : 0u));
    return 0;
}
int main(int argc, char **argv)
{
    const char *manual_test_child = getenv("MUHTARI_MANUAL_INPUT_TEST_CHILD");
    if (manual_test_child != NULL &&
        (strcmp(manual_test_child, "hang") == 0 || strcmp(manual_test_child, "interrupt") == 0) &&
        argc > 1 && strcmp(argv[1], "--manual-test-hang") == 0)
        return manual_hang_app();
    if (manual_test_child != NULL && strcmp(manual_test_child, "1") == 0 && argc > 1 &&
        strcmp(argv[1], "--verbose") == 0)
        return manual_fake_app(argc, argv);
    if (argc == 4 && strcmp(argv[1], "--manual-control") == 0)
        return manual_control(argv[2], argv[0], argv[3]);
    if (argc >= 5 && strcmp(argv[1], "--manual-launch") == 0 && strcmp(argv[3], "--") == 0) {
        manual_result launch;
        int rc = manual_launch(argv[2], &argv[4], MANUAL_WALL_MS, MANUAL_GRACE_MS, 0, &launch);
        printf("manual launch outcome=%d child=%ld status=%d exec_errno=%d signals=%d\n",
               launch.outcome, (long)launch.child, launch.status, launch.exec_error,
               launch.signals_sent);
        return rc;
    }
    REQUIRE(argc == 4);
    uint64_t normal_ns = 0, trace_ns = 0;
    for (scenario = 0; scenario < 8; scenario++) {
        char normal_path[4096], trace_path[4096];
        snprintf(normal_path, sizeof normal_path, "%s/normal-%u.trace", argv[3], scenario);
        snprintf(trace_path, sizeof trace_path, "%s/module-%u.trace", argv[3], scenario);
        unlink(normal_path);
        unlink(trace_path);
        result normal, diagnostic;
        uint64_t elapsed;
        REQUIRE(session(argv[1], normal_path, &normal, &elapsed) == 0);
        normal_ns += elapsed;
        REQUIRE(session(argv[2], trace_path, &diagnostic, &elapsed) == 0);
        trace_ns += elapsed;
        REQUIRE(memcmp(&normal, &diagnostic, sizeof normal) == 0);
        REQUIRE(normal.polls == (scenario == 3 || scenario == 6 ? 0u : scenario == 7 ? 3u : 6u));
        REQUIRE(normal.frames == (scenario == 4 || scenario == 6 ? 0u : 6u));
        if (normal.frames)
            REQUIRE(normal.samples > 0);
        it_trace t;
        it_read(normal_path, IT_CORE, &t);
        REQUIRE(t.status == IT_EMPTY);
        it_read(trace_path, IT_CORE, &t);
        REQUIRE(oracle(&t) == 0);
        unlink(normal_path);
        unlink(trace_path);
    }
    printf("same callbacks/order/return-values/frame/audio; normal_ns=%llu instrumented_ns=%llu "
           "delta_ns=%lld (local elapsed only)\n",
           (unsigned long long)normal_ns, (unsigned long long)trace_ns,
           (long long)trace_ns - (long long)normal_ns);
    return 0;
}
