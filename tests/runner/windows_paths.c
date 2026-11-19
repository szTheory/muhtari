/* Real-process Windows path acceptance. Generated files stay in the test tree. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void report_last_error(const wchar_t *operation)
{
    fwprintf(stderr, L"windows path test: %ls failed (%lu)\n", operation,
             (unsigned long)GetLastError());
}

static wchar_t *wide_join(const wchar_t *parent, const wchar_t *leaf)
{
    size_t parent_len = wcslen(parent);
    size_t leaf_len = wcslen(leaf);
    int separator = parent_len != 0u && parent[parent_len - 1u] != L'\\';
    if (parent_len > SIZE_MAX - (size_t)separator ||
        parent_len + (size_t)separator > SIZE_MAX - leaf_len - 1u ||
        parent_len + (size_t)separator + leaf_len >= 32767u)
        return NULL;
    size_t length = parent_len + (size_t)separator + leaf_len;
    wchar_t *joined = (wchar_t *)malloc((length + 1u) * sizeof *joined);
    if (joined == NULL)
        return NULL;
    memcpy(joined, parent, parent_len * sizeof *joined);
    if (separator)
        joined[parent_len] = L'\\';
    memcpy(joined + parent_len + (size_t)separator, leaf, (leaf_len + 1u) * sizeof *joined);
    return joined;
}

static wchar_t *wide_concat(const wchar_t *left, const wchar_t *right)
{
    size_t left_len = wcslen(left);
    size_t right_len = wcslen(right);
    if (left_len > SIZE_MAX - right_len - 1u || left_len + right_len >= 32767u)
        return NULL;
    wchar_t *joined = (wchar_t *)malloc((left_len + right_len + 1u) * sizeof *joined);
    if (joined != NULL) {
        memcpy(joined, left, left_len * sizeof *joined);
        memcpy(joined + left_len, right, (right_len + 1u) * sizeof *joined);
    }
    return joined;
}

static wchar_t *wide_prefix(const wchar_t *prefix, const wchar_t *path)
{
    size_t prefix_len = wcslen(prefix);
    size_t path_len = wcslen(path);
    if (prefix_len > SIZE_MAX - path_len - 1u || prefix_len + path_len >= 32767u)
        return NULL;
    wchar_t *joined = (wchar_t *)malloc((prefix_len + path_len + 1u) * sizeof *joined);
    if (joined == NULL)
        return NULL;
    memcpy(joined, prefix, prefix_len * sizeof *joined);
    memcpy(joined + prefix_len, path, (path_len + 1u) * sizeof *joined);
    return joined;
}

static wchar_t *wide_full_path(const wchar_t *path)
{
    wchar_t full[32768];
    DWORD length = GetFullPathNameW(path, (DWORD)(sizeof full / sizeof full[0]), full, NULL);
    if (length == 0u || length >= sizeof full / sizeof full[0])
        return NULL;
    size_t bytes = ((size_t)length + 1u) * sizeof *full;
    wchar_t *copy = (wchar_t *)malloc(bytes);
    if (copy != NULL)
        memcpy(copy, full, bytes);
    return copy;
}

static wchar_t *extended_drive_root(const wchar_t *work_dir)
{
    wchar_t *full = wide_full_path(work_dir);
    if (full == NULL || full[0] == L'\0' || full[1] != L':' || full[2] != L'\\') {
        free(full);
        return NULL;
    }
    wchar_t *extended = wide_prefix(L"\\\\?\\", full);
    free(full);
    return extended;
}

static wchar_t *ordinary_namespace_path(const wchar_t *path)
{
    if (wcsncmp(path, L"\\\\?\\UNC\\", 8u) == 0)
        return wide_prefix(L"\\\\", path + 8u);
    if (wcsncmp(path, L"\\\\?\\", 4u) == 0)
        return _wcsdup(path + 4u);
    return _wcsdup(path);
}

static int append_char(wchar_t *buffer, size_t capacity, size_t *used, wchar_t value)
{
    if (*used >= capacity)
        return 0;
    buffer[(*used)++] = value;
    return 1;
}

static int append_slashes(wchar_t *buffer, size_t capacity, size_t *used, size_t count)
{
    for (size_t i = 0u; i < count; i++) {
        if (!append_char(buffer, capacity, used, L'\\'))
            return 0;
    }
    return 1;
}

/* Quote one argument using the CommandLineToArgvW / CRT parsing rules. */
static int append_quoted(wchar_t *buffer, size_t capacity, size_t *used, const wchar_t *argument)
{
    if (!append_char(buffer, capacity, used, L'"'))
        return 0;
    size_t slashes = 0u;
    for (const wchar_t *p = argument; *p != L'\0'; p++) {
        if (*p == L'\\') {
            slashes++;
        } else if (*p == L'"') {
            if (slashes > (SIZE_MAX - 1u) / 2u ||
                !append_slashes(buffer, capacity, used, slashes * 2u + 1u) ||
                !append_char(buffer, capacity, used, L'"'))
                return 0;
            slashes = 0u;
        } else {
            if (!append_slashes(buffer, capacity, used, slashes) ||
                !append_char(buffer, capacity, used, *p))
                return 0;
            slashes = 0u;
        }
    }
    if (slashes > SIZE_MAX / 2u || !append_slashes(buffer, capacity, used, slashes * 2u) ||
        !append_char(buffer, capacity, used, L'"'))
        return 0;
    return 1;
}

static wchar_t *make_command_line(const wchar_t *const *arguments, size_t count)
{
    size_t capacity = 1u;
    for (size_t i = 0u; i < count; i++) {
        size_t length = wcslen(arguments[i]);
        if (length > (SIZE_MAX - 4u) / 2u || capacity > SIZE_MAX - (length * 2u + 4u))
            return NULL;
        capacity += length * 2u + 4u;
    }
    if (capacity > SIZE_MAX / sizeof(wchar_t))
        return NULL;
    wchar_t *line = (wchar_t *)malloc(capacity * sizeof *line);
    if (line == NULL)
        return NULL;
    size_t used = 0u;
    for (size_t i = 0u; i < count; i++) {
        if (i != 0u && !append_char(line, capacity, &used, L' ')) {
            free(line);
            return NULL;
        }
        if (!append_quoted(line, capacity, &used, arguments[i])) {
            free(line);
            return NULL;
        }
    }
    if (!append_char(line, capacity, &used, L'\0')) {
        free(line);
        return NULL;
    }
    return line;
}

static void report_child_output(const wchar_t *path)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    LARGE_INTEGER size;
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart <= 65536) {
        size_t capacity = (size_t)size.QuadPart;
        char *bytes = (char *)malloc(capacity);
        if (bytes != NULL) {
            DWORD got = 0u;
            if (ReadFile(file, bytes, (DWORD)capacity, &got, NULL) && got != 0u) {
                fputs("child output:\n", stderr);
                (void)fwrite(bytes, 1u, got, stderr);
            }
            free(bytes);
        }
    }
    (void)CloseHandle(file);
}

