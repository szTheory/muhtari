/* Compile the real adapter with deterministic file/allocation faults. This
   exercises its public load entry point without platform-specific I/O tricks
   or test hooks in the shipped module. All images are generated in memory. */
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../check.h"
#include "../cart/images.h"
#include "internal.h"
#include "libretro.h"

enum { ABSENT, VALID, SHORT, OVERSIZED, BAD_VECTOR, READ_ERROR, OPEN_ERROR };
static int candidates[2];
static FILE *handles[2];
static unsigned opens, closes;
static bool fail_alloc;
static const char *directory;
static char seen_log[4096], seen_message[256];

static FILE *bios_open(const char *path, const char *mode)
{
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
    unsigned i = f == handles[0] ? 0u : 1u;
    return candidates[i] == READ_ERROR;
}

static int bios_close(FILE *f)
{
    (void)f;
    closes++;
    return 0;
}

static void *bios_alloc(size_t size)
{
    return fail_alloc && size == 16385u ? NULL : malloc(size);
}

#define fopen bios_open
#define fread bios_read
#define ferror bios_error
#define fclose bios_close
#define malloc bios_alloc
#include "../../libretro/libretro.c"
#undef fopen
#undef fread
#undef ferror
#undef fclose
#undef malloc

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
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MESSAGE) {
        (void)snprintf(seen_message, sizeof seen_message, "%s",
                       ((const struct retro_message *)data)->msg);
        return true;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT;
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
    retro_deinit();
    CHECK_DONE();
}
