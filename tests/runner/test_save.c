/* Actual runner consumer and generated, owned cartridge; runtime bytes stay in build/. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "../../runner/clock.h"
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "muhtari.h"
#include "internal.h"
#include "../check.h"

enum {
    IO_OK,
    IMPORT_READ,
    IMPORT_CLOSE,
    SAVE_CREATE,
    SAVE_FDOPEN,
    SAVE_PARTIAL,
    SAVE_WRITE,
    SAVE_FLUSH,
    SAVE_SYNC,
    SAVE_CLOSE,
    SAVE_REPLACE,
    STATE_READ,
    STATE_CLOSE,
    TRACE_ALLOC
};
static int io_fault;
static int output_fault;
static int track_runner_allocations;
static size_t largest_runner_allocation;
static int time_trace_write;
static uint64_t trace_write_started_ns, trace_write_finished_ns;
static unsigned frames_run, temp_opens, replacements;
static unsigned create_calls;
static unsigned trace_error_reports, trace_alloc_reports, trace_stderr_reports;
static unsigned trace_missing_reports, trace_detach_checks;
static int assert_trace_detached;
static unsigned change_at, request_at, stop_at, request_during_write;
static int change_every;
static unsigned replaced_at[128];
static void request_flush(void);
static void request_stop(void);
#ifdef _WIN32
static DWORD WINAPI concurrent_flush(LPVOID unused)
{
    (void)unused;
    request_flush();
    return 0;
}
#endif
static int reject_frame;
static unsigned reject_at;
static muhtari *trace_instance;
static FILE *import_stream, *save_stream;
static int state_fd = -1;
static FILE *state_stream;
static size_t state_read_requested;
static unsigned state_probe_reads;
static const uint8_t *expected_import;
static size_t expected_bytes;
static const uint8_t *expected_firmware;
static unsigned notice_stub, notice_unknown, notice_signature, notice_sha1;
static int test_print(FILE *f, const char *format, ...)
{
    if (strstr(format, "cannot serialize or commit trace file") != NULL) {
        trace_error_reports++;
        if (f == stderr)
            trace_stderr_reports++;
    }
    if (strstr(format, "out of memory for trace capture") != NULL) {
        trace_alloc_reports++;
        if (f == stderr)
            trace_stderr_reports++;
    }
    if (strstr(format, "selected trace frame did not complete") != NULL) {
        trace_missing_reports++;
        if (f == stderr)
            trace_stderr_reports++;
    }
    if (strstr(format, "signature-only stub") != NULL)
        notice_stub++;
    if (strstr(format, "unknown HSC firmware") != NULL)
        notice_unknown++;
    if (strstr(format, "lacks C6/FE") != NULL)
        notice_signature++;
    if (strstr(format, "hsc_sha1") != NULL)
        notice_sha1++;
    va_list args;
    va_start(args, format);
    int rc = vfprintf(f, format, args);
    va_end(args);
    return rc;
}
static void *test_malloc(size_t size)
{
    if (track_runner_allocations && size > largest_runner_allocation)
        largest_runner_allocation = size;
    if (io_fault == TRACE_ALLOC &&
        size == 65536u * sizeof(struct muhtari__trace_event)) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(size);
}

static FILE *test_fopen(const char *path, const char *mode)
{
    FILE *f = fopen(path, mode);
    if (strstr(path, ".sav") != NULL && strcmp(mode, "rb") == 0)
        import_stream = f;
    return f;
}
static FILE *test_fdopen(int fd, const char *mode)
{
    int is_state = strcmp(mode, "rb") == 0 && fd == state_fd;
    if (strcmp(mode, "rb") != 0 && io_fault == SAVE_FDOPEN)
        return NULL;
#ifdef _WIN32
    FILE *f = _fdopen(fd, mode);
#else
    FILE *f = fdopen(fd, mode);
#endif
    if (is_state) {
        state_fd = -1;
        state_stream = f;
    } else if (strcmp(mode, "rb") == 0)
        import_stream = f;
    else {
        save_stream = f;
        temp_opens++;
    }
    return f;
}
static size_t test_read(void *p, size_t n, size_t count, FILE *f)
{
    if (f == state_stream) {
        state_read_requested += n * count;
        if (n == 1u && count == 1u)
            state_probe_reads++;
        if (io_fault == STATE_READ)
            return 0u;
    }
    return f == import_stream && io_fault == IMPORT_READ ? 0u : fread(p, n, count, f);
}
static int test_error(FILE *f)
{
    if (f == state_stream && io_fault == STATE_READ)
        return 1;
    return f == import_stream && io_fault == IMPORT_READ ? 1 : ferror(f);
}
static size_t test_write(const void *p, size_t n, size_t count, FILE *f)
{
    if (f == save_stream && request_during_write) {
        request_during_write = 0;
#ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, concurrent_flush, NULL, 0, NULL);
        CHECK(thread != NULL);
        if (thread != NULL) {
            CHECK_EQ_U64(WaitForSingleObject(thread, 1000), WAIT_OBJECT_0);
            CloseHandle(thread);
        }
#else
        request_flush();
#endif
    }
    if (f == save_stream && time_trace_write) {
        if (trace_write_started_ns == 0u)
            trace_write_started_ns = muhtari__clock_ns();
        size_t written = fwrite(p, n, count, f);
        if (strncmp((const char *)p, "{\"status\":", 10u) == 0)
            trace_write_finished_ns = muhtari__clock_ns();
        return written;
    }
    if (f == save_stream && io_fault == SAVE_PARTIAL)
        return fwrite(p, n, count - 1u, f);
    return f == save_stream && io_fault == SAVE_WRITE ? 0u : fwrite(p, n, count, f);
}
static int test_open(const char *path, int flags, ...)
{
    int mode = 0;
#ifdef _WIN32
    int creating = flags & _O_CREAT;
#else
    int creating = flags & O_CREAT;
#endif
    if (creating) {
        create_calls++;
        if (io_fault == SAVE_CREATE) {
            errno = EACCES;
            return -1;
        }
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
    }
#ifdef _WIN32
    int fd = _open(path, flags, mode);
#else
    int fd = open(path, flags, (mode_t)mode);
#endif
    if (fd >= 0 && !creating && strstr(path, ".state") != NULL)
        state_fd = fd;
    return fd;
}
static int test_flush(FILE *f)
{
    if (f == stdout && output_fault)
        return EOF;
    return f == save_stream && io_fault == SAVE_FLUSH ? EOF : fflush(f);
}
static int test_close(FILE *f)
{
    int fail = (f == import_stream && io_fault == IMPORT_CLOSE) ||
               (f == save_stream && io_fault == SAVE_CLOSE) ||
               (f == state_stream && io_fault == STATE_CLOSE);
    if (f == import_stream)
        import_stream = NULL;
    if (f == save_stream)
        save_stream = NULL;
    if (f == state_stream)
        state_stream = NULL;
    int rc = fclose(f);
    return fail ? EOF : rc;
}
static int test_sync(int fd)
{
    if (io_fault == SAVE_SYNC)
        return -1;
#ifdef _WIN32
    return _commit(fd);
#else
    return fsync(fd);
#endif
}
#ifndef _WIN32
static int test_rename(const char *from, const char *to)
{
    replacements++;
    if (replacements <= 128u)
        replaced_at[replacements - 1u] = frames_run;
    return io_fault == SAVE_REPLACE ? -1 : rename(from, to);
}
#else
static BOOL test_move(LPCSTR from, LPCSTR to, DWORD flags)
{
    replacements++;
    if (replacements <= 128u)
        replaced_at[replacements - 1u] = frames_run;
    return io_fault == SAVE_REPLACE ? FALSE : MoveFileExA(from, to, flags);
}
#endif
static muhtari_status test_frame(muhtari *m, muhtari_frame *io)
{
    if (assert_trace_detached)
        trace_instance = m;
    if (expected_firmware != NULL && frames_run == 0u) {
        muhtari_const_span span = {0};
        span.size = (uint32_t)sizeof span;
        CHECK_EQ_U64(muhtari_get_hsc_rom(m, &span), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(span.bytes, 4096);
        CHECK(memcmp(span.data, expected_firmware, 4096) == 0);
    }
    if (expected_import != NULL && frames_run == 0u) {
        muhtari_battery_info b = {0};
        b.size = (uint32_t)sizeof b;
        CHECK_EQ_U64(muhtari_get_battery(m, &b), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(b.bytes, expected_bytes);
        if (expected_bytes != 0u)
            CHECK(memcmp(b.data, expected_import, expected_bytes) == 0);
        CHECK_EQ_U64(b.generation, 0);
    }
    frames_run++;
    muhtari_status st = muhtari_run_frame(m, io);
    if (change_every || frames_run == change_at)
        muhtari__bus_write(m, 0x1000u, (uint8_t)frames_run);
    if (frames_run == request_at) {
        request_flush();
        request_flush();
    }
    if (frames_run == stop_at)
        request_stop();
    return reject_frame || frames_run == reject_at ? MUHTARI_STATUS_ERR_BIOS_REJECTED_CART : st;
}
int runner_entry(int argc, char **argv);
void test_destroy(muhtari *m);
#define main runner_entry
#define malloc test_malloc
#define fprintf test_print
#define fopen test_fopen
#define open test_open
#define _open test_open
#define fdopen test_fdopen
#define _fdopen test_fdopen
#define fread test_read
#define ferror test_error
#define fwrite test_write
#define fflush test_flush
#define fclose test_close
#define fsync test_sync
#define _commit test_sync
#define rename test_rename
#define MoveFileExA test_move
#define muhtari_run_frame test_frame
#define muhtari_destroy test_destroy
#include "../../runner/main.c"
#undef muhtari_destroy
#undef muhtari_run_frame
#undef rename
#undef MoveFileExA
#undef _commit
#undef fsync
#undef fclose
#undef fflush
#undef fwrite
#undef ferror
#undef fread
#undef _fdopen
#undef fdopen
#undef fopen
#undef open
#undef _open
#undef main
#undef malloc
#undef fprintf
#include "../../runner/stop.c"

void test_destroy(muhtari *m)
{
    if (assert_trace_detached) {
        CHECK(m->trace_callback == NULL && m->trace_user == NULL);
        trace_detach_checks++;
    }
    muhtari_destroy(m);
}

static void request_flush(void)
{
#ifdef _WIN32
    CHECK(on_console(CTRL_BREAK_EVENT));
#elif defined(SIGUSR1)
    on_signal(SIGUSR1);
#else
    flush_flag = 1;
#endif
}
static void request_stop(void)
{
#ifdef _WIN32
    CHECK(on_console(CTRL_C_EVENT));
#else
    on_signal(SIGINT);
#endif
}

static void write_bytes(const char *path, const void *p, size_t n)
{
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(p, 1, n, f), n);
        CHECK_EQ_U64(fclose(f), 0);
    }
}

static void cartridge(const char *path, uint8_t devices, int increment)
{
    uint8_t bytes[128 + 4096];
    memset(bytes, 0, sizeof bytes);
    bytes[0] = 4;
    memcpy(bytes + 1, "ATARI7800", 9);
    bytes[50] = 0x10;
    bytes[55] = bytes[56] = 1;
    bytes[58] = devices;
    memcpy(bytes + 100, "ACTUAL CART DATA STARTS HERE", 28);
    memset(bytes + 128, 0xEA, 4096);
    /* Increment imported SRAM once, then loop for the rest of the frame. */
    static const uint8_t code[] = {0xEE, 0x00, 0x10, 0x4C, 0x03, 0xF0};
    memcpy(bytes + 128, code, sizeof code);
    if (!increment)
        memcpy(bytes + 128, "\x4C\x00\xF0", 3);
    bytes[128 + 0xFFC] = 0;
    bytes[128 + 0xFFD] = 0xF0;
    write_bytes(path, bytes, sizeof bytes);
}