static int file_contains(const wchar_t *path, const char *text)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    LARGE_INTEGER size;
    int found = 0;
    if (GetFileSizeEx(file, &size) && size.QuadPart >= 0 && size.QuadPart <= 65536) {
        size_t capacity = (size_t)size.QuadPart;
        char *bytes = (char *)malloc(capacity + 1u);
        if (bytes != NULL) {
            DWORD got = 0u;
            if (ReadFile(file, bytes, (DWORD)capacity, &got, NULL) && got == (DWORD)capacity) {
                bytes[got] = '\0';
                found = strstr(bytes, text) != NULL;
            }
            free(bytes);
        }
    }
    (void)CloseHandle(file);
    return found;
}

static int write_bytes(const wchar_t *path, const void *bytes, size_t length)
{
    HANDLE file =
        CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    const uint8_t *position = (const uint8_t *)bytes;
    int good = 1;
    while (length != 0u) {
        DWORD chunk = length > UINT32_MAX ? UINT32_MAX : (DWORD)length;
        DWORD written = 0u;
        if (!WriteFile(file, position, chunk, &written, NULL) || written != chunk) {
            good = 0;
            break;
        }
        position += written;
        length -= written;
    }
    if (!CloseHandle(file))
        good = 0;
    return good;
}

static int delete_file_if_present(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    return DeleteFileW(path) != FALSE;
}

static int run_child(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *log_name,
                     const wchar_t *const *arguments, size_t argument_count, DWORD expected_exit,
                     const char *required_output)
{
    wchar_t *log_path = wide_join(work_dir, log_name);
    wchar_t *command_line = make_command_line(arguments, argument_count);
    if (log_path == NULL || command_line == NULL) {
        free(log_path);
        free(command_line);
        fwprintf(stderr, L"windows path test: could not allocate child command\n");
        return 1;
    }
    SECURITY_ATTRIBUTES security = {0};
    security.nLength = (DWORD)sizeof security;
    security.bInheritHandle = TRUE;
    HANDLE log = CreateFileW(log_path, GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE input = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (log == INVALID_HANDLE_VALUE || input == INVALID_HANDLE_VALUE) {
        report_last_error(L"creating child output handles");
        if (log != INVALID_HANDLE_VALUE)
            (void)CloseHandle(log);
        if (input != INVALID_HANDLE_VALUE)
            (void)CloseHandle(input);
        free(log_path);
        free(command_line);
        return 1;
    }
    STARTUPINFOW startup = {0};
    startup.cb = (DWORD)sizeof startup;
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = log;
    startup.hStdError = log;
    PROCESS_INFORMATION process = {0};
    BOOL started = CreateProcessW(runner, command_line, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
                                  work_dir, &startup, &process);
    DWORD creation_error = GetLastError();
    (void)CloseHandle(input);
    (void)CloseHandle(log);
    free(command_line);
    if (!started) {
        SetLastError(creation_error);
        report_last_error(L"starting muhtari-run");
        (void)DeleteFileW(log_path);
        free(log_path);
        return 1;
    }
    DWORD waited = WaitForSingleObject(process.hProcess, 10000u);
    DWORD exit_code = 1u;
    if (waited == WAIT_OBJECT_0) {
        if (!GetExitCodeProcess(process.hProcess, &exit_code))
            exit_code = 1u;
    } else {
        (void)TerminateProcess(process.hProcess, 124u);
        (void)WaitForSingleObject(process.hProcess, 5000u);
        fwprintf(stderr, L"windows path test: muhtari-run exceeded 10 seconds\n");
    }
    (void)CloseHandle(process.hThread);
    (void)CloseHandle(process.hProcess);
    int output_matches = required_output == NULL || file_contains(log_path, required_output);
    if (waited != WAIT_OBJECT_0 || exit_code != expected_exit || !output_matches) {
        fwprintf(stderr, L"windows path test: muhtari-run exit %lu\n", (unsigned long)exit_code);
        if (!output_matches)
            fwprintf(stderr, L"windows path test: expected diagnostic was not present\n");
        report_child_output(log_path);
    }
    int log_removed = delete_file_if_present(log_path);
    if (!log_removed)
        report_last_error(L"removing child output log");
    free(log_path);
    return waited == WAIT_OBJECT_0 && exit_code == expected_exit && output_matches && log_removed
               ? 0
               : 1;
}

static wchar_t *state_point(const wchar_t *path, wchar_t frame)
{
    size_t length = wcslen(path);
    if (length > SIZE_MAX / sizeof(wchar_t) - 3u)
        return NULL;
    wchar_t *point = (wchar_t *)malloc((length + 3u) * sizeof *point);
    if (point != NULL) {
        point[0] = frame;
        point[1] = L':';
        memcpy(point + 2u, path, (length + 1u) * sizeof *point);
    }
    return point;
}

static int save_state(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *rom,
                      const wchar_t *state, const wchar_t *log_name)
{
    wchar_t *point = state_point(state, L'0');
    if (point == NULL)
        return 1;
    const wchar_t *arguments[] = {runner, L"--frames0", L"--state-at-frame", point, rom};
    int rc = run_child(runner, work_dir, log_name, arguments,
                       sizeof arguments / sizeof arguments[0], 0u, NULL);
    free(point);
    return rc;
}

static int load_state(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *rom,
                      const wchar_t *state, const wchar_t *log_name)
{
    const wchar_t *arguments[] = {runner, L"--frames0", L"--load-state", state, rom};
    return run_child(runner, work_dir, log_name, arguments, sizeof arguments / sizeof arguments[0],
                     0u, NULL);
}

static int load_state_with_savekey(const wchar_t *runner, const wchar_t *work_dir,
                                   const wchar_t *rom, const wchar_t *state,
                                   const wchar_t *log_name)
{
    const wchar_t *arguments[] = {runner,          L"--frames0", L"--port1", L"savekey",
                                  L"--load-state", state,        rom};
    return run_child(runner, work_dir, log_name, arguments, sizeof arguments / sizeof arguments[0],
                     0u, NULL);
}

/* Return 0 for equal bytes, 1 for different bytes, and -1 when comparison fails. */
static int compare_files(const wchar_t *left, const wchar_t *right)
{
    HANDLE a = CreateFileW(left, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE b = CreateFileW(right, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (a == INVALID_HANDLE_VALUE || b == INVALID_HANDLE_VALUE) {
        if (a != INVALID_HANDLE_VALUE)
            (void)CloseHandle(a);
        if (b != INVALID_HANDLE_VALUE)
            (void)CloseHandle(b);
        return -1;
    }
    LARGE_INTEGER a_size, b_size;
    int result = -1;
    if (GetFileSizeEx(a, &a_size) && GetFileSizeEx(b, &b_size) && a_size.QuadPart >= 0 &&
        b_size.QuadPart >= 0 && a_size.QuadPart <= 16 * 1024 * 1024 &&
        b_size.QuadPart <= 16 * 1024 * 1024) {
        result = a_size.QuadPart == b_size.QuadPart ? 0 : 1;
    }
    uint8_t a_bytes[4096], b_bytes[4096];
    while (result == 0 && a_size.QuadPart > 0) {
        DWORD request = a_size.QuadPart > (LONGLONG)sizeof a_bytes ? (DWORD)sizeof a_bytes
                                                                   : (DWORD)a_size.QuadPart;
        DWORD a_got = 0u, b_got = 0u;
        if (!ReadFile(a, a_bytes, request, &a_got, NULL) ||
            !ReadFile(b, b_bytes, request, &b_got, NULL) || a_got != request || b_got != request) {
            result = -1;
            break;
        }
        if (memcmp(a_bytes, b_bytes, request) != 0) {
            result = 1;
            break;
        }
        a_size.QuadPart -= request;
    }
    BOOL a_closed = CloseHandle(a);
    BOOL b_closed = CloseHandle(b);
    if (!a_closed || !b_closed)
        result = -1;
    return result;
}

static int equal_files(const wchar_t *left, const wchar_t *right)
{
    return compare_files(left, right) == 0;
}

static int verify_output_aliases(const wchar_t *runner, const wchar_t *work_dir,
                                 const wchar_t *namespace_root, const wchar_t *fixture,
                                 const wchar_t *name, const wchar_t *log_name)
{
    static const char collision_message[] = "output path conflicts with an input or another output";
    wchar_t *ordinary_root = ordinary_namespace_path(namespace_root);
    wchar_t *image = wide_join(namespace_root, L"identity-alias.a78");
    wchar_t *ordinary_image = image != NULL ? ordinary_namespace_path(image) : NULL;
    wchar_t *image_copy = wide_join(namespace_root, L"identity-alias-copy.bin");
    wchar_t *battery_rom = wide_join(namespace_root, L"identity-battery.sav");
    wchar_t *battery_copy = wide_join(namespace_root, L"identity-battery-copy.bin");
    wchar_t *hard_link = wide_join(namespace_root, L"identity-hard-link.a78");
    wchar_t *frame = ordinary_image != NULL ? state_point(ordinary_image, L'1') : NULL;
    wchar_t *state = ordinary_image != NULL ? state_point(ordinary_image, L'1') : NULL;
    int failed = ordinary_root == NULL || image == NULL || ordinary_image == NULL ||
                 image_copy == NULL || battery_rom == NULL || battery_copy == NULL ||
                 hard_link == NULL || frame == NULL || state == NULL;
    const wchar_t *owned[] = {image, image_copy, battery_rom, battery_copy, hard_link};
    for (size_t i = 0u; !failed && i < sizeof owned / sizeof owned[0]; i++) {
        if (!delete_file_if_present(owned[i])) {
            report_last_error(L"clearing an identity alias fixture");
            failed = 1;
        }
    }
    if (!failed && (!CopyFileW(fixture, image, FALSE) || !CopyFileW(image, image_copy, FALSE))) {
        report_last_error(L"creating an identity collision fixture");
        failed = 1;
    }

    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--dump-frame", frame, image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                      collision_message) != 0 ||
            !equal_files(image, image_copy))
            failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--state-at-frame", state, image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                      collision_message) != 0 ||
            !equal_files(image, image_copy))
            failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {
            runner, L"--frames",      L"1",       L"--trace", ordinary_image, L"--trace-frame",
            L"1",   L"--trace-range", L"0:65535", image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                      collision_message) != 0 ||
            !equal_files(image, image_copy))
            failed = 1;
    }
    if (!failed &&
        (!CopyFileW(fixture, battery_rom, FALSE) || !CopyFileW(battery_rom, battery_copy, FALSE))) {
        report_last_error(L"creating a battery identity collision fixture");
        failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner,        L"--frames",   L"1",
                                 L"--save-dir", ordinary_root, battery_rom};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                      collision_message) != 0 ||
            !equal_files(battery_rom, battery_copy))
            failed = 1;
    }
    if (!failed && wcscmp(name, L"drive") == 0) {
        if (!CreateHardLinkW(hard_link, image, NULL)) {
            report_last_error(L"creating a hard-link alias");
            failed = 1;
        } else {
            const wchar_t *args[] = {runner, L"--frames", L"1", L"--dump-frame", frame, hard_link};
            if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                          collision_message) != 0 ||
                !equal_files(image, image_copy) || !equal_files(hard_link, image_copy))
                failed = 1;
        }
    }

    for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
        if (owned[i] != NULL && !delete_file_if_present(owned[i])) {
            report_last_error(L"removing an identity alias fixture");
            failed = 1;
        }
    }
    free(ordinary_root);
    free(image);
    free(ordinary_image);
    free(image_copy);
    free(battery_rom);
    free(battery_copy);
    free(hard_link);
    free(frame);
    free(state);
    return failed ? 1 : 0;
}

