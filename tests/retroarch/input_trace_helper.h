/* Optional standalone helper build: compile with MUHTARI_INPUT_TRACE and
 * tests/libretro/input_trace.c. The normal hosted helper remains unchanged. */
#include "../libretro/input_trace.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Transition-only POST records describe changes, not a per-tick heartbeat. */
typedef struct keyboard_trace_key {
    uint32_t key;
    uint32_t down_count, up_count;
    int64_t first_down_ms, last_down_ms, first_up_ms, last_up_ms;
    int state, observed;
} keyboard_trace_key;

static keyboard_trace_key keyboard_trace_keys[3];
static it_writer keyboard_trace = {.fd = -1, .disabled = 1};
static int keyboard_trace_child = -1;

static void keyboard_trace_reset(void)
{
    memset(keyboard_trace_keys, 0, sizeof keyboard_trace_keys);
    keyboard_trace_keys[0].key = 126u;
    keyboard_trace_keys[1].key = 6u;
    keyboard_trace_keys[2].key = 100u;
}

static void keyboard_trace_post(uint32_t key, int down, pid_t child, int64_t elapsed)
{
    for (unsigned i = 0; i < sizeof keyboard_trace_keys / sizeof keyboard_trace_keys[0]; i++) {
        keyboard_trace_key *entry = &keyboard_trace_keys[i];
        if (entry->key != key)
            continue;
        if (down) {
            if (entry->down_count == 0u)
                entry->first_down_ms = elapsed;
            entry->last_down_ms = elapsed;
            entry->down_count++;
        } else {
            if (entry->up_count == 0u)
                entry->first_up_ms = elapsed;
            entry->last_up_ms = elapsed;
            entry->up_count++;
        }
        if (!entry->observed || entry->state != (down != 0))
            it_emit(&keyboard_trace, IT_POST, key, (uint32_t)(down != 0), (uint32_t)child);
        entry->state = down != 0;
        entry->observed = 1;
        return;
    }
}

static int keyboard_trace_setup(void)
{
    const char *path = getenv("MUHTARI_INPUT_TRACE_DIR");
    if (path == NULL)
        return 1;
    keyboard_trace_reset();
    int dir = open(path, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    struct stat info;
    if (dir < 0)
        return 0;
    int valid = fstat(dir, &info) == 0 && S_ISDIR(info.st_mode) && (info.st_mode & 0777) == 0700 &&
                info.st_uid == getuid();
    int own = valid ? openat(dir, "helper.trace",
                             O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600)
                    : -1;
    int child = own >= 0 ? openat(dir, "core.trace",
                                  O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600)
                         : -1;
    close(dir);
    /* Only the child's dedicated descriptor is made inheritable. */
    if (own < 3 || child < 3 || fcntl(child, F_SETFD, 0) != 0 ||
        !it_begin(&keyboard_trace, own, IT_HELPER, 0)) {
        if (own >= 0)
            close(own);
        if (child >= 0)
            close(child);
        keyboard_trace.fd = -1;
        keyboard_trace.disabled = 1;
        return 0;
    }
    keyboard_trace_child = child;
    return 1;
}
static int keyboard_trace_inherit(void)
{
    if (keyboard_trace.fd >= 0)
        close(keyboard_trace.fd);
    if (keyboard_trace_child >= 3) {
        char value[32];
        snprintf(value, sizeof value, "%d", keyboard_trace_child);
        return setenv("MUHTARI_INPUT_TRACE_FD", value, 1) == 0;
    }
    return unsetenv("MUHTARI_INPUT_TRACE_FD") == 0;
}
static void keyboard_trace_parent(pid_t child)
{
    if (keyboard_trace_child >= 0)
        close(keyboard_trace_child);
    keyboard_trace_child = -1;
    if (child > 0) {
        it_emit(&keyboard_trace, IT_FORK, (uint32_t)child, 0, 0);
        it_emit(&keyboard_trace, IT_START, 0, 0, 0);
    }
}
static void keyboard_trace_finish(uint32_t reason, uint32_t detail)
{
    if (keyboard_trace.fd >= 0) {
        for (unsigned i = 0; i < sizeof keyboard_trace_keys / sizeof keyboard_trace_keys[0]; i++) {
            const keyboard_trace_key *entry = &keyboard_trace_keys[i];
            if (entry->down_count == 0u)
                fprintf(stderr, "input trace key=%u state=down count=0 first_ms=na last_ms=na\n",
                        entry->key);
            else
                fprintf(stderr,
                        "input trace key=%u state=down count=%u first_ms=%lld last_ms=%lld\n",
                        entry->key, entry->down_count, (long long)entry->first_down_ms,
                        (long long)entry->last_down_ms);
            if (entry->up_count == 0u)
                fprintf(stderr, "input trace key=%u state=up count=0 first_ms=na last_ms=na\n",
                        entry->key);
            else
                fprintf(stderr,
                        "input trace key=%u state=up count=%u first_ms=%lld last_ms=%lld\n",
                        entry->key, entry->up_count, (long long)entry->first_up_ms,
                        (long long)entry->last_up_ms);
        }
    }
    it_emit(&keyboard_trace, IT_END, reason, detail, 0);
    if (keyboard_trace.fd >= 0)
        close(keyboard_trace.fd);
    if (keyboard_trace_child >= 0)
        close(keyboard_trace_child);
    keyboard_trace.fd = keyboard_trace_child = -1;
    keyboard_trace.disabled = 1;
}
