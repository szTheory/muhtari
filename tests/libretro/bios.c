/* Compile the real adapter with deterministic file/allocation faults. This
   exercises its public load entry point without platform-specific I/O tricks
   or test hooks in the shipped module. All images are generated in memory. */
#if !defined(_WIN32) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "../check.h"
#include "../cart/images.h"
#include "internal.h"
#include "libretro.h"

enum {
    ABSENT,
    VALID,
    SHORT,
    OVERSIZED,
    BAD_VECTOR,
    READ_ERROR,
    OPEN_ERROR,
    NONREGULAR,
    CLOSE_ERROR,
#ifdef _WIN32
    PATH_ABSENT,
    PROBE_ERROR,
    TYPE_ERROR,
    TYPE_PIPE,
    TYPE_CHAR,
    PROBE_CLOSE_ERROR,
#endif
    CASE_COUNT
};
static int candidates[2];
static FILE *handles[2];
static unsigned opens, closes;
static bool fail_alloc;
static int hsc_case = -1;
static const char *directory;
static char seen_log[4096], seen_message[256];

static FILE *bios_open(const char *path, const char *mode)
{
    if (strstr(path, "highscre.bin") != NULL) {
        opens++;
        if (hsc_case == OPEN_ERROR) {
            errno = EACCES;
            return NULL;
        }
        return stderr;
    }
    unsigned i = strstr(path, "(U)") != NULL ? 0u : 1u;
    (void)mode;
    opens++;
    if (candidates[i] == ABSENT || candidates[i] == OPEN_ERROR) {
        errno = candidates[i] == ABSENT ? ENOENT : EACCES;
        return NULL;
    }
    return handles[i];
}

static size_t bios_read(void *ptr, size_t size, size_t count, FILE *f)
{
    if (f == stderr) {
        memset(ptr, 0x19, count);
        return hsc_case == SHORT ? 4095u : hsc_case == OVERSIZED ? count : 4096u;
    }
    unsigned i = f == handles[0] ? 0u : 1u;
    uint8_t *buf = (uint8_t *)ptr;
    CHECK_EQ_U64(size, 1u);
    CHECK(count >= 4096u);
    if (candidates[i] == OVERSIZED) {
        memset(buf, 0, count);
        return count;
    }
    memset(buf, 0, 4096u);
    if (candidates[i] != BAD_VECTOR) {
        buf[4093] = 0xF0u; /* reset at $F000, inside this synthetic BIOS */
    }
    return candidates[i] == SHORT ? 17u : 4096u;
}

static int bios_error(FILE *f)
{
    if (f == stderr)
        return hsc_case == READ_ERROR;
    unsigned i = f == handles[0] ? 0u : 1u;
    return candidates[i] == READ_ERROR;
}

static int bios_close(FILE *f)
{
    closes++;
    return f == stderr && hsc_case == CLOSE_ERROR ? EOF : 0;
}

static void *bios_alloc(size_t size)
{
    return fail_alloc && size == (hsc_case >= 0 ? 4097u : 16385u) ? NULL : malloc(size);
}