static int file_has_bytes_and_identity(HANDLE original, const wchar_t *path, const char *expected,
                                       size_t expected_length)
{
    HANDLE current =
        CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (current == INVALID_HANDLE_VALUE)
        return 0;
    FILE_ID_INFO original_id, current_id;
    int equal = GetFileInformationByHandleEx(original, FileIdInfo, &original_id,
                                             (DWORD)sizeof original_id) != 0 &&
                GetFileInformationByHandleEx(current, FileIdInfo, &current_id,
                                             (DWORD)sizeof current_id) != 0 &&
                original_id.VolumeSerialNumber == current_id.VolumeSerialNumber &&
                memcmp(original_id.FileId.Identifier, current_id.FileId.Identifier,
                       sizeof original_id.FileId.Identifier) == 0;
    LARGE_INTEGER size;
    if (!GetFileSizeEx(current, &size) || size.QuadPart != (LONGLONG)expected_length)
        equal = 0;
    char bytes[64];
    DWORD got = 0u;
    if (expected_length > sizeof bytes ||
        !ReadFile(current, bytes, (DWORD)expected_length, &got, NULL) ||
        got != (DWORD)expected_length || memcmp(bytes, expected, expected_length) != 0)
        equal = 0;
    (void)CloseHandle(current);
    return equal;
}

