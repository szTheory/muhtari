/* Optional standalone helper build: compile with MUHTARI_INPUT_TRACE and
 * tests/libretro/input_trace.c. The normal hosted helper remains unchanged. */
#include "../libretro/input_trace.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
static it_writer keyboard_trace = {.fd = -1, .disabled = 1};
static int keyboard_trace_child = -1;
static int keyboard_trace_setup(void)
{
    const char *path = getenv("MUHTARI_INPUT_TRACE_DIR");
    if (path == NULL)
        return 1;
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
    it_emit(&keyboard_trace, IT_END, reason, detail, 0);
    if (keyboard_trace.fd >= 0)
        close(keyboard_trace.fd);
    if (keyboard_trace_child >= 0)
        close(keyboard_trace_child);
    keyboard_trace.fd = keyboard_trace_child = -1;
    keyboard_trace.disabled = 1;
}
