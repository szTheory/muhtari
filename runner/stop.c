#include "stop.h"
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>

/* Windows invokes console handlers on a separate thread (HandlerRoutine). */
static volatile LONG stop_flag, flush_flag;
static BOOL WINAPI on_console(DWORD event)
{
    if (event == CTRL_BREAK_EVENT) {
        (void)InterlockedExchange(&flush_flag, 1);
        return TRUE;
    }
    if (event == CTRL_C_EVENT) {
        (void)InterlockedExchange(&stop_flag, 1);
        return TRUE;
    }
    return FALSE;
}
void muhtari__stop_install(void)
{
    /* Registering preserves the inherited CTRL_C ignore attribute. */
    (void)SetConsoleCtrlHandler(on_console, TRUE);
}
int muhtari__stop_requested(void)
{
    return InterlockedCompareExchange(&stop_flag, 0, 0) != 0;
}
int muhtari__flush_requested(void)
{
    return InterlockedExchange(&flush_flag, 0) != 0;
}
#else
#include <signal.h>

/* ISO C17 7.14.1.1 permits handler stores to volatile sig_atomic_t. */
static volatile sig_atomic_t stop_flag, flush_flag;
static void on_signal(int sig)
{
#ifdef SIGUSR1
    if (sig == SIGUSR1)
        flush_flag = 1;
    else
#endif
        stop_flag = 1;
}
static void catch_signal(int sig)
{
#ifdef __wasi__
    if (signal(sig, on_signal) == SIG_IGN)
        (void)signal(sig, SIG_IGN);
#else
    struct sigaction old, action = {0};
    if (sigaction(sig, NULL, &old) != 0 || old.sa_handler == SIG_IGN)
        return;
    action.sa_handler = on_signal;
    (void)sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    (void)sigaction(sig, &action, NULL);
#endif
}
void muhtari__stop_install(void)
{
#if defined(SIGPIPE) && !defined(__wasi__)
    /* Broken output must return EPIPE so the frame loop can save before exit. */
    struct sigaction action = {0};
    action.sa_handler = SIG_IGN;
    (void)sigemptyset(&action.sa_mask);
    (void)sigaction(SIGPIPE, &action, NULL);
#endif
    catch_signal(SIGINT);
    catch_signal(SIGTERM);
#ifdef SIGUSR1
    catch_signal(SIGUSR1);
#endif
}
int muhtari__stop_requested(void)
{
    return stop_flag != 0;
}
int muhtari__flush_requested(void)
{
#if defined(SIGUSR1) && !defined(__wasi__)
    sigset_t set, old;
    (void)sigemptyset(&set);
    (void)sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, &old) != 0)
        return 0;
#endif
    int requested = flush_flag != 0;
    flush_flag = 0;
#if defined(SIGUSR1) && !defined(__wasi__)
    (void)sigprocmask(SIG_SETMASK, &old, NULL);
#endif
    return requested;
}
#endif
