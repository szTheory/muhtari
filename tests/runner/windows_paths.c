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
    security.nLength = sizeof security;
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
    startup.cb = sizeof startup;
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

static wchar_t *state_point(const wchar_t *path)
{
    size_t length = wcslen(path);
    if (length > SIZE_MAX / sizeof(wchar_t) - 3u)
        return NULL;
    wchar_t *point = (wchar_t *)malloc((length + 3u) * sizeof *point);
    if (point != NULL) {
        point[0] = L'0';
        point[1] = L':';
        memcpy(point + 2u, path, (length + 1u) * sizeof *point);
    }
    return point;
}

static int save_state(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *rom,
                      const wchar_t *state, const wchar_t *log_name)
{
    wchar_t *point = state_point(state);
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

static int equal_files(const wchar_t *left, const wchar_t *right)
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
        return 0;
    }
    LARGE_INTEGER a_size, b_size;
    int equal = GetFileSizeEx(a, &a_size) && GetFileSizeEx(b, &b_size) && a_size.QuadPart >= 0 &&
                b_size.QuadPart >= 0 && a_size.QuadPart == b_size.QuadPart &&
                a_size.QuadPart <= 16 * 1024 * 1024;
    uint8_t a_bytes[4096], b_bytes[4096];
    while (equal && a_size.QuadPart > 0) {
        DWORD request = a_size.QuadPart > (LONGLONG)sizeof a_bytes ? (DWORD)sizeof a_bytes
                                                                   : (DWORD)a_size.QuadPart;
        DWORD a_got = 0u, b_got = 0u;
        if (!ReadFile(a, a_bytes, request, &a_got, NULL) ||
            !ReadFile(b, b_bytes, request, &b_got, NULL) || a_got != request || b_got != request ||
            memcmp(a_bytes, b_bytes, request) != 0) {
            equal = 0;
            break;
        }
        a_size.QuadPart -= request;
    }
    (void)CloseHandle(a);
    (void)CloseHandle(b);
    return equal;
}

static int run_namespace(const wchar_t *runner, const wchar_t *work_dir, const wchar_t *fixture,
                         const wchar_t *simple_source, const wchar_t *bios_source,
                         const wchar_t *namespace_root, const wchar_t *name)
{
    static const char movie_bytes[] = "muhtari-movie 1\nport1 savekey\nport2 none\n|||...BB|\n";
    uint8_t firmware_bytes[4096];
    memset(firmware_bytes, 0x64, sizeof firmware_bytes);

    wchar_t *test_dir = wide_join(namespace_root, L"paths-\x732B");
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
    wchar_t *battery = test_dir != NULL ? wide_join(test_dir, L"image-\x732B.sav") : NULL;
    wchar_t *invalid_trace = test_dir != NULL ? wide_join(test_dir, L"invalid-trace.jsonl") : NULL;
    wchar_t *sentinel = test_dir != NULL ? wide_join(test_dir, L"sentinel.bin") : NULL;
    wchar_t *log_name = wide_concat(L"windows-paths-", name);
    wchar_t *baseline = wide_join(work_dir, L"windows-paths-baseline.state");
    wchar_t *owned[] = {image,    state, state_output, movie,   simple,        bios,
                        firmware, trace, png,          battery, invalid_trace, sentinel};
    int failed = test_dir == NULL || image == NULL || state == NULL || state_output == NULL ||
                 movie == NULL || simple == NULL || bios == NULL || firmware == NULL ||
                 trace == NULL || png == NULL || battery == NULL || invalid_trace == NULL ||
                 sentinel == NULL || log_name == NULL || baseline == NULL;
    if (!failed) {
        for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
            if (!delete_file_if_present(owned[i])) {
                report_last_error(L"clearing a previous namespace test file");
                failed = 1;
            }
        }
    }
    if (!failed && !CreateDirectoryW(test_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        report_last_error(L"creating namespace test directory");
        failed = 1;
    }
    if (!failed) {
        DWORD attributes = GetFileAttributesW(test_dir);
        if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u)
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
        const wchar_t *args[] = {runner, L"--frames0", L"--boot", L"bios", L"--bios", bios, simple};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed) {
        const wchar_t *args[] = {runner,       L"--frames0", L"--hsc", L"on",
                                 L"--hsc-rom", firmware,     image};
        if (run_child(runner, work_dir, log_name, args, sizeof args / sizeof args[0], 0u, NULL) !=
            0)
            failed = 1;
    }
    if (!failed) {
        wchar_t *state_arg = state_point(state_output);
        if (state_arg == NULL) {
            failed = 1;
        } else {
            wchar_t *dump_arg = NULL;
            if (png != NULL) {
                size_t prefix = 2u;
                size_t length = wcslen(png);
                dump_arg = (wchar_t *)malloc((prefix + length + 1u) * sizeof *dump_arg);
                if (dump_arg != NULL) {
                    dump_arg[0] = L'1';
                    dump_arg[1] = L':';
                    memcpy(dump_arg + 2u, png, (length + 1u) * sizeof *dump_arg);
                }
            }
            const wchar_t *run_args[] = {
                runner,    L"--frames",      L"1",       L"--save-dir",
                test_dir,  L"--trace",       trace,      L"--trace-frame",
                L"1",      L"--trace-range", L"0:65535", L"--state-at-frame",
                state_arg, L"--dump-frame",  dump_arg,   image};
            if (dump_arg == NULL ||
                run_child(runner, work_dir, log_name, run_args,
                          sizeof run_args / sizeof run_args[0], 0u, NULL) != 0) {
                failed = 1;
            }
            free(dump_arg);
            free(state_arg);
        }
    }
    if (!failed) {
        const wchar_t *outputs[] = {state_output, trace, png, battery};
        for (size_t i = 0u; i < sizeof outputs / sizeof outputs[0]; i++) {
            DWORD attributes = GetFileAttributesW(outputs[i]);
            if (attributes == INVALID_FILE_ATTRIBUTES ||
                (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u) {
                fwprintf(stderr, L"windows path test: expected output is missing\n");
                failed = 1;
                break;
            }
        }
    }

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

    for (size_t i = 0u; i < sizeof owned / sizeof owned[0]; i++) {
        if (owned[i] != NULL && !delete_file_if_present(owned[i])) {
            report_last_error(L"cleaning namespace test file");
            failed = 1;
        }
    }
    if (test_dir != NULL) {
        DWORD attributes = GetFileAttributesW(test_dir);
        if (attributes != INVALID_FILE_ATTRIBUTES && !RemoveDirectoryW(test_dir)) {
            report_last_error(L"removing namespace test directory");
            failed = 1;
        }
    }
    free(test_dir);
    free(image);
    free(state);
    free(state_output);
    free(movie);
    free(simple);
    free(bios);
    free(firmware);
    free(trace);
    free(png);
    free(battery);
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