static void exact_file(const char *path, const uint8_t *want, size_t bytes)
{
    uint8_t *got = malloc(bytes + 1u);
    CHECK(got != NULL);
    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    if (f != NULL && got != NULL) {
        CHECK_EQ_U64(fread(got, 1, bytes + 1u, f), bytes);
        CHECK(memcmp(want, got, bytes) == 0);
    }
    if (f != NULL)
        CHECK_EQ_U64(fclose(f), 0);
    free(got);
}

static int trace_file_exists(const char *path)
{
#ifdef _WIN32
    struct _stat64 info;
    return _stat64(path, &info) == 0;
#else
    struct stat info;
    return stat(path, &info) == 0;
#endif
}

static int trace_snapshot(const char *path, uint8_t *bytes, size_t capacity, size_t *length)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return -1;
    size_t n = fread(bytes, 1u, capacity, f);
    int extra = fgetc(f);
    int failed = ferror(f);
    int close_failed = fclose(f);
    if (failed || close_failed != 0 || (n == capacity && extra != EOF))
        return -1;
    *length = n;
    return 0;
}

static unsigned long trace_test_pid(void)
{
#ifdef _WIN32
    return (unsigned long)GetCurrentProcessId();
#elif defined(__wasi__)
    return 0UL;
#else
    return (unsigned long)getpid();
#endif
}

static void trace_temp_path(char *path, size_t capacity, const char *destination, unsigned attempt)
{
    int n = snprintf(path, capacity, "%s.tmp.%lu.%u", destination, trace_test_pid(), attempt);
    CHECK(n > 0 && (size_t)n < capacity);
}

static void trace_check_temps(const char *destination, int preserved_attempt)
{
    char path[1200];
    for (unsigned attempt = 0u; attempt < 32u; attempt++) {
        trace_temp_path(path, sizeof path, destination, attempt);
        CHECK_EQ_U64(trace_file_exists(path), attempt == (unsigned)preserved_attempt);
    }
}

static void trace_check_bytes(const char *path, const uint8_t *expected, size_t expected_n)
{
    uint8_t actual[8192];
    size_t actual_n = 0u;
    CHECK(expected_n <= sizeof actual);
    CHECK_EQ_U64(trace_snapshot(path, actual, sizeof actual, &actual_n), 0u);
    CHECK_EQ_U64(actual_n, expected_n);
    if (actual_n == expected_n)
        CHECK(memcmp(actual, expected, expected_n) == 0);
}

static void trace_reset_io(void)
{
    io_fault = IO_OK;
    output_fault = 0;
    track_runner_allocations = 0;
    largest_runner_allocation = 0u;
    time_trace_write = 0;
    trace_write_started_ns = trace_write_finished_ns = 0u;
    frames_run = temp_opens = replacements = create_calls = 0u;
    trace_error_reports = trace_alloc_reports = trace_stderr_reports = 0u;
    trace_missing_reports = trace_detach_checks = 0u;
    assert_trace_detached = 1;
    change_at = request_at = stop_at = request_during_write = 0u;
    change_every = 0;
    reject_frame = 0;
    reject_at = 0u;
    trace_instance = NULL;
    import_stream = save_stream = NULL;
    state_fd = -1;
    state_stream = NULL;
    state_read_requested = 0u;
    state_probe_reads = 0u;
    expected_import = NULL;
    expected_bytes = 0u;
    expected_firmware = NULL;
    stop_flag = flush_flag = 0;
}