static int no_temporary_files(const wchar_t *destination)
{
    const wchar_t *slash = wcsrchr(destination, L'\\');
    if (slash == NULL || slash[1] == L'\0')
        return 0;
    size_t parent_length = (size_t)(slash - destination) + 1u;
    size_t leaf_length = wcslen(slash + 1u);
    wchar_t *parent = (wchar_t *)malloc((parent_length + 1u) * sizeof *parent);
    if (parent == NULL)
        return 0;
    memcpy(parent, destination, parent_length * sizeof *parent);
    parent[parent_length] = L'\0';
    wchar_t *pattern = wide_join(parent, L"*");
    free(parent);
    if (pattern == NULL)
        return 0;
    WIN32_FIND_DATAW entry;
    HANDLE search = FindFirstFileW(pattern, &entry);
    free(pattern);
    if (search == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_NO_MORE_FILES;
    }
    int found = 0;
    do {
        if (wcsncmp(entry.cFileName, slash + 1u, leaf_length) == 0 &&
            wcsncmp(entry.cFileName + leaf_length, L".tmp.", 5u) == 0) {
            found = 1;
            break;
        }
    } while (FindNextFileW(search, &entry));
    DWORD error = GetLastError();
    (void)FindClose(search);
    return !found && error == ERROR_NO_MORE_FILES;
}