#ifdef _WIN32
static unsigned probe_opens, probe_closes;
static HANDLE WINAPI bios_probe(LPCSTR path, DWORD access, DWORD share,
                                LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD flags,
                                HANDLE template_file)
{
    CHECK(strstr(path, "highscre.bin") != NULL);
    CHECK_EQ_U64(access, 0u);
    CHECK_EQ_U64(share, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
    CHECK(security == NULL);
    CHECK_EQ_U64(creation, OPEN_EXISTING);
    CHECK_EQ_U64(flags, FILE_ATTRIBUTE_NORMAL);
    CHECK(template_file == NULL);
    probe_opens++;
    if (hsc_case == ABSENT || hsc_case == PATH_ABSENT || hsc_case == PROBE_ERROR ||
        hsc_case == NONREGULAR) {
        SetLastError(hsc_case == ABSENT        ? ERROR_FILE_NOT_FOUND
                     : hsc_case == PATH_ABSENT ? ERROR_PATH_NOT_FOUND
                                               : ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)(uintptr_t)1u;
}

static DWORD WINAPI bios_type(HANDLE handle)
{
    CHECK(handle == (HANDLE)(uintptr_t)1u);
    return hsc_case == TYPE_ERROR  ? FILE_TYPE_UNKNOWN
           : hsc_case == TYPE_PIPE ? FILE_TYPE_PIPE
           : hsc_case == TYPE_CHAR ? FILE_TYPE_CHAR
                                   : FILE_TYPE_DISK;
}

static BOOL WINAPI bios_probe_close(HANDLE handle)
{
    CHECK(handle == (HANDLE)(uintptr_t)1u);
    probe_closes++;
    return hsc_case != PROBE_CLOSE_ERROR;
}
#else
static int bios_stat(const char *path, struct stat *info)
{
    (void)path;
    if (hsc_case == ABSENT) {
        errno = ENOENT;
        return -1;
    }
    memset(info, 0, sizeof *info);
    info->st_mode = hsc_case == NONREGULAR ? S_IFDIR : S_IFREG;
    return 0;
}
#endif

#define fopen bios_open
#define fread bios_read
#define ferror bios_error
#define fclose bios_close
#define malloc bios_alloc
#ifdef _WIN32
#define CreateFileA bios_probe
#define GetFileType bios_type
#define CloseHandle bios_probe_close
#else
#define stat(path, info) bios_stat(path, info)
#endif
#include "../../libretro/libretro.c"
#undef fopen
#undef fread
#undef ferror
#undef fclose
#undef malloc
#ifdef _WIN32
#undef CreateFileA
#undef GetFileType
#undef CloseHandle
#else
#undef stat
#endif

static void RETRO_CALLCONV bios_log(enum retro_log_level level, const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(seen_log);
    (void)level;
    va_start(ap, fmt);
    (void)vsnprintf(seen_log + used, sizeof seen_log - used, fmt, ap);
    va_end(ap);
}

static bool RETRO_CALLCONV bios_env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_CAN_DUPE) {
        *(bool *)data = false;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_LOG_INTERFACE) {
        ((struct retro_log_callback *)data)->log = bios_log;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY) {
        *(const char **)data = directory;
        return directory != NULL;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE) {
        struct retro_variable *v = (struct retro_variable *)data;
        if (strcmp(v->key, "muhtari_boot") == 0) {
            v->value = "bios";
            return true;
        }
        if (hsc_case >= 0 && strcmp(v->key, "muhtari_hsc") == 0) {
            v->value = "on";
            return true;
        }
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MESSAGE) {
        (void)snprintf(seen_message, sizeof seen_message, "%s",
                       ((const struct retro_message *)data)->msg);
        return true;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT;
}

static lr_pixel retained[LR_PIXELS];
static unsigned videos;
static bool capture;

static void RETRO_CALLCONV failure_video(const void *data, unsigned width, unsigned height,
                                         size_t pitch)
{
    videos++;
    CHECK(data != NULL);
    CHECK_EQ_U64(width, LR_WIDTH);
    CHECK_EQ_U64(height, LR_HEIGHT);
    CHECK_EQ_U64(pitch, LR_WIDTH * sizeof retained[0]);
    if (data != NULL) {
        if (capture) {
            memcpy(retained, data, sizeof retained);
        } else {
            CHECK(memcmp(retained, data, sizeof retained) == 0);
        }
    }
}

/* Inject the core's persistent BIOS rejection before the first run and
   after a rendered test card. A frontend without duping requires pixels. */
static void failed_frames(void)
{
    CHECK(retro_load_game(NULL));
    retro_set_video_refresh(failure_video);
    memset(retained, 0, sizeof retained);
    videos = 0u;
    capture = false;
    inst->boot.rejected = 1u;
    retro_run();
    CHECK_EQ_U64(videos, 1u);
    inst->boot.rejected = 0u;
    capture = true;
    retro_run();
    CHECK_EQ_U64(videos, 2u);
    bool nonzero = false;
    for (size_t i = 0; i < LR_PIXELS; i++) {
        nonzero = nonzero || retained[i] != 0u;
    }
    CHECK(nonzero);
    capture = false;
    inst->boot.rejected = 1u;
    retro_run();
    retro_run();
    CHECK_EQ_U64(videos, 4u);
    retro_unload_game();
}

int main(void)
{
    static uint8_t image[128u + 4096u];
    static const struct {
        int first, second;
        bool oom;
        unsigned expected_opens, expected_closes;
        uint32_t bios_size;
        const char *log, *message;
    } cases[] = {
        {VALID, VALID, false, 1u, 1u, 4096u, "BIOS (U).rom, SHA-256", ""},
        {SHORT, VALID, false, 2u, 2u, 4096u, "BIOS (E).rom, SHA-256", ""},
        {OVERSIZED, VALID, false, 2u, 2u, 4096u, "BIOS (E).rom, SHA-256", ""},
        {BAD_VECTOR, VALID, false, 2u, 2u, 4096u, "BIOS (E).rom, SHA-256", ""},
        {READ_ERROR, VALID, false, 2u, 2u, 4096u, "BIOS read error", ""},
        {OPEN_ERROR, VALID, false, 2u, 1u, 4096u, "cannot open BIOS file", ""},
        {SHORT, BAD_VECTOR, false, 2u, 2u, 0u, "no usable", "No usable"},
        {READ_ERROR, ABSENT, false, 2u, 1u, 0u, "BIOS read error", "No usable"},
        {ABSENT, ABSENT, false, 2u, 0u, 0u, "no 7800 BIOS", "No 7800 BIOS"},
        {VALID, VALID, true, 1u, 1u, 0u, "out of memory", "out of memory"},
    };
    struct retro_game_info game;
    /* Borrow distinct stream pointers as tokens; the substituted operations
       never access or close these streams. */
    handles[0] = stdin;
    handles[1] = stdout;
    memset(&game, 0, sizeof game);
    game.data = image;
    game.size = images_linear(image, 3u, 4096u);
    directory = "system";
    retro_set_environment(bios_env);
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        candidates[0] = cases[i].first;
        candidates[1] = cases[i].second;
        fail_alloc = cases[i].oom;
        opens = closes = 0u;
        seen_log[0] = seen_message[0] = '\0';
        CHECK_EQ_U64(retro_load_game(&game), !fail_alloc);
        CHECK_EQ_U64(opens, cases[i].expected_opens);
        CHECK_EQ_U64(closes, cases[i].expected_closes);
        if (inst != NULL) {
            CHECK_EQ_U64(inst->bios_size, cases[i].bios_size);
        }
        CHECK(strstr(seen_log, cases[i].log) != NULL);
        if (cases[i].message[0] == '\0') {
            CHECK_EQ_U64(seen_message[0], 0u);
        } else {
            CHECK(strstr(seen_message, cases[i].message) != NULL);
        }
        if (fail_alloc) {
            CHECK(strstr(seen_log, "no 7800 BIOS") == NULL);
            CHECK(strstr(seen_log, "fast boot") == NULL);
        }
        retro_unload_game();
    }
    static char long_dir[4096];
    memset(long_dir, 'x', sizeof long_dir - 1u);
    directory = long_dir;
    fail_alloc = false;
    opens = closes = 0u;
    seen_log[0] = seen_message[0] = '\0';
    CHECK(retro_load_game(&game));
    CHECK_EQ_U64(opens, 0u);
    CHECK_EQ_U64(inst->bios_size, 0u);
    CHECK(strstr(seen_log, "BIOS path too long") != NULL);
    CHECK(strstr(seen_message, "No usable") != NULL);
    retro_unload_game();
    failed_frames();
    /* Present bad HSC firmware never becomes the missing-file fallback. */
    directory = "system";
    for (unsigned test = ABSENT; test < CASE_COUNT; test++) {
        if (test == BAD_VECTOR)
            continue;
        hsc_case = (int)test;
        candidates[0] = SHORT;
        candidates[1] = VALID;
        fail_alloc = false;
        seen_log[0] = seen_message[0] = '\0';
        bool accepted = test == ABSENT || test == VALID;
#ifdef _WIN32
        accepted = accepted || test == PATH_ABSENT;
        probe_opens = probe_closes = 0u;
        opens = closes = 0u;
#endif
        CHECK_EQ_U64(retro_load_game(&game), accepted);
#ifdef _WIN32
        CHECK_EQ_U64(probe_opens, 1u);
        CHECK_EQ_U64(probe_closes, test == ABSENT || test == PATH_ABSENT || test == PROBE_ERROR ||
                                           test == NONREGULAR
                                       ? 0u
                                       : 1u);
        if (test >= PROBE_ERROR || test == NONREGULAR)
            CHECK_EQ_U64(opens, 0u);
#endif
        if (accepted) {
            muhtari_const_span rom = {0};
            rom.size = (uint32_t)sizeof rom;
            CHECK_EQ_U64(muhtari_get_hsc_rom(inst, &rom), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(rom.bytes, 4096u);
            CHECK_EQ_U64(rom.data[123], test == VALID ? 0x19u : 0xFFu);
            CHECK(strstr(seen_log, test == VALID ? "unknown identity" : "signature-only") != NULL);
            CHECK_EQ_U64(inst->bios_size, 4096u);
        } else {
            CHECK(inst == NULL);
            CHECK(strstr(seen_log, "signature-only") == NULL);
        }
        retro_unload_game();
    }
    hsc_case = VALID;
    fail_alloc = true;
    CHECK(!retro_load_game(&game));
    CHECK(inst == NULL);
    fail_alloc = false;
    directory = long_dir;
    CHECK(!retro_load_game(&game));
    CHECK(inst == NULL);
    retro_deinit();
    CHECK_DONE();
}