static int trace_run_cli(char *rom, char *trace, const char *frames,
                         const char *frame, const char *limit)
{
    char *args[12];
    char frames_copy[16], frame_copy[16], limit_copy[16];
    int frames_n = snprintf(frames_copy, sizeof frames_copy, "%s", frames);
    int frame_n = snprintf(frame_copy, sizeof frame_copy, "%s", frame);
    int limit_n = limit == NULL ? 0 : snprintf(limit_copy, sizeof limit_copy, "%s", limit);
    CHECK(frames_n > 0 && (size_t)frames_n < sizeof frames_copy);
    CHECK(frame_n > 0 && (size_t)frame_n < sizeof frame_copy);
    CHECK(limit == NULL || (limit_n > 0 && (size_t)limit_n < sizeof limit_copy));
    if (frames_n <= 0 || (size_t)frames_n >= sizeof frames_copy || frame_n <= 0 ||
        (size_t)frame_n >= sizeof frame_copy ||
        (limit != NULL && (limit_n <= 0 || (size_t)limit_n >= sizeof limit_copy)))
        return 2;
    int count = 0;
    args[count++] = "muhtari-run";
    args[count++] = "--frames";
    args[count++] = frames_copy;
    args[count++] = "--trace";
    args[count++] = trace;
    args[count++] = "--trace-frame";
    args[count++] = frame_copy;
    args[count++] = "--trace-range";
    args[count++] = "0:65535";
    if (limit != NULL) {
        args[count++] = "--trace-limit";
        args[count++] = limit_copy;
    }
    args[count++] = rom;
    CHECK(count <= (int)(sizeof args / sizeof args[0]));
    return runner_entry(count, args);
}