static wchar_t *local_volume_root(const wchar_t *local_path)
{
    const DWORD capacity = 32768u;
    wchar_t *volume_root = (wchar_t *)malloc((size_t)capacity * sizeof *volume_root);
    if (volume_root == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    if (!GetVolumePathNameW(local_path, volume_root, capacity)) {
        DWORD error = GetLastError();
        free(volume_root);
        SetLastError(error);
        return NULL;
    }
    size_t length = wcslen(volume_root);
    if (length == 0u || volume_root[length - 1u] != L'\\') {
        free(volume_root);
        SetLastError(ERROR_INVALID_NAME);
        return NULL;
    }
    return volume_root;
}

static int query_volume_limits(const wchar_t *local_path, DWORD *maximum_component,
                               wchar_t *filesystem_name, DWORD filesystem_name_capacity)
{
    wchar_t *volume_root = local_volume_root(local_path);
    if (volume_root == NULL) {
        report_last_error(L"locating the containing local volume");
        return 0;
    }
    BOOL queried = GetVolumeInformationW(volume_root, NULL, 0u, NULL, maximum_component, NULL,
                                         filesystem_name, filesystem_name_capacity);
    free(volume_root);
    if (!queried) {
        report_last_error(L"querying the namespace filesystem limits");
        return 0;
    }
    if (*maximum_component < 8u ||
        *maximum_component > sizeof(((WIN32_FIND_DATAW *)0)->cFileName) / sizeof(wchar_t) - 5u) {
        fwprintf(stderr, L"windows path test: unreasonable filesystem component limit\n");
        return 0;
    }
    return 1;
}

static size_t sibling_temp_candidate_max_length(const wchar_t *destination)
{
    size_t length = wcslen(destination);
    /* The sibling suffix is .tmp.<DWORD PID>.<attempt 0..31>. */
    return length <= SIZE_MAX - 18u ? length + 18u : SIZE_MAX;
}

static int file_starts_with(const wchar_t *path, const void *expected, DWORD expected_length)
{
    HANDLE file =
        CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    unsigned char buffer[64];
    DWORD read = 0u;
    int matches = expected_length <= (DWORD)sizeof buffer &&
                  ReadFile(file, buffer, expected_length, &read, NULL) != 0 &&
                  read == expected_length && memcmp(buffer, expected, expected_length) == 0;
    (void)CloseHandle(file);
    return matches;
}

static int files_have_distinct_identity(const wchar_t *left, const wchar_t *right)
{
    HANDLE left_file =
        CreateFileW(left, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE right_file =
        CreateFileW(right, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (left_file == INVALID_HANDLE_VALUE || right_file == INVALID_HANDLE_VALUE) {
        if (left_file != INVALID_HANDLE_VALUE)
            (void)CloseHandle(left_file);
        if (right_file != INVALID_HANDLE_VALUE)
            (void)CloseHandle(right_file);
        return 0;
    }

    FILE_ID_INFO left_id, right_id;
    int distinct =
        GetFileInformationByHandleEx(left_file, FileIdInfo, &left_id, (DWORD)sizeof left_id) != 0 &&
        GetFileInformationByHandleEx(right_file, FileIdInfo, &right_id, (DWORD)sizeof right_id) !=
            0 &&
        (left_id.VolumeSerialNumber != right_id.VolumeSerialNumber ||
         memcmp(left_id.FileId.Identifier, right_id.FileId.Identifier,
                sizeof left_id.FileId.Identifier) != 0);
    BOOL left_closed = CloseHandle(left_file);
    BOOL right_closed = CloseHandle(right_file);
    return distinct && left_closed && right_closed;
}

static int verify_suffix_distinct_outputs(const wchar_t *runner, const wchar_t *work_dir,
                                          const wchar_t *image, const wchar_t *state_path,
                                          const wchar_t *png_path, const wchar_t *log_name,
                                          const wchar_t *suffix_name)
{
    static const unsigned char png_signature[] = {0x89u, 'P', 'N', 'G', 0x0Du, 0x0Au, 0x1Au, 0x0Au};
    wchar_t *state_arg = state_point(state_path, L'1');
    wchar_t *png_arg = state_point(png_path, L'1');
    int failed = state_arg == NULL || png_arg == NULL;
    if (!failed && (!delete_file_if_present(state_path) || !delete_file_if_present(png_path))) {
        report_last_error(L"clearing suffix-distinct output fixtures");
        failed = 1;
    }
    if (!failed) {
        const wchar_t *arguments[] = {
            runner,    L"--frames",     L"1",    L"--port1", L"savekey", L"--state-at-frame",
            state_arg, L"--dump-frame", png_arg, image};
        failed = run_child(runner, work_dir, log_name, arguments,
                           sizeof arguments / sizeof arguments[0], 0u, NULL) != 0;
    }
    if (!failed) {
        DWORD state_attributes = GetFileAttributesW(state_path);
        DWORD png_attributes = GetFileAttributesW(png_path);
        if (state_attributes == INVALID_FILE_ATTRIBUTES ||
            (state_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u ||
            png_attributes == INVALID_FILE_ATTRIBUTES ||
            (png_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u ||
            !file_starts_with(png_path, png_signature, (DWORD)sizeof png_signature) ||
            !files_have_distinct_identity(state_path, png_path) ||
            load_state_with_savekey(runner, work_dir, image, state_path, log_name) != 0) {
            fwprintf(stderr,
                     L"windows path test: suffix-distinct extended %ls outputs were not "
                     L"separate valid files\n",
                     suffix_name);
            failed = 1;
        }
    }
    if (!delete_file_if_present(state_path) || !delete_file_if_present(png_path)) {
        report_last_error(L"removing suffix-distinct output fixtures");
        failed = 1;
    }
    free(state_arg);
    free(png_arg);
    return failed ? 1 : 0;
}

static int file_contains_bytes(const wchar_t *path, const void *expected, size_t expected_length)
{
    if (expected_length == 0u || expected_length > 64u)
        return -1;
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return -1;

    uint8_t buffer[4096u + 64u];
    size_t carried = 0u;
    int found = 0;
    int failed = 0;
    for (;;) {
        DWORD read = 0u;
        if (!ReadFile(file, buffer + carried, 4096u, &read, NULL)) {
            failed = 1;
            break;
        }
        size_t available = carried + (size_t)read;
        for (size_t i = 0u; i + expected_length <= available; i++) {
            if (memcmp(buffer + i, expected, expected_length) == 0) {
                found = 1;
                break;
            }
        }
        if (found || read == 0u)
            break;
        carried = available < expected_length - 1u ? available : expected_length - 1u;
        memmove(buffer, buffer + available - carried, carried);
    }
    if (!CloseHandle(file))
        failed = 1;
    return failed ? -1 : found;
}

static int file_has_exact_size(const wchar_t *path, LONGLONG expected_size)
{
    HANDLE file =
        CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    LARGE_INTEGER size;
    int matches = GetFileSizeEx(file, &size) != 0 && size.QuadPart == expected_size;
    (void)CloseHandle(file);
    return matches;
}

static int blocked_replace_case(const wchar_t *runner, const wchar_t *work_dir,
                                const wchar_t *log_name, const wchar_t *destination,
                                const wchar_t *const *arguments, size_t argument_count,
                                const char *diagnostic)
{
    static const char sentinel[] = "prior output bytes";
    if (!delete_file_if_present(destination) ||
        !write_bytes(destination, sentinel, sizeof sentinel)) {
        report_last_error(L"preparing a replacement-failure sentinel");
        return 1;
    }
    HANDLE blocker =
        CreateFileW(destination, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (blocker == INVALID_HANDLE_VALUE) {
        report_last_error(L"holding an output without delete sharing");
        return 1;
    }
    int failed =
        run_child(runner, work_dir, log_name, arguments, argument_count, 1u, diagnostic) != 0;
    if (!file_has_bytes_and_identity(blocker, destination, sentinel, sizeof sentinel)) {
        fwprintf(stderr,
                 L"windows path test: blocked replacement changed the previous destination\n");
        failed = 1;
    }
    (void)CloseHandle(blocker);
    if (!no_temporary_files(destination)) {
        fwprintf(stderr, L"windows path test: failed replacement left a sibling temporary\n");
        failed = 1;
    }
    return failed;
}

static int make_battery_cartridge(const wchar_t *path)
{
    uint8_t image[128u + 4096u];
    memset(image, 0, sizeof image);
    image[0] = 4u;
    memcpy(image + 1u, "ATARI7800", 9u);
    image[50] = 0x10u;
    image[55] = image[56] = 1u;
    image[58] = 1u;
    memset(image + 128u, 0xEA, 4096u);
    static const uint8_t program[] = {0xEE, 0x00, 0x10, 0x4C, 0x03, 0xF0};
    memcpy(image + 128u, program, sizeof program);
    image[128u + 0xFFCu] = 0x00u;
    image[128u + 0xFFDu] = 0xF0u;
    return write_bytes(path, image, sizeof image);
}

static int rejected_component_output(const wchar_t *runner, const wchar_t *work_dir,
                                     const wchar_t *log_name, const wchar_t *destination,
                                     const wchar_t *const *arguments, size_t argument_count,
                                     const char *diagnostic)
{
    static const char sentinel[] = "prior output bytes";
    if (!write_bytes(destination, sentinel, sizeof sentinel)) {
        report_last_error(L"preparing a component-boundary sentinel");
        return 1;
    }
    HANDLE original = CreateFileW(destination, GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (original == INVALID_HANDLE_VALUE) {
        report_last_error(L"opening a component-boundary sentinel");
        return 1;
    }
    int failed =
        run_child(runner, work_dir, log_name, arguments, argument_count, 1u, diagnostic) != 0;
    if (!file_has_bytes_and_identity(original, destination, sentinel, sizeof sentinel)) {
        fwprintf(stderr,
                 L"windows path test: component failure changed the previous destination\n");
        failed = 1;
    }
    (void)CloseHandle(original);
    if (!no_temporary_files(destination)) {
        fwprintf(stderr, L"windows path test: component failure left a sibling temporary\n");
        failed = 1;
    }
    return failed;
}

static int verify_component_boundaries(const wchar_t *runner, const wchar_t *work_dir,
                                       const wchar_t *test_dir, const wchar_t *image,
                                       const wchar_t *log_name, DWORD maximum_component)
{
    size_t length = (size_t)maximum_component;
    wchar_t *png_leaf = (wchar_t *)malloc((length + 1u) * sizeof *png_leaf);
    wchar_t *rom_leaf = (wchar_t *)malloc((length + 1u) * sizeof *rom_leaf);
    if (png_leaf == NULL || rom_leaf == NULL) {
        free(png_leaf);
        free(rom_leaf);
        fwprintf(stderr, L"windows path test: allocating component-boundary names failed\n");
        return 1;
    }
    for (size_t i = 0u; i < length; i++)
        png_leaf[i] = rom_leaf[i] = L'p';
    memcpy(png_leaf + length - 4u, L".png", 5u * sizeof *png_leaf);
    memcpy(rom_leaf + length - 4u, L".a78", 5u * sizeof *rom_leaf);

    wchar_t *png = wide_join(test_dir, png_leaf);
    wchar_t *frame = png != NULL ? state_point(png, L'1') : NULL;
    wchar_t *rom = wide_join(test_dir, rom_leaf);
    wchar_t *battery_leaf = _wcsdup(rom_leaf);
    if (battery_leaf != NULL)
        memcpy(battery_leaf + length - 4u, L".sav", 5u * sizeof *battery_leaf);
    wchar_t *battery = battery_leaf != NULL ? wide_join(test_dir, battery_leaf) : NULL;
    int failed =
        png == NULL || frame == NULL || rom == NULL || battery_leaf == NULL || battery == NULL;
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--dump-frame", frame, image};
        failed =
            rejected_component_output(runner, work_dir, log_name, png, args,
                                      sizeof args / sizeof args[0], "cannot open or write") != 0;
    }
    if (!failed && !make_battery_cartridge(rom)) {
        report_last_error(L"creating a maximum-component battery cartridge");
        failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--save-dir", test_dir, rom};
        failed = rejected_component_output(runner, work_dir, log_name, battery, args,
                                           sizeof args / sizeof args[0], "cannot commit save") != 0;
    }
    if (png != NULL && !delete_file_if_present(png)) {
        report_last_error(L"removing a component-boundary PNG");
        failed = 1;
    }
    if (rom != NULL && !delete_file_if_present(rom)) {
        report_last_error(L"removing a component-boundary cartridge");
        failed = 1;
    }
    if (battery != NULL && !delete_file_if_present(battery)) {
        report_last_error(L"removing a component-boundary battery file");
        failed = 1;
    }
    free(png_leaf);
    free(rom_leaf);
    free(png);
    free(frame);
    free(rom);
    free(battery_leaf);
    free(battery);
    return failed ? 1 : 0;
}

static int run_namespace_outputs(const wchar_t *runner, const wchar_t *work_dir,
                                 const wchar_t *test_dir, const wchar_t *image,
                                 const wchar_t *state_output, const wchar_t *trace,
                                 const wchar_t *png, const wchar_t *log_name)
{
    wchar_t *state_arg = state_point(state_output, L'1');
    size_t png_length = wcslen(png);
    wchar_t *dump_arg = NULL;
    if (png_length <= SIZE_MAX / sizeof *dump_arg - 3u) {
        dump_arg = (wchar_t *)malloc((png_length + 3u) * sizeof *dump_arg);
        if (dump_arg != NULL) {
            dump_arg[0] = L'1';
            dump_arg[1] = L':';
            memcpy(dump_arg + 2u, png, (png_length + 1u) * sizeof *dump_arg);
        }
    }
    if (state_arg == NULL || dump_arg == NULL) {
        free(state_arg);
        free(dump_arg);
        fwprintf(stderr, L"windows path test: allocating extended output arguments failed\n");
        return 1;
    }

    const wchar_t *arguments[] = {
        runner,     L"--frames",         L"1",      L"--port1",       L"savekey", L"--save-dir",
        test_dir,   L"--trace",          trace,     L"--trace-frame", L"1",       L"--trace-range",
        L"0:65535", L"--state-at-frame", state_arg, L"--dump-frame",  dump_arg,   image};
    int failed = run_child(runner, work_dir, log_name, arguments,
                           sizeof arguments / sizeof arguments[0], 0u, NULL) != 0;
    free(state_arg);
    free(dump_arg);
    return failed;
}

static int verify_blocked_outputs(const wchar_t *runner, const wchar_t *work_dir,
                                  const wchar_t *test_dir, const wchar_t *image,
                                  const wchar_t *name, const wchar_t *log_name)
{
    wchar_t *png = wide_join(test_dir, L"blocked-frame.png");
    wchar_t *state = wide_join(test_dir, L"blocked-state.bin");
    wchar_t *trace = wide_join(test_dir, L"blocked-trace.jsonl");
    wchar_t *battery_rom = wide_join(test_dir, L"blocked-battery.a78");
    wchar_t *battery = wide_join(test_dir, L"blocked-battery.sav");
    wchar_t *frame = png != NULL ? state_point(png, L'1') : NULL;
    wchar_t *state_arg = state != NULL ? state_point(state, L'1') : NULL;
    int failed = png == NULL || state == NULL || trace == NULL || battery_rom == NULL ||
                 battery == NULL || frame == NULL || state_arg == NULL;
    const wchar_t *owned[] = {png, state, trace, battery_rom, battery};
    for (size_t i = 0u; !failed && i < sizeof owned / sizeof owned[0]; i++) {
        if (!delete_file_if_present(owned[i])) {
            report_last_error(L"clearing a blocked output fixture");
            failed = 1;
        }
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--dump-frame", frame, image};
        failed = blocked_replace_case(runner, work_dir, log_name, png, args,
                                      sizeof args / sizeof args[0], "cannot open or write") != 0;
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--state-at-frame", state_arg, image};
        failed =
            blocked_replace_case(runner, work_dir, log_name, state, args,
                                 sizeof args / sizeof args[0], "cannot commit state file") != 0;
    }
    if (!failed) {
        const wchar_t *args[] = {
            runner, L"--frames",      L"1",       L"--trace", trace, L"--trace-frame",
            L"1",   L"--trace-range", L"0:65535", image};
        failed = blocked_replace_case(runner, work_dir, log_name, trace, args,
                                      sizeof args / sizeof args[0],
                                      "cannot serialize or commit trace file") != 0;
    }
    if (!failed && !make_battery_cartridge(battery_rom)) {
        report_last_error(L"creating a battery output fixture");
        failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--save-dir", test_dir, battery_rom};
        failed = blocked_replace_case(runner, work_dir, log_name, battery, args,
                                      sizeof args / sizeof args[0], "cannot commit save") != 0;
    }
    for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
        if (owned[i] != NULL && !delete_file_if_present(owned[i])) {
            report_last_error(L"removing a blocked output fixture");
            failed = 1;
        }
    }
    free(png);
    free(state);
    free(trace);
    free(battery_rom);
    free(battery);
    free(frame);
    free(state_arg);
    (void)name;
    return failed ? 1 : 0;
}

static wchar_t *deep_test_directory(const wchar_t *root, wchar_t **directories, size_t capacity,
                                    size_t *count)
{
    static const wchar_t component[] =
        L"segment-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-\x732B";
    *count = 0u;
    if (capacity == 0u)
        return NULL;
    directories[0] = wide_join(root, L"paths-\x732B");
    if (directories[0] == NULL)
        return NULL;
    *count = 1u;
    while (*count < capacity) {
        directories[*count] = wide_join(directories[*count - 1u], component);
        if (directories[*count] == NULL)
            return NULL;
        (*count)++;
    }
    return directories[*count - 1u];
}

static int run_namespace(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *fixture,
                         const wchar_t *simple_source, const wchar_t *bios_source,
                         const wchar_t *namespace_root, const wchar_t *name)
{
    static const char movie_bytes[] = "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n";
    uint8_t firmware_bytes[4096];
    memset(firmware_bytes, 0x64, sizeof firmware_bytes);

    wchar_t *directories[8] = {0};
    size_t directory_count = 0u;
    wchar_t *test_dir = deep_test_directory(
        namespace_root, directories, sizeof directories / sizeof directories[0], &directory_count);
    DWORD maximum_component = 0u;
    wchar_t filesystem_name[MAX_PATH + 1u] = {0};
    wchar_t *image = test_dir != NULL ? wide_join(test_dir, L"image-\x732B.a78") : NULL;
    wchar_t *state = test_dir != NULL ? wide_join(test_dir, L"state-\x732B. ") : NULL;
    wchar_t *state_output =
        test_dir != NULL ? wide_join(test_dir, L"state-output-\x732B.state.") : NULL;
    wchar_t *movie = test_dir != NULL ? wide_join(test_dir, L"movie-\x732B. ") : NULL;
    wchar_t *simple = test_dir != NULL ? wide_join(test_dir, L"simple-\x732B.a78") : NULL;
    wchar_t *bios = test_dir != NULL ? wide_join(test_dir, L"bios-\x732B.bin.") : NULL;
    wchar_t *firmware = test_dir != NULL ? wide_join(test_dir, L"firmware-\x732B.bin ") : NULL;
    wchar_t *trace = test_dir != NULL ? wide_join(test_dir, L"trace-\x732B.jsonl ") : NULL;
    wchar_t *png = test_dir != NULL ? wide_join(test_dir, L"frame-\x732B.png.") : NULL;
    wchar_t *dot_state = test_dir != NULL ? wide_join(test_dir, L"suffix-distinct-dot") : NULL;
    wchar_t *dot_png = test_dir != NULL ? wide_join(test_dir, L"suffix-distinct-dot.") : NULL;
    wchar_t *space_state = test_dir != NULL ? wide_join(test_dir, L"suffix-distinct-space") : NULL;
    wchar_t *space_png = test_dir != NULL ? wide_join(test_dir, L"suffix-distinct-space ") : NULL;
    wchar_t *battery = test_dir != NULL ? wide_join(test_dir, L"image-\x732B.sav") : NULL;
    wchar_t *battery_before = wide_join(namespace_root, L"replacement-before.sav");
    wchar_t *invalid_trace = test_dir != NULL ? wide_join(test_dir, L"invalid-trace.jsonl") : NULL;
    wchar_t *sentinel = test_dir != NULL ? wide_join(test_dir, L"sentinel.bin") : NULL;
    wchar_t *log_name = wide_concat(L"windows-paths-", name);
    wchar_t *baseline = wide_join(work_dir, L"windows-paths-baseline.state");
    wchar_t *owned[] = {image,         state,       state_output, movie,   simple,
                        bios,          firmware,    trace,        png,     dot_state,
                        dot_png,       space_state, space_png,    battery, battery_before,
                        invalid_trace, sentinel};
    int failed = test_dir == NULL || image == NULL || state == NULL || state_output == NULL ||
                 movie == NULL || simple == NULL || bios == NULL || firmware == NULL ||
                 trace == NULL || png == NULL || dot_state == NULL || dot_png == NULL ||
                 space_state == NULL || space_png == NULL || battery == NULL ||
                 battery_before == NULL || invalid_trace == NULL || sentinel == NULL ||
                 log_name == NULL || baseline == NULL;
    if (!failed) {
        for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
            if (!delete_file_if_present(owned[i])) {
                report_last_error(L"clearing a previous namespace test file");
                failed = 1;
            }
        }
    }
    if (!failed) {
        for (size_t i = 0u; i < directory_count; i++) {
            if (!CreateDirectoryW(directories[i], NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                report_last_error(L"creating deep namespace test directory");
                failed = 1;
                break;
            }
            DWORD attributes = GetFileAttributesW(directories[i]);
            if (attributes == INVALID_FILE_ATTRIBUTES ||
                (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u) {
                report_last_error(L"checking deep namespace test directory");
                failed = 1;
                break;
            }
        }
        if (!failed && wcslen(test_dir) <= (size_t)MAX_PATH) {
            fwprintf(stderr, L"windows path test: generated path did not exceed MAX_PATH\n");
            failed = 1;
        }
        if (!failed &&
            !query_volume_limits(wcscmp(name, L"drive") == 0 ? namespace_root : work_dir,
                                 &maximum_component, filesystem_name,
                                 (DWORD)(sizeof filesystem_name / sizeof filesystem_name[0])))
            failed = 1;
    }
    if (!failed && (!CopyFileW(fixture, image, FALSE) || !CopyFileW(simple_source, simple, FALSE) ||
                    !CopyFileW(bios_source, bios, FALSE) ||
                    !write_bytes(movie, movie_bytes, sizeof movie_bytes - 1u) ||
                    !write_bytes(firmware, firmware_bytes, sizeof firmware_bytes))) {
        report_last_error(L"preparing namespace path fixtures");
        failed = 1;
    }
    if (!failed && wcscmp(name, L"drive") == 0) {
        if (!delete_file_if_present(baseline) ||
            save_state(runner, work_dir, fixture, baseline, L"windows-paths-baseline.log") != 0)
            failed = 1;
    }
    if (!failed && wcscmp(name, L"unc") == 0 &&
        GetFileAttributesW(baseline) == INVALID_FILE_ATTRIBUTES) {
        fwprintf(stderr, L"windows path test: drive baseline was not produced before UNC\n");
        failed = 1;
    }

    if (!failed) {
        const wchar_t *args[] = {runner, L"--probe", image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed && save_state(runner, work_dir, image, state, log_name) != 0)
        failed = 1;
    if (!failed && !equal_files(baseline, state)) {
        fwprintf(stderr, L"windows path test: state bytes differ from ordinary-path control\n");
        failed = 1;
    }
    if (!failed && load_state(runner, work_dir, image, state, log_name) != 0)
        failed = 1;

    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames", L"1", L"--movie", movie, image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner,  L"--frames", L"1", L"--boot",
                                 L"bios", L"--bios",   bios, simple};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner, L"--frames",  L"1",     L"--hsc",
                                 L"on",  L"--hsc-rom", firmware, image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed) {
        failed = run_namespace_outputs(runner, work_dir, test_dir, image, state_output, trace, png,
                                       log_name) != 0;
    }
    if (!failed) {
        const wchar_t *outputs[] = {state_output, trace, png, battery};
        static const wchar_t *const output_roles[] = {L"state", L"trace", L"PNG", L"battery"};
        for (size_t i = 0u; i < sizeof outputs / sizeof outputs[0]; i++) {
            DWORD attributes = GetFileAttributesW(outputs[i]);
            if (attributes == INVALID_FILE_ATTRIBUTES ||
                (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u) {
                fwprintf(stderr, L"windows path test: expected %ls output is missing: %ls\n",
                         output_roles[i], outputs[i]);
                failed = 1;
                break;
            }
        }
    }
    static const unsigned char png_signature[] = {0x89u, 'P', 'N', 'G', 0x0Du, 0x0Au, 0x1Au, 0x0Au};
    static const char trace_header[] = "{\"format\":\"muhtari-trace\"";
    if (!failed && (!file_starts_with(png, png_signature, (DWORD)sizeof png_signature) ||
                    !file_starts_with(trace, trace_header, (DWORD)(sizeof trace_header - 1u)) ||
                    !file_has_exact_size(battery, 34816))) {
        fwprintf(stderr, L"windows path test: PNG, trace, or combined HSC/SaveKey battery bytes "
                         L"did not match the expected output\n");
        failed = 1;
    }
    if (!failed && load_state_with_savekey(runner, work_dir, image, state_output, log_name) != 0) {
        fwprintf(stderr, L"windows path test: generated state output could not be loaded\n");
        failed = 1;
    }
    if (!failed) {
        static const char replacement_sentinel[] = "prior extended output bytes";
        if (!CopyFileW(battery, battery_before, FALSE) ||
            !write_bytes(state_output, replacement_sentinel, sizeof replacement_sentinel - 1u) ||
            !write_bytes(trace, replacement_sentinel, sizeof replacement_sentinel - 1u) ||
            !write_bytes(png, replacement_sentinel, sizeof replacement_sentinel - 1u)) {
            report_last_error(L"preparing existing extended output destinations");
            failed = 1;
        }
        if (!failed && run_namespace_outputs(runner, work_dir, test_dir, image, state_output, trace,
                                             png, log_name) != 0)
            failed = 1;
        if (!failed &&
            (!file_starts_with(png, png_signature, (DWORD)sizeof png_signature) ||
             !file_starts_with(trace, trace_header, (DWORD)(sizeof trace_header - 1u)) ||
             file_contains_bytes(state_output, replacement_sentinel,
                                 sizeof replacement_sentinel - 1u) != 0 ||
             file_contains_bytes(trace, replacement_sentinel, sizeof replacement_sentinel - 1u) !=
                 0 ||
             file_contains_bytes(png, replacement_sentinel, sizeof replacement_sentinel - 1u) !=
                 0 ||
             !file_has_exact_size(battery, 34816) || compare_files(battery_before, battery) != 1)) {
            fwprintf(stderr, L"windows path test: successful replacement of an existing extended "
                             L"output did not produce expected bytes\n");
            failed = 1;
        }
        if (!failed &&
            load_state_with_savekey(runner, work_dir, image, state_output, log_name) != 0) {
            fwprintf(stderr,
                     L"windows path test: replaced extended state output could not be loaded\n");
            failed = 1;
        }
        if (!failed)
            printf("windows-paths: successful replacement verified for extended %ls paths\n", name);
    }
    if (!failed && (verify_suffix_distinct_outputs(runner, work_dir, image, dot_state, dot_png,
                                                   log_name, L"trailing-dot") != 0 ||
                    verify_suffix_distinct_outputs(runner, work_dir, image, space_state, space_png,
                                                   log_name, L"trailing-space") != 0)) {
        failed = 1;
    }
    if (!failed)
        printf("windows-paths: suffix-distinct outputs verified for extended %ls paths\n", name);
    if (!failed && verify_output_aliases(runner, work_dir, namespace_root, fixture, name, log_name))
        failed = 1;
    if (!failed && verify_blocked_outputs(runner, work_dir, test_dir, image, name, log_name))
        failed = 1;
    if (!failed &&
        verify_component_boundaries(runner, work_dir, test_dir, image, log_name, maximum_component))
        failed = 1;

    static const wchar_t *const invalid_paths[] = {
        L"\\\\?\\relative\\bad.a78",
        L"\\\\.\\C:\\bad.a78",
        L"\\??\\C:\\bad.a78",
        L"\\\\?\\Volume{00000000-0000-0000-0000-000000000000}\\bad.a78",
        L"\\\\?\\UNC\\server",
        L"\\\\?\\UNC\\\\share\\bad.a78",
        L"\\\\?\\C:relative\\bad.a78",
    };
    static const char sentinel_bytes[] = "must remain unchanged\n";
    if (!failed && (!write_bytes(invalid_trace, sentinel_bytes, sizeof sentinel_bytes - 1u) ||
                    !write_bytes(sentinel, sentinel_bytes, sizeof sentinel_bytes - 1u)))
        failed = 1;
    for (size_t i = 0u; !failed && i < sizeof invalid_paths / sizeof invalid_paths[0]; i++) {
        const wchar_t *args[] = {runner,        L"--frames",      L"1", L"--trace",
                                 invalid_trace, L"--trace-frame", L"1", L"--trace-range",
                                 L"0:65535",    invalid_paths[i]};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 2u,
                      "unsupported or malformed Windows path namespace") != 0 ||
            !equal_files(invalid_trace, sentinel)) {
            fwprintf(stderr, L"windows path test: malformed namespace changed an output\n");
            failed = 1;
        }
    }

    if (!failed) {
        const wchar_t *outputs[] = {state_output, trace, png, battery};
        size_t longest_output = 0u;
        size_t longest_temp_candidate = 0u;
        for (size_t i = 0u; i < sizeof outputs / sizeof outputs[0]; i++) {
            size_t output_length = wcslen(outputs[i]);
            size_t temp_length = sibling_temp_candidate_max_length(outputs[i]);
            if (output_length > longest_output)
                longest_output = output_length;
            if (temp_length > longest_temp_candidate)
                longest_temp_candidate = temp_length;
        }
        printf("windows-paths: namespace=%ls filesystem=%ls filesystem_limit_source=%ls "
               "max_component_utf16=%lu "
               "test_dir_utf16=%zu max_path_exceeded=yes max_final_output_utf16=%zu "
               "battery_sav_utf16=%zu battery_temp_candidate_max_utf16=%zu "
               "all_temp_candidate_max_utf16=%zu suffix_case=literal_trailing_dot_and_space\n",
               name, filesystem_name,
               wcscmp(name, L"drive") == 0 ? L"namespace-drive" : L"SMB-backing-drive",
               (unsigned long)maximum_component, wcslen(test_dir), longest_output, wcslen(battery),
               sibling_temp_candidate_max_length(battery), longest_temp_candidate);
    }

    for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
        if (owned[i] != NULL && !delete_file_if_present(owned[i])) {
            report_last_error(L"cleaning namespace test file");
            failed = 1;
        }
    }
    for (size_t i = directory_count; i != 0u; i--) {
        if (directories[i - 1u] != NULL) {
            DWORD attributes = GetFileAttributesW(directories[i - 1u]);
            if (attributes != INVALID_FILE_ATTRIBUTES && !RemoveDirectoryW(directories[i - 1u])) {
                report_last_error(L"removing deep namespace test directory");
                failed = 1;
            }
            free(directories[i - 1u]);
        }
    }
    free(image);
    free(state);
    free(state_output);
    free(movie);
    free(simple);
    free(bios);
    free(firmware);
    free(trace);
    free(png);
    free(dot_state);
    free(dot_png);
    free(space_state);
    free(space_png);
    free(battery);
    free(battery_before);
    free(invalid_trace);
    free(sentinel);
    free(log_name);
    free(baseline);
    return failed ? 1 : 0;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 7 && argc != 8) {
        fwprintf(
            stderr,
            L"usage: windows-paths RUNNER WORK-DIR FIXTURE SIMPLE BIOS drive|unc [UNC-ROOT]\n");
        return 2;
    }
    int drive = wcscmp(argv[6], L"drive") == 0;
    int unc = wcscmp(argv[6], L"unc") == 0;
    if ((!drive && !unc) || (drive && argc != 7) || (unc && argc != 8))
        return 2;
    wchar_t *root = drive ? extended_drive_root(argv[2]) : NULL;
    if (unc) {
        if (wcsncmp(argv[7], L"\\\\?\\UNC\\", 8u) != 0) {
            fwprintf(stderr, L"windows path test: UNC root is not an extended UNC path\n");
            return 1;
        }
        root = _wcsdup(argv[7]);
    }
    if (root == NULL) {
        report_last_error(L"building namespace root");
        return 1;
    }
    int rc = run_namespace(argv[1], argv[2], argv[3], argv[4], argv[5], root, argv[6]);
    free(root);
    if (unc || rc != 0) {
        wchar_t *baseline = wide_join(argv[2], L"windows-paths-baseline.state");
        if (baseline == NULL || !delete_file_if_present(baseline)) {
            report_last_error(L"removing ordinary-path baseline state");
            rc = 1;
        }
        free(baseline);
    }
    return rc;
}
