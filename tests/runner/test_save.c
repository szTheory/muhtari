/* Actual runner consumer and generated, owned cartridge; runtime bytes stay in build/. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
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
    SAVE_REPLACE
};
static int io_fault;
static int output_fault;
static unsigned frames_run, temp_opens, replacements;
static unsigned create_calls;
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
static FILE *import_stream, *save_stream;
static const uint8_t *expected_import;
static size_t expected_bytes;
static const uint8_t *expected_firmware;
static unsigned notice_stub, notice_unknown, notice_signature, notice_sha1;
static int test_print(FILE *f, const char *format, ...)
{
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

static FILE *test_fopen(const char *path, const char *mode)
{
    FILE *f = fopen(path, mode);
    if (strstr(path, ".sav") != NULL && strcmp(mode, "rb") == 0)
        import_stream = f;
    return f;
}
static FILE *test_fdopen(int fd, const char *mode)
{
    if (strcmp(mode, "rb") != 0 && io_fault == SAVE_FDOPEN)
        return NULL;
#ifdef _WIN32
    FILE *f = _fdopen(fd, mode);
#else
    FILE *f = fdopen(fd, mode);
#endif
    if (strcmp(mode, "rb") == 0)
        import_stream = f;
    else {
        save_stream = f;
        temp_opens++;
    }
    return f;
}
static size_t test_read(void *p, size_t n, size_t count, FILE *f)
{
    return f == import_stream && io_fault == IMPORT_READ ? 0u : fread(p, n, count, f);
}
static int test_error(FILE *f)
{
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
    return _open(path, flags, mode);
#else
    return open(path, flags, (mode_t)mode);
#endif
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
               (f == save_stream && io_fault == SAVE_CLOSE);
    if (f == import_stream)
        import_stream = NULL;
    if (f == save_stream)
        save_stream = NULL;
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
    return reject_frame ? MUHTARI_STATUS_ERR_BIOS_REJECTED_CART : st;
}
int runner_entry(int argc, char **argv);
#define main runner_entry
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
#include "../../runner/main.c"
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
#undef fprintf
#include "../../runner/stop.c"

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
    CHECK_EQ_U64(runner_entry(8, dual), 1);
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
    expected_import = NULL;
    free(blank);
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

int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    if (strcmp(argv[2], "roundtrip") == 0 || strcmp(argv[2], "verify") == 0)
        roundtrip(argv[1], strcmp(argv[2], "verify") == 0);
    else if (strcmp(argv[2], "import") == 0)
        imports(argv[1]);
    else if (strcmp(argv[2], "faults") == 0)
        faults(argv[1]);
    else if (strcmp(argv[2], "options") == 0)
        save_options(argv[1]);
    else if (strcmp(argv[2], "cadence") == 0)
        cadence(argv[1]);
#ifndef __wasi__
    else if (strcmp(argv[2], "nonregular") == 0)
        nonregular(argv[1]);
#endif
    else
        return 2;
    CHECK_DONE();
}