static void trace_io(const char *dir)
{
    static const uint8_t sentinel[] = "previous complete capture";
    static const int faults[] = {TRACE_ALLOC, SAVE_CREATE, SAVE_FDOPEN, SAVE_PARTIAL,
                                 SAVE_WRITE, SAVE_FLUSH, SAVE_SYNC, SAVE_CLOSE, SAVE_REPLACE};
    char rom[1024], destination[1024], candidate[1200];
    (void)snprintf(rom, sizeof rom, "%s/trace-io.a78", dir);
    (void)snprintf(destination, sizeof destination, "%s/trace-io.jsonl", dir);
    cartridge(rom, 0u, 0);

    for (size_t i = 0u; i < sizeof faults / sizeof faults[0]; i++) {
        write_bytes(destination, sentinel, sizeof sentinel);
        trace_reset_io();
        io_fault = faults[i];
        unsigned status = (unsigned)trace_run_cli(rom, destination, "1", "1", NULL);
        CHECK_EQ_U64(status, 1u);
        CHECK_EQ_U64(frames_run, faults[i] == TRACE_ALLOC ? 0u : 1u);
        CHECK_EQ_U64(create_calls, faults[i] == TRACE_ALLOC ? 0u : 1u);
        CHECK_EQ_U64(temp_opens, faults[i] == TRACE_ALLOC || faults[i] == SAVE_CREATE ||
                                     faults[i] == SAVE_FDOPEN
                                 ? 0u
                                 : 1u);
        CHECK_EQ_U64(replacements, faults[i] == SAVE_REPLACE ? 1u : 0u);
        CHECK_EQ_U64(trace_alloc_reports, faults[i] == TRACE_ALLOC ? 1u : 0u);
        CHECK_EQ_U64(trace_error_reports, faults[i] == TRACE_ALLOC ? 0u : 1u);
        CHECK_EQ_U64(trace_stderr_reports, 1u);
        CHECK_EQ_U64(trace_detach_checks, faults[i] == TRACE_ALLOC ? 0u : 1u);
        CHECK(faults[i] != TRACE_ALLOC || trace_instance == NULL);
        trace_check_bytes(destination, sentinel, sizeof sentinel);
        trace_check_temps(destination, -1);
    }

    /* The first sibling candidate is not ours when exclusive creation finds it. */
    write_bytes(destination, sentinel, sizeof sentinel);
    trace_temp_path(candidate, sizeof candidate, destination, 0u);
    static const uint8_t foreign[] = "unowned temporary";
    write_bytes(candidate, foreign, sizeof foreign);
    trace_reset_io();
    io_fault = SAVE_WRITE;
    CHECK_EQ_U64(trace_run_cli(rom, destination, "1", "1", NULL), 1u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(create_calls, 2u);
    CHECK_EQ_U64(temp_opens, 1u);
    CHECK_EQ_U64(replacements, 0u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK_EQ_U64(trace_missing_reports, 0u);
    trace_check_bytes(destination, sentinel, sizeof sentinel);
    trace_check_bytes(candidate, foreign, sizeof foreign);
    trace_check_temps(destination, 0);
    CHECK(remove(candidate) == 0);

    write_bytes(destination, sentinel, sizeof sentinel);
    trace_reset_io();
    CHECK_EQ_U64(trace_run_cli(rom, destination, "1", "1", NULL), 0u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(replacements, 1u);
    CHECK_EQ_U64(trace_error_reports, 0u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK(trace_file_exists(destination));
    trace_check_temps(destination, -1);

    (void)snprintf(destination, sizeof destination, "%s/trace-io-truncated.jsonl", dir);
    write_bytes(destination, sentinel, sizeof sentinel);
    trace_reset_io();
    CHECK_EQ_U64(trace_run_cli(rom, destination, "1", "1", "1"), 0u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(replacements, 1u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK(trace_file_exists(destination));
    trace_check_temps(destination, -1);

    (void)snprintf(destination, sizeof destination, "%s/trace-io-refused.jsonl", dir);
    write_bytes(destination, sentinel, sizeof sentinel);
    trace_reset_io();
    reject_at = 1u;
    CHECK_EQ_U64(trace_run_cli(rom, destination, "1", "1", NULL), 77u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(replacements, 0u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK_EQ_U64(trace_missing_reports, 0u);
    trace_check_bytes(destination, sentinel, sizeof sentinel);
    trace_check_temps(destination, -1);

    (void)snprintf(destination, sizeof destination, "%s/trace-io-stopped.jsonl", dir);
    write_bytes(destination, sentinel, sizeof sentinel);
    trace_reset_io();
    stop_at = 1u;
    CHECK_EQ_U64(trace_run_cli(rom, destination, "2", "2", NULL), 1u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(replacements, 0u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK_EQ_U64(trace_missing_reports, 1u);
    CHECK_EQ_U64(trace_stderr_reports, 1u);
    trace_check_bytes(destination, sentinel, sizeof sentinel);
    trace_check_temps(destination, -1);

    /* Once frame two has a truthful footer, a later unrelated refusal cannot revoke it. */
    (void)snprintf(destination, sizeof destination, "%s/trace-io-later-failure.jsonl", dir);
    write_bytes(destination, sentinel, sizeof sentinel);
    trace_reset_io();
    reject_at = 3u;
    CHECK_EQ_U64(trace_run_cli(rom, destination, "3", "2", NULL), 77u);
    CHECK_EQ_U64(frames_run, 3u);
    CHECK_EQ_U64(replacements, 1u);
    CHECK_EQ_U64(trace_detach_checks, 1u);
    CHECK_EQ_U64(trace_missing_reports, 0u);
    CHECK(trace_file_exists(destination));
    trace_check_temps(destination, -1);
}

static void trace_expect_usage(int argc, char **argv, const char *protected_path)
{
    uint8_t before[8192], after[8192];
    size_t before_n = 0u, after_n = 0u;
    int existed = protected_path != NULL && trace_file_exists(protected_path);
    if (existed)
        CHECK_EQ_U64(trace_snapshot(protected_path, before, sizeof before, &before_n), 0u);
    frames_run = 0u;
    CHECK_EQ_U64(runner_entry(argc, argv), 2u);
    CHECK_EQ_U64(frames_run, 0u);
    if (protected_path != NULL) {
        CHECK_EQ_U64(trace_file_exists(protected_path), existed);
        if (existed) {
            CHECK_EQ_U64(trace_snapshot(protected_path, after, sizeof after, &after_n), 0u);
            CHECK_EQ_U64(after_n, before_n);
            if (after_n == before_n)
                CHECK(memcmp(before, after, before_n) == 0);
        }
    }
}

static void trace_arguments(const char *dir)
{
    char rom[1024], trace[1024], state[1024], movie[1024];
    (void)snprintf(rom, sizeof rom, "%s/trace-arguments.a78", dir);
    (void)snprintf(trace, sizeof trace, "%s/trace-arguments.jsonl", dir);
    (void)snprintf(state, sizeof state, "%s/trace-arguments.state", dir);
    (void)snprintf(movie, sizeof movie, "%s/trace-arguments.movie", dir);
    cartridge(rom, 0u, 0);
    static const char movie_text[] = "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n";
    write_bytes(movie, movie_text, sizeof movie_text - 1u);
    static const uint8_t sentinel[] = "prior trace bytes";
    write_bytes(trace, sentinel, sizeof sentinel);

#define TRACE_BAD(...)                                                                             \
    do {                                                                                           \
        char *bad_argv[] = {"muhtari-run", __VA_ARGS__};                                           \
        trace_expect_usage((int)(sizeof bad_argv / sizeof bad_argv[0]), bad_argv, trace);           \
    } while (0)
    TRACE_BAD("--frames", "1", "--trace-frame", "1", "--trace-range", "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-range", "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace", trace, rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-frame", "1",
              "--trace-range", "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-range", "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "2", "--trace-limit", "3", rom);
    TRACE_BAD("--frames", "1", "--trace", "", "--trace-frame", "1", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", "-", "--trace-frame", "1", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "0", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "+1", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "-1", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1x", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "4294967296", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "2", "--trace-range",
              "0:65535", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              ":1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "1:", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "1:2:3", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "+1:2", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "-1:2", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0x:2", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "1x:2", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65536", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0x10000:0xFFFF", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "2:1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "0", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "+1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "-1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "1x", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "131073", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit", "4294967296", rom);
    TRACE_BAD("--frames", "1", "--trace-limit", "1", rom);
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--trace-limit");
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535", "--probe", rom);
    TRACE_BAD("--frames", "1", "--trace-frame", "1", "--trace-range", "0:65535", "--trace");
    TRACE_BAD("--frames", "1", "--trace", trace, "--trace-frame", "1", "--trace-range",
              "0:65535");
    {
        char *bad_argv[] = {"muhtari-run", "--frames0", "--trace", trace, "--trace-frame", "1",
                            "--trace-range", "0:65535", "--load-state", state, rom};
        trace_expect_usage((int)(sizeof bad_argv / sizeof bad_argv[0]), bad_argv, trace);
    }
    {
        char *bad_argv[] = {"muhtari-run", "--movie", movie, "--trace", trace, "--trace-frame",
                            "2", "--trace-range", "0:65535", rom};
        trace_expect_usage((int)(sizeof bad_argv / sizeof bad_argv[0]), bad_argv, trace);
    }
    TRACE_BAD("--trace", trace, "--trace-frame", "1", "--trace-range", "0:65535", rom);
#undef TRACE_BAD
}

static void trace_paths(const char *dir)
{
    char rom[1024], trace[1024], movie[1024], bios[1024], firmware[1024], state[1024];
    char subdir[1024], path[1024], alias[1024], save_dir[1024], state_point[1100];
    (void)snprintf(rom, sizeof rom, "%s/option.a78", dir);
    (void)snprintf(movie, sizeof movie, "%s/option.movie", dir);
    (void)snprintf(bios, sizeof bios, "%s/option.bios", dir);
    (void)snprintf(firmware, sizeof firmware, "%s/option-firmware.bin", dir);
    (void)snprintf(state, sizeof state, "%s/option.state", dir);
    (void)snprintf(save_dir, sizeof save_dir, "%s", dir);
    (void)snprintf(subdir, sizeof subdir, "%s/subdir", dir);
#ifdef _WIN32
    (void)_mkdir(subdir);
#else
    (void)mkdir(subdir, 0700);
#endif
    /* The script-mode test can be rerun in the same build directory. Remove
       only names this matrix owns so each absent-destination case starts new. */
    (void)snprintf(path, sizeof path, "%s/state-output", dir);
    (void)remove(path);
    (void)snprintf(path, sizeof path, "%s/frame-output.png", dir);
    (void)remove(path);
    (void)snprintf(path, sizeof path, "%s/normalized-output", dir);
    (void)remove(path);
    (void)snprintf(path, sizeof path, "%s/option.sav", dir);
    (void)remove(path);
    (void)snprintf(path, sizeof path, "%s/option-hardlink", dir);
    (void)remove(path);
    (void)snprintf(path, sizeof path, "%s/option-symlink", dir);
    (void)remove(path);
    cartridge(rom, 1u, 0);
    static const char movie_text[] = "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n";
    write_bytes(movie, movie_text, sizeof movie_text - 1u);
    uint8_t image[4096];
    memset(image, 0x64, sizeof image);
    write_bytes(bios, image, sizeof image);
    write_bytes(firmware, image, sizeof image);
    write_bytes(state, image, 128u);

#define TRACE_BAD_PATH(protected_path, ...)                                                       \
    do {                                                                                           \
        char *bad_argv[] = {"muhtari-run", __VA_ARGS__};                                           \
        trace_expect_usage((int)(sizeof bad_argv / sizeof bad_argv[0]), bad_argv, protected_path);  \
    } while (0)
    TRACE_BAD_PATH(rom, "--frames", "1", "--trace", rom, "--trace-frame", "1", "--trace-range",
                   "0:65535", rom);
    TRACE_BAD_PATH(movie, "--frames", "1", "--trace", movie, "--trace-frame", "1",
                   "--trace-range", "0:65535", "--movie", movie, rom);
    TRACE_BAD_PATH(bios, "--frames", "1", "--boot", "bios", "--bios", bios, "--trace", bios,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);
    TRACE_BAD_PATH(firmware, "--frames", "1", "--hsc", "on", "--hsc-rom", firmware,
                   "--trace", firmware, "--trace-frame", "1", "--trace-range", "0:65535", rom);
    TRACE_BAD_PATH(state, "--frames", "1", "--load-state", state, "--trace", state,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);

    (void)snprintf(path, sizeof path, "%s/state-output", dir);
    (void)snprintf(state_point, sizeof state_point, "1:%s", path);
    TRACE_BAD_PATH(path, "--frames", "1", "--state-at-frame", state_point, "--trace", path,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);
    (void)snprintf(path, sizeof path, "%s/frame-output.png", dir);
    (void)snprintf(state_point, sizeof state_point, "1:%s", path);
    TRACE_BAD_PATH(path, "--frames", "1", "--dump-frame", state_point, "--trace", path,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);

    (void)snprintf(trace, sizeof trace, "%s/./option.sav", dir);
    TRACE_BAD_PATH(trace, "--frames", "1", "--save-dir", save_dir, "--trace", trace,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);

    (void)snprintf(path, sizeof path, "%s/normalized-output", dir);
    (void)snprintf(alias, sizeof alias, "%s/subdir/../normalized-output", dir);
    (void)snprintf(state_point, sizeof state_point, "1:%s", path);
    TRACE_BAD_PATH(alias, "--frames", "1", "--dump-frame", state_point, "--trace", alias,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);
    (void)snprintf(save_dir, sizeof save_dir, "%s/subdir/..", dir);
    (void)snprintf(alias, sizeof alias, "%s/subdir/../option.sav", dir);
    TRACE_BAD_PATH(alias, "--frames", "1", "--save-dir", save_dir, "--trace", alias,
                   "--trace-frame", "1", "--trace-range", "0:65535", rom);

#if !defined(_WIN32) && !defined(__wasi__)
    (void)snprintf(alias, sizeof alias, "%s/option-hardlink", dir);
    if (link(rom, alias) == 0) {
        TRACE_BAD_PATH(alias, "--frames", "1", "--trace", alias, "--trace-frame", "1",
                       "--trace-range", "0:65535", rom);
        (void)remove(alias);
    } else {
        CHECK(0);
    }
    (void)snprintf(alias, sizeof alias, "%s/option-symlink", dir);
    if (symlink(rom, alias) == 0) {
        TRACE_BAD_PATH(alias, "--frames", "1", "--trace", alias, "--trace-frame", "1",
                       "--trace-range", "0:65535", rom);
        (void)remove(alias);
    }
#endif
    (void)snprintf(trace, sizeof trace, "%s/subdir/../option.a78", dir);
    TRACE_BAD_PATH(trace, "--frames", "1", "--trace", trace, "--trace-frame", "1",
                   "--trace-range", "0:65535", rom);
#undef TRACE_BAD_PATH
}

static void expansion_images(const char *dir)
{
    char path[1024];
    /* Static: the wasm32 lane's 64 KiB stack is smaller than the 104 KiB image. */
    static uint8_t image[128u + 106496u];
    memset(image, 0, sizeof image);
    image[0] = 4u;
    memcpy(image + 1u, "ATARI7800", 9u);
    image[51] = 0x40u;
    image[55] = image[56] = 1u;
    memset(image + 128u, 0x02, 16384u);
    static const uint8_t program[] = {0xA9, 0x08, 0x8D, 0x70, 0x04, 0xEE, 0x00, 0x10, 0x02};
    memcpy(image + 128u, program, sizeof program);
    image[128u + 0x3FFCu] = 0u;
    image[128u + 0x3FFDu] = 0xC0u;
    for (unsigned value = 1u; value <= 2u; value++) {
        image[63] = (uint8_t)value;
        snprintf(path, sizeof path, "%s/xm%u.a78", dir, value);
        write_bytes(path, image, 128u + 16384u);
    }
    image[63] = 0u;
    static const char *const sounds[] = {"ym2151", "covox", "adpcm"};
    for (unsigned bit = 0u; bit < 3u; bit++) {
        image[67] = (uint8_t)(8u << bit);
        snprintf(path, sizeof path, "%s/%s.a78", dir, sounds[bit]);
        write_bytes(path, image, 128u + 16384u);
    }
    image[67] = 0u;
    image[50] = 1u;
    image[51] = 0xA0u;
    image[65] = 0x80u;
    snprintf(path, sizeof path, "%s/ram52.a78", dir);
    write_bytes(path, image, sizeof image);
    memset(image, 0x64, 4096u);
    snprintf(path, sizeof path, "%s/hsc.bin", dir);
    write_bytes(path, image, 4096u);
    snprintf(path, sizeof path, "%s/bad-hsc.bin", dir);
    write_bytes(path, image, 4095u);
}

static int consume(const char *dir, const char *rom)
{
    char directory[1024], image[1024];
    snprintf(directory, sizeof directory, "%s", dir);
    snprintf(image, sizeof image, "%s", rom);
    char *args[] = {"muhtari-run", "--frames", "1", "--save-dir", directory, image};
    frames_run = temp_opens = replacements = 0;
    create_calls = 0;
    import_stream = save_stream = NULL;
    stop_flag = 0;
    flush_flag = 0;
    return runner_entry(6, args);
}

static void roundtrip(const char *dir, int verify)
{
    char rom[1024], save[1024];
    snprintf(rom, sizeof rom, "%s/game.a78", dir);
    snprintf(save, sizeof save, "%s/game.sav", dir);
    uint8_t original[2048];
    memset(original, 0x5A, sizeof original);
    if (verify) {
        original[0] += 2;
        exact_file(save, original, sizeof original);
        return;
    }
    cartridge(rom, 1u, 1);
    write_bytes(save, original, sizeof original);
    CHECK_EQ_U64(consume(dir, rom), 0);
    CHECK_EQ_U64(frames_run, 1);
    CHECK_EQ_U64(temp_opens, 1);
    original[0]++;
    exact_file(save, original, sizeof original);
}

static void imports(const char *dir)
{
    char rom[1024], save[1024];
    snprintf(rom, sizeof rom, "%s/import.a78", dir);
    snprintf(save, sizeof save, "%s/import.sav", dir);
    static const size_t sizes[] = {2048, 32768, 34816};
    uint8_t *bytes = malloc(70000);
    uint8_t *expected = malloc(34816);
    CHECK(bytes != NULL);
    CHECK(expected != NULL);
    if (bytes == NULL || expected == NULL) {
        free(bytes);
        free(expected);
        return;
    }
    memset(bytes, 0x39, 70000);
    for (unsigned layout = 0; layout < 3; layout++) {
        size_t n = sizes[layout];
        cartridge(rom, (uint8_t)(layout + 1u), 0);
        size_t lengths[] = {0, 7, n, n + 1, 70000};
        for (unsigned k = 0; k < 5; k++) {
            memset(expected, 0xFF, n);
            memcpy(expected, bytes, lengths[k] < n ? lengths[k] : n);
            expected_import = expected;
            expected_bytes = n;
            write_bytes(save, bytes, lengths[k]);
            CHECK_EQ_U64(consume(dir, rom), lengths[k] > n ? 4 : 0);
            CHECK_EQ_U64(frames_run, lengths[k] > n ? 0 : 1);
            CHECK_EQ_U64(temp_opens, 0);
            exact_file(save, bytes, lengths[k]);
        }
        CHECK_EQ_U64(remove(save), 0);
        memset(expected, 0xFF, n);
        CHECK_EQ_U64(consume(dir, rom), 0);
        CHECK_EQ_U64(temp_opens, 0);
        FILE *f = fopen(save, "rb");
        CHECK(f == NULL);
        if (f != NULL)
            fclose(f);
    }
    expected_import = NULL;
    cartridge(rom, 0u, 0);
    write_bytes(save, bytes, 70000);
    CHECK_EQ_U64(consume(dir, rom), 0);
    CHECK_EQ_U64(temp_opens, 0);
    exact_file(save, bytes, 70000);
    CHECK_EQ_U64(remove(save), 0);
    const char *names[] = {"many.parts.a78", ".hidden", "noextension"};
    const char *saves[] = {"many.parts.sav", ".hidden.sav", "noextension.sav"};
    for (unsigned i = 0; i < 3; i++) {
        snprintf(rom, sizeof rom, "%s/%s", dir, names[i]);
        snprintf(save, sizeof save, "%s/%s", dir, saves[i]);
        cartridge(rom, 1u, 1);
        (void)remove(save);
        CHECK_EQ_U64(consume(dir, rom), 0);
        memset(expected, 0xFF, 2048);
        expected[0] = 0;
        exact_file(save, expected, 2048);
        CHECK_EQ_U64(remove(save), 0);
    }
    free(bytes);
    free(expected);
}

static void faults(const char *dir)
{
    char rom[1024], save[1024], foreign[1100], own[1100];
    snprintf(rom, sizeof rom, "%s/fault.a78", dir);
    snprintf(save, sizeof save, "%s/fault.sav", dir);
    cartridge(rom, 1u, 1);
    uint8_t bytes[2048];
    memset(bytes, 0x31, sizeof bytes);
#ifdef _WIN32
    unsigned long pid = (unsigned long)GetCurrentProcessId();
#elif defined(__wasi__)
    unsigned long pid = 0;
#else
    unsigned long pid = (unsigned long)getpid();
#endif
    snprintf(foreign, sizeof foreign, "%s.tmp.%lu.31", save, pid);
    snprintf(own, sizeof own, "%s.tmp.%lu.0", save, pid);
    write_bytes(foreign, bytes, 3);
    for (int fault = IMPORT_READ; fault <= SAVE_REPLACE; fault++) {
        write_bytes(save, bytes, sizeof bytes);
        io_fault = fault;
        CHECK_EQ_U64(consume(dir, rom), 1);
        CHECK_EQ_U64(frames_run, fault <= IMPORT_CLOSE ? 0 : 1);
        CHECK_EQ_U64(temp_opens, fault <= SAVE_FDOPEN ? 0 : 1);
        CHECK_EQ_U64(create_calls, fault <= IMPORT_CLOSE ? 0 : 1);
        io_fault = IO_OK;
        exact_file(save, bytes, sizeof bytes);
        exact_file(foreign, bytes, 3);
        FILE *leftover = fopen(own, "rb");
        CHECK(leftover == NULL);
        if (leftover != NULL)
            fclose(leftover);
    }
    CHECK_EQ_U64(remove(foreign), 0);
    CHECK_EQ_U64(consume("missing-save-directory", rom), 1);
    CHECK_EQ_U64(frames_run, 0);
    write_bytes(save, bytes, sizeof bytes);
    reject_frame = 1;
    CHECK_EQ_U64(consume(dir, rom), 77);
    CHECK_EQ_U64(temp_opens, 0);
    reject_frame = 0;
    exact_file(save, bytes, sizeof bytes);
    char *plain[] = {"muhtari-run", "--frames", "1", rom};
    CHECK_EQ_U64(runner_entry(4, plain), 0);
    exact_file(save, bytes, sizeof bytes);
    char bad_dump[1100];
    snprintf(bad_dump, sizeof bad_dump, "1:%s/missing/picture.png", dir);
    char directory[1024];
    snprintf(directory, sizeof directory, "%s", dir);
    char *output[] = {"muhtari-run", "--frames",     "1",      "--save-dir",
                      directory,     "--dump-frame", bad_dump, rom};
    CHECK_EQ_U64(runner_entry(8, output), 1);
    bytes[0]++;
    exact_file(save, bytes, sizeof bytes);
    output_fault = 1;
    CHECK_EQ_U64(consume(dir, rom), 1);
    output_fault = 0;
    bytes[0]++;
    exact_file(save, bytes, sizeof bytes);
    /* Every collision belongs to someone else and must survive exhaustion. */
    write_bytes(save, bytes, sizeof bytes);
    char temporary[1100];
    for (unsigned i = 0; i < 32; i++) {
        snprintf(temporary, sizeof temporary, "%s.tmp.%lu.%u", save, pid, i);
        write_bytes(temporary, bytes, 3);
    }
    CHECK_EQ_U64(consume(dir, rom), 1);
    CHECK_EQ_U64(temp_opens, 0);
    CHECK_EQ_U64(create_calls, 32);
    exact_file(save, bytes, sizeof bytes);
    for (unsigned i = 0; i < 32; i++) {
        snprintf(temporary, sizeof temporary, "%s.tmp.%lu.%u", save, pid, i);
        exact_file(temporary, bytes, 3);
        CHECK_EQ_U64(remove(temporary), 0);
    }
    /* An early cadence failure stops once and cleanup does not retry it. */
    write_bytes(save, bytes, sizeof bytes);
    char *early[] = {"muhtari-run", "--frames",        "9", "--save-dir",
                     directory,     "--save-interval", "1", rom};
    io_fault = SAVE_WRITE;
    frames_run = replacements = temp_opens = create_calls = 0;
    CHECK_EQ_U64(runner_entry(8, early), 1);
    CHECK_EQ_U64(frames_run, 1);
    CHECK_EQ_U64(temp_opens, 1);
    CHECK_EQ_U64(create_calls, 1);
    CHECK_EQ_U64(replacements, 0);
    io_fault = IO_OK;
    exact_file(save, bytes, sizeof bytes);
    /* Replacement commits; a later stdout failure retains the new bytes. */
    frames_run = replacements = 0;
    output_fault = 1;
    CHECK_EQ_U64(runner_entry(8, early), 1);
    output_fault = 0;
    CHECK_EQ_U64(frames_run, 9);
    CHECK_EQ_U64(replacements, 1);
    bytes[0]++;
    exact_file(save, bytes, sizeof bytes);
}

#ifndef __wasi__
static void nonregular(const char *dir)
{
    char rom[1024], save[1024];
    snprintf(rom, sizeof rom, "%s/fifo.a78", dir);
    snprintf(save, sizeof save, "%s/fifo.sav", dir);
    cartridge(rom, 1u, 1);
    (void)remove(save);
#ifdef _WIN32
    CHECK_EQ_U64(_mkdir(save), 0);
#else
    CHECK_EQ_U64(mkdir(save, 0700), 0);
#endif
    CHECK_EQ_U64(consume(dir, rom), 1);
    CHECK_EQ_U64(frames_run, 0);
#ifdef _WIN32
    CHECK_EQ_U64(_rmdir(save), 0);
#else
    CHECK_EQ_U64(rmdir(save), 0);
    CHECK_EQ_U64(mkfifo(save, 0600), 0);
    CHECK_EQ_U64(consume(dir, rom), 1);
    CHECK_EQ_U64(frames_run, 0);
    CHECK_EQ_U64(remove(save), 0);
#endif
}
#endif

static void save_options(const char *dir)
{
    char rom[1024];
    snprintf(rom, sizeof rom, "%s/option.a78", dir);
    cartridge(rom, 0u, 0);
    char *args[] = {"muhtari-run", "--frames", "1", "--port1", "savekey", rom};
    uint8_t *blank = malloc(34816);
    CHECK(blank != NULL);
    if (blank == NULL)
        return;
    memset(blank, 0xFF, 34816);
    expected_import = blank;
    expected_bytes = 32768;
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, args), 0);
    CHECK_EQ_U64(frames_run, 1);
    args[4] = "atarivox";
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, args), 0);
    args[3] = "--port2";
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, args), 0);
    args[4] = "savekey";
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, args), 0);
    char *combined[] = {"muhtari-run", "--frames", "1", "--hsc", "on", "--port2", "savekey", rom};
    expected_bytes = 34816;
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(8, combined), 0);
    CHECK(notice_stub != 0u);
    char *dual[] = {"muhtari-run", "--frames", "1",        "--port1",
                    "savekey",     "--port2",  "atarivox", rom};
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(8, dual), 77);
    CHECK_EQ_U64(frames_run, 0);
    cartridge(rom, 2u, 0);
    char *suppress[] = {"muhtari-run", "--frames", "1", "--port2", "none", rom};
    expected_bytes = 0;
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, suppress), 0);
    char movie[1024];
    snprintf(movie, sizeof movie, "%s/device.movie", dir);
    static const char text[] = "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n";
    write_bytes(movie, text, sizeof text - 1u);
    char *replay_args[] = {"muhtari-run", "--movie", movie, "--port1", "savekey", rom};
    expected_bytes = 32768;
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, replay_args), 0);
    CHECK_EQ_U64(frames_run, 1);
    replay_args[4] = "none";
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(6, replay_args), 2);
    CHECK_EQ_U64(frames_run, 0);
    char firmware[1024];
    snprintf(firmware, sizeof firmware, "%s/firmware.bin", dir);
    uint8_t bytes[4097];
    memset(bytes, 0x64, sizeof bytes);
    char *fw[] = {"muhtari-run", "--frames", "1", "--hsc", "on", "--hsc-rom", firmware, rom};
    expected_bytes = 2048;
    cartridge(rom, 0u, 0);
    write_bytes(firmware, bytes, 4096);
    expected_firmware = bytes;
    frames_run = 0;
    notice_unknown = notice_signature = notice_sha1 = 0;
    CHECK_EQ_U64(runner_entry(8, fw), 0);
    CHECK_EQ_U64(notice_unknown, 1);
    CHECK_EQ_U64(notice_signature, 1);
    CHECK_EQ_U64(notice_sha1, 1);
    bytes[0x900] = 0xC6;
    bytes[0x904] = 0xFE;
    write_bytes(firmware, bytes, 4096);
    frames_run = notice_signature = 0;
    CHECK_EQ_U64(runner_entry(8, fw), 0);
    CHECK_EQ_U64(notice_signature, 0);
    expected_firmware = NULL;
    size_t bad_sizes[] = {0, 4095, 4097};
    for (unsigned i = 0; i < 3; i++) {
        write_bytes(firmware, bytes, bad_sizes[i]);
        frames_run = 0;
        CHECK_EQ_U64(runner_entry(8, fw), 1);
        CHECK_EQ_U64(frames_run, 0);
    }
    CHECK_EQ_U64(remove(firmware), 0);
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(8, fw), 1);
    CHECK_EQ_U64(frames_run, 0);
    write_bytes(firmware, bytes, 4096);
    fw[4] = "off";
    CHECK_EQ_U64(runner_entry(8, fw), 1);
    CHECK_EQ_U64(frames_run, 0);
    /* XM Auto must discover supplied firmware and import physical HSC bytes
       before the first frame even though both CPU windows begin hidden. */
    char *xm[] = {"muhtari-run", "--frames", "1", "--expansion", "xm", "--hsc-rom", firmware, rom};
    expected_firmware = bytes;
    expected_bytes = 2048u;
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(8, xm), 0);
    CHECK_EQ_U64(frames_run, 1);
    expected_firmware = NULL;
    xm[4] = "xboard";
    frames_run = 0;
    CHECK_EQ_U64(runner_entry(8, xm), 1);
    CHECK_EQ_U64(frames_run, 0);
    xm[4] = "off";
    CHECK_EQ_U64(runner_entry(8, xm), 1);
    expected_import = NULL;
    free(blank);
}

