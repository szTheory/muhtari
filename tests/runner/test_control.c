/* Native OS delivery to the actual runner; all generated media stay build-local. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#endif
#include "../check.h"

static void pause_poll(void)
{
#ifdef _WIN32
    Sleep(10);
#else
    struct timespec delay = {0, 10000000};
    (void)nanosleep(&delay, NULL);
#endif
}
static int exists(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return 0;
    (void)fclose(f);
    return 1;
}
static int expected_save(const char *path)
{
    uint8_t bytes[2049];
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return 0;
    size_t got = fread(bytes, 1u, sizeof bytes, f);
    int rc = fclose(f);
    if (got != 2048u || rc != 0 || bytes[0] != 0x32u)
        return 0;
    for (size_t i = 1; i < got; i++)
        if (bytes[i] != 0x31u)
            return 0;
    return 1;
}
static void write_file(const char *path, const uint8_t *bytes, size_t size)
{
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(bytes, 1u, size, f), size);
        CHECK_EQ_U64(fclose(f), 0);
    }
}
#ifdef _WIN32
static BOOL WINAPI own_console(DWORD event)
{
    return event == CTRL_BREAK_EVENT || event == CTRL_C_EVENT;
}
#endif

static void exercise(int command_count, char **command_args, char *dir, unsigned case_number)
{
    char rom[1024], save[1024], ready[1024], dump[1100], log[1024];
    (void)snprintf(rom, sizeof rom, "%s/control.a78", dir);
    (void)snprintf(save, sizeof save, "%s/control.sav", dir);
    (void)snprintf(ready, sizeof ready, "%s/ready.png", dir);
    (void)snprintf(dump, sizeof dump, "1:%s", ready);
    (void)snprintf(log, sizeof log, "%s/control.txt", dir);
    uint8_t cart[128u + 4096u], bytes[2048];
    memset(cart, 0, sizeof cart);
    cart[0] = 4;
    memcpy(cart + 1, "ATARI7800", 9);
    cart[50] = 0x10;
    cart[55] = cart[56] = cart[58] = 1;
    memcpy(cart + 100, "ACTUAL CART DATA STARTS HERE", 28);
    memset(cart + 128, 0xEA, 4096);
    /* One emulated increment then a bounded idle loop: the full saved bytes are known. */
    static const uint8_t code[] = {0xEE, 0, 0x10, 0x4C, 3, 0xF0};
    memcpy(cart + 128, code, sizeof code);
    cart[128 + 0xFFC] = 0;
    cart[128 + 0xFFD] = 0xF0;
    memset(bytes, 0x31, sizeof bytes);
    write_file(rom, cart, sizeof cart);
    write_file(save, bytes, sizeof bytes);
    (void)remove(ready);
#ifdef _WIN32
    (void)command_count;
    SECURITY_ATTRIBUTES security = {(DWORD)sizeof security, NULL, TRUE};
    HANDLE output = CreateFileA(log, GENERIC_WRITE, FILE_SHARE_READ, &security, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK(output != INVALID_HANDLE_VALUE);
    if (output == INVALID_HANDLE_VALUE)
        return;
    STARTUPINFOA startup = {0};
    PROCESS_INFORMATION child = {0};
    startup.cb = (DWORD)sizeof startup;
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = startup.hStdError = output;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    char command[5000];
    (void)snprintf(command, sizeof command,
                   "\"%s\" --frames 4294967295 --save-dir \"%s\" --save-interval 4294967295 "
                   "--dump-frame \"%s\" \"%s\"",
                   command_args[0], dir, dump, rom);
    /* Same private console, no new group: CTRL_C cannot target a nonzero group. */
    BOOL launched =
        CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &child);
    CHECK(launched);
    CloseHandle(output);
    if (!launched)
        return;
#else
    pid_t child = fork();
    CHECK(child >= 0);
    if (child < 0)
        return;
    if (child == 0) {
        int fd = open(log, O_CREAT | O_TRUNC | O_WRONLY, 0600);
        if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
            _exit(126);
        (void)close(fd);
        char *args[32];
        int n = 0;
        for (int i = 0; i < command_count; i++)
            args[n++] = command_args[i];
        args[n++] = "--frames";
        args[n++] = "4294967295";
        args[n++] = "--save-dir";
        args[n++] = dir;
        args[n++] = "--save-interval";
        args[n++] = "4294967295";
        args[n++] = "--dump-frame";
        args[n++] = dump;
        args[n++] = rom;
        args[n] = NULL;
        execvp(args[0], args);
        _exit(127);
    }
#endif
    unsigned polls = 0;
    while (polls++ < 500u && !exists(ready))
        pause_poll();
    CHECK(exists(ready));
    if (case_number == 0u && exists(ready)) {
        for (unsigned repeat = 0; repeat < 4; repeat++) {
#ifdef _WIN32
            CHECK(GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0));
#else
            CHECK_EQ_U64(kill(child, SIGUSR1), 0);
#endif
            pause_poll();
        }
        polls = 0;
        while (polls++ < 500u && !expected_save(save))
            pause_poll();
        CHECK(expected_save(save));
        /* On-demand persistence must complete before the child exits. */
#ifdef _WIN32
        CHECK_EQ_U64(WaitForSingleObject(child.hProcess, 0), WAIT_TIMEOUT);
#else
        int status = 0;
        CHECK_EQ_U64(waitpid(child, &status, WNOHANG), 0);
#endif
    }
#ifdef _WIN32
    CHECK(GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0));
    DWORD waited = WaitForSingleObject(child.hProcess, 5000);
    CHECK_EQ_U64(waited, WAIT_OBJECT_0);
    if (waited != WAIT_OBJECT_0) {
        (void)TerminateProcess(child.hProcess, 125);
        (void)WaitForSingleObject(child.hProcess, 1000);
    }
    DWORD status = 125;
    CHECK(GetExitCodeProcess(child.hProcess, &status));
    CHECK_EQ_U64(status, 0);
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
#else
    CHECK_EQ_U64(kill(child, case_number == 0u ? SIGINT : SIGTERM), 0);
    int status = 0;
    pid_t result = 0;
    polls = 0;
    while (polls++ < 500u && (result = waitpid(child, &status, WNOHANG)) == 0)
        pause_poll();
    CHECK(result == child);
    if (result == 0) {
        (void)kill(child, SIGKILL);
        (void)waitpid(child, &status, 0);
    }
    CHECK(WIFEXITED(status));
    if (WIFEXITED(status))
        CHECK_EQ_U64(WEXITSTATUS(status), 0);
#endif
    CHECK(expected_save(save));
    (void)remove(ready);
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 18)
        return 2;
#ifdef _WIN32
    /* Own the console so broadcast C/BREAK cannot touch the CI runner or shell. */
    (void)FreeConsole();
    CHECK(AllocConsole());
    CHECK(SetConsoleCtrlHandler(NULL, FALSE));
    CHECK(SetConsoleCtrlHandler(own_console, TRUE));
#endif
    exercise(argc - 2, argv + 2, argv[1], 0);
    exercise(argc - 2, argv + 2, argv[1], 1);
#ifdef _WIN32
    (void)FreeConsole();
#endif
    CHECK_DONE();
}