static size_t state_file_size(const char *path)
{
    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    if (f == NULL)
        return 0u;
    CHECK_EQ_U64(fseek(f, 0L, SEEK_END), 0u);
    long end = ftell(f);
    CHECK(end > 0);
    CHECK_EQ_U64(fclose(f), 0u);
    return end > 0 ? (size_t)end : 0u;
}

static uint8_t *state_file_read(const char *path, size_t bytes)
{
    uint8_t *data = (uint8_t *)malloc(bytes);
    FILE *f = fopen(path, "rb");
    CHECK(data != NULL && f != NULL);
    if (data != NULL && f != NULL) {
        CHECK_EQ_U64(fread(data, 1u, bytes, f), bytes);
        CHECK_EQ_U64(fclose(f), 0u);
    } else if (f != NULL) {
        fclose(f);
    }
    return data;
}

static int runner_save_state_zero(char *rom, char *path)
{
    char point[1200];
    snprintf(point, sizeof point, "0:%s", path);
    char *args[] = {"muhtari-run", "--frames0", "--state-at-frame", point, rom};
    frames_run = 0u;
    return runner_entry(5, args);
}

static int runner_load_state_one_with_save(char *dir, char *rom, char *path)
{
    char *args[] = {"muhtari-run", "--frames", "1", "--save-dir", dir, "--load-state", path, rom};
    frames_run = temp_opens = replacements = 0u;
    return runner_entry(8, args);
}

static void state_files(char *dir)
{
    char rom[1024], state[1024], save[1024], nonregular[1024];
    snprintf(rom, sizeof rom, "%s/state-files.a78", dir);
    snprintf(state, sizeof state, "%s/restore.state", dir);
    snprintf(save, sizeof save, "%s/state-files.sav", dir);
    snprintf(nonregular, sizeof nonregular, "%s/nonregular.state", dir);
    cartridge(rom, 1u, 1);
    CHECK_EQ_U64(runner_save_state_zero(rom, state), 0u);
    CHECK_EQ_U64(frames_run, 0u);

    size_t bytes = state_file_size(state);
    uint8_t *valid = state_file_read(state, bytes);
    CHECK(valid != NULL);
    if (valid == NULL)
        return;

    char *exact[] = {"muhtari-run", "--frames0", "--load-state", state, (char *)rom};
    state_fd = -1;
    state_stream = NULL;
    state_probe_reads = 0u;
    state_read_requested = 0u;
    frames_run = 0u;
    CHECK_EQ_U64(runner_entry(5, exact), 0u);
    CHECK_EQ_U64(frames_run, 0u);
    CHECK_EQ_U64(state_probe_reads, 1u);
    CHECK_EQ_U64(state_read_requested, bytes + 1u);
    CHECK(state_fd == -1);
    CHECK(state_stream == NULL);
    if (state_probe_reads != 1u || state_read_requested != bytes + 1u) {
        free(valid);
        return;
    }

    uint8_t newer[2048];
    memset(newer, 0x5Au, sizeof newer);
    write_bytes(save, newer, sizeof newer);
    write_bytes(state, valid, bytes - 1u);
    CHECK_EQ_U64(runner_load_state_one_with_save(dir, rom, state), 1u);
    CHECK_EQ_U64(frames_run, 0u);
    CHECK_EQ_U64(temp_opens, 0u);
    CHECK_EQ_U64(replacements, 0u);
    exact_file(save, newer, sizeof newer);
    uint8_t *longer = (uint8_t *)malloc(bytes + 1u);
    CHECK(longer != NULL);
    if (longer != NULL) {
        memcpy(longer, valid, bytes);
        longer[bytes] = 0u;
        write_bytes(state, longer, bytes + 1u);
        CHECK_EQ_U64(runner_load_state_one_with_save(dir, rom, state), 1u);
        CHECK_EQ_U64(frames_run, 0u);
        CHECK_EQ_U64(temp_opens, 0u);
        CHECK_EQ_U64(replacements, 0u);
        exact_file(save, newer, sizeof newer);
        free(longer);
    }

    write_bytes(state, valid, bytes);
    for (int fault = STATE_READ; fault <= STATE_CLOSE; fault++) {
        io_fault = fault;
        CHECK_EQ_U64(runner_load_state_one_with_save(dir, rom, state), 1u);
        CHECK_EQ_U64(frames_run, 0u);
        CHECK_EQ_U64(temp_opens, 0u);
        CHECK_EQ_U64(replacements, 0u);
        exact_file(save, newer, sizeof newer);
        io_fault = IO_OK;
    }
    char *missing[] = {"muhtari-run", "--frames", "1", "--load-state", nonregular, rom};
    CHECK_EQ_U64(runner_entry(6, missing), 1u);
    CHECK_EQ_U64(frames_run, 0u);

#ifndef __wasi__
#ifdef _WIN32
    CHECK_EQ_U64(_mkdir(nonregular), 0u);
#else
    CHECK_EQ_U64(mkdir(nonregular, 0700), 0u);
#endif
    CHECK_EQ_U64(runner_entry(6, missing), 1u);
    CHECK_EQ_U64(frames_run, 0u);
#ifdef _WIN32
    CHECK_EQ_U64(_rmdir(nonregular), 0u);
#else
    CHECK_EQ_U64(rmdir(nonregular), 0u);
#endif
#endif
    /* A nonregular state path is rejected and closed before fdopen. */
    state_fd = -1;

    uint8_t previous[8] = {0x31u, 0x32u, 0x33u, 0x34u, 0x35u, 0x36u, 0x37u, 0x38u};
    static const int write_faults[] = {SAVE_CREATE, SAVE_FDOPEN, SAVE_PARTIAL, SAVE_WRITE,
                                       SAVE_FLUSH,  SAVE_SYNC,   SAVE_CLOSE,   SAVE_REPLACE};
    for (size_t i = 0; i < sizeof write_faults / sizeof write_faults[0]; i++) {
        write_bytes(state, previous, sizeof previous);
        io_fault = write_faults[i];
        temp_opens = create_calls = replacements = 0u;
        CHECK_EQ_U64(runner_save_state_zero(rom, state), 1u);
        CHECK_EQ_U64(frames_run, 0u);
        CHECK_EQ_U64(temp_opens,
                     write_faults[i] == SAVE_CREATE || write_faults[i] == SAVE_FDOPEN ? 0u : 1u);
        CHECK_EQ_U64(replacements, write_faults[i] == SAVE_REPLACE ? 1u : 0u);
        io_fault = IO_OK;
        exact_file(state, previous, sizeof previous);
    }

    write_bytes(state, valid, bytes);
    write_bytes(save, newer, sizeof newer);
    char *rollback_zero[] = {"muhtari-run",  "--frames0", "--save-dir", dir,
                             "--load-state", state,       rom};
    temp_opens = replacements = 0u;
    CHECK_EQ_U64(runner_entry(7, rollback_zero), 0u);
    CHECK_EQ_U64(frames_run, 0u);
    CHECK_EQ_U64(temp_opens, 0u);
    CHECK_EQ_U64(replacements, 0u);
    exact_file(save, newer, sizeof newer);

    free(valid);
}

static void cadence(const char *dir)
{
    char rom[1024], save[1024], directory[1024];
    snprintf(rom, sizeof rom, "%s/cadence.a78", dir);
    snprintf(save, sizeof save, "%s/cadence.sav", dir);
    snprintf(directory, sizeof directory, "%s", dir);
    cartridge(rom, 1u, 0);
    char *args[] = {"muhtari-run", "--frames",        "9", "--save-dir",
                    directory,     "--save-interval", "4", rom};
    change_every = 1;
    frames_run = temp_opens = replacements = 0;
    (void)remove(save);
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 3);
    CHECK_EQ_U64(replaced_at[0], 4);
    CHECK_EQ_U64(replaced_at[1], 8);
    CHECK_EQ_U64(replaced_at[2], 9);
    args[2] = "3";
    args[6] = "1";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 3);
    CHECK_EQ_U64(replaced_at[0], 1);
    args[2] = "61";
    char *default_args[] = {"muhtari-run", "--frames", "61", "--save-dir", directory, rom};
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(6, default_args), 0);
    CHECK_EQ_U64(replacements, 2);
    CHECK_EQ_U64(replaced_at[0], 60);
    args[2] = "2";
    args[6] = "4294967295";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 1);
    CHECK_EQ_U64(replaced_at[0], 2);
    /* A change just after frame four's clean cadence is persisted at frame eight. */
    change_every = 0;
    change_at = 5;
    args[2] = "9";
    args[6] = "4";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 1);
    CHECK_EQ_U64(replaced_at[0], 8);
    change_at = 0;
    request_at = 1;
    args[2] = "2";
    args[6] = "4294967295";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 0);
    change_every = 1;
    request_during_write = 1;
    args[2] = "3";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(replacements, 3);
    CHECK_EQ_U64(replaced_at[0], 1);
    CHECK_EQ_U64(replaced_at[1], 2);
    CHECK_EQ_U64(replaced_at[2], 3);
    request_at = stop_at = 3;
    args[2] = "9";
    frames_run = replacements = 0;
    CHECK_EQ_U64(runner_entry(8, args), 0);
    CHECK_EQ_U64(frames_run, 3);
    CHECK_EQ_U64(replacements, 1);
    CHECK_EQ_U64(replaced_at[0], 3);
    stop_flag = flush_flag = 0;
    request_at = stop_at = 0;
    change_every = 0;
    char *bad[] = {"0", "-1", "4294967296", "", "no"};
    for (unsigned i = 0; i < sizeof bad / sizeof bad[0]; i++) {
        args[6] = bad[i];
        CHECK_EQ_U64(runner_entry(8, args), 2);
    }
    char *no_dir[] = {"muhtari-run", "--frames", "1", "--save-interval", "1", rom};
    CHECK_EQ_U64(runner_entry(6, no_dir), 2);
    char *missing[] = {"muhtari-run", "--frames", "1", "--save-interval"};
    CHECK_EQ_U64(runner_entry(4, missing), 2);
}

static void trace_bounds(const char *dir)
{
    char rom[1024], trace[1024];
    snprintf(rom, sizeof rom, "%s/trace-bounds.a78", dir);
    snprintf(trace, sizeof trace, "%s/trace-bounds.jsonl", dir);
    cartridge(rom, 1u, 0);
    char *args[] = {"muhtari-run", "--frames", "1", "--trace", trace, "--trace-frame", "1",
                    "--trace-range", "0:65535", "--trace-limit", "131072", rom};
    largest_runner_allocation = 0u;
    track_runner_allocations = 1;
    int status = runner_entry((int)(sizeof args / sizeof args[0]), args);
    track_runner_allocations = 0;
    CHECK_EQ_U64(status, 0u);
    CHECK_EQ_U64(frames_run, 1u);
    CHECK_EQ_U64(sizeof(struct muhtari__trace_event), 24u);
    CHECK(largest_runner_allocation <= 131072u * sizeof(struct muhtari__trace_event));

    struct muhtari__trace_event maximum = {0};
    maximum.tick = UINT64_MAX;
    maximum.line = UINT16_MAX;
    maximum.t = UINT16_MAX;
    maximum.pc = UINT16_MAX;
    maximum.addr = UINT16_MAX;
    maximum.kind = UINT8_MAX;
    maximum.rw = UINT8_MAX;
    maximum.value = UINT8_MAX;
    maximum.a = maximum.x = maximum.y = maximum.s = maximum.p = UINT8_MAX;
    char line[TRACE_LINE_MAX];
    int line_length = trace_format_line(line, sizeof line, &maximum);
    CHECK(line_length >= 0);
    CHECK_EQ_U64((uint64_t)line_length, TRACE_RECORD_MAX_BYTES);
    if (line_length >= 0)
        CHECK(line[line_length - 1] == '\n');
    printf("trace_event_size=%zu maximum_record_line_bytes=%d record_line_ceiling=%u\n",
           sizeof(struct muhtari__trace_event), line_length, TRACE_RECORD_MAX_BYTES);
}

static void trace_measure(char *rom, char *dir, char *region, char *mode)
{
    char trace[1024];
    int n = snprintf(trace, sizeof trace, "%s/trace-%s-%s.jsonl", dir, region, mode);
    CHECK(n > 0 && (size_t)n < sizeof trace);
    if (n <= 0 || (size_t)n >= sizeof trace)
        return;
    char *plain[] = {"muhtari-run", "--frames", "2", "--region", region, "--trace", trace,
                     "--trace-frame", "2", "--trace-range", "0:65535", rom};
    char *maximum[] = {"muhtari-run", "--frames", "2", "--region", region, "--trace", trace,
                       "--trace-frame", "2", "--trace-range", "0:65535", "--trace-limit",
                       "131072", rom};
    trace_write_started_ns = trace_write_finished_ns = 0u;
    time_trace_write = 1;
    int status = strcmp(mode, "maximum") == 0
                     ? runner_entry((int)(sizeof maximum / sizeof maximum[0]), maximum)
                     : runner_entry((int)(sizeof plain / sizeof plain[0]), plain);
    time_trace_write = 0;
    CHECK_EQ_U64(status, 0u);
    CHECK(trace_write_started_ns != 0u && trace_write_finished_ns >= trace_write_started_ns);
    printf("serialization_write_ns=%llu\n",
           (unsigned long long)(trace_write_finished_ns - trace_write_started_ns));
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return 2;
    if (strcmp(argv[2], "roundtrip") == 0 || strcmp(argv[2], "verify") == 0)
        roundtrip(argv[1], strcmp(argv[2], "verify") == 0);
    else if (strcmp(argv[2], "import") == 0)
        imports(argv[1]);
    else if (strcmp(argv[2], "faults") == 0)
        faults(argv[1]);
    else if (strcmp(argv[2], "options") == 0)
        save_options(argv[1]);
    else if (strcmp(argv[2], "files") == 0)
        state_files(argv[1]);
    else if (strcmp(argv[2], "expansion") == 0)
        expansion_images(argv[1]);
    else if (strcmp(argv[2], "cadence") == 0)
        cadence(argv[1]);
    else if (strcmp(argv[2], "trace_bounds") == 0)
        trace_bounds(argv[1]);
    else if (strcmp(argv[2], "trace_arguments") == 0)
        trace_arguments(argv[1]);
    else if (strcmp(argv[2], "trace_paths") == 0)
        trace_paths(argv[1]);
    else if (strcmp(argv[2], "trace_io") == 0)
        trace_io(argv[1]);
    else if (argc == 6 && strcmp(argv[2], "trace_measure") == 0)
        trace_measure(argv[3], argv[1], argv[4], argv[5]);
#ifndef __wasi__
    else if (strcmp(argv[2], "nonregular") == 0)
        nonregular(argv[1]);
#endif
    else
        return 2;
    CHECK_DONE();
}
