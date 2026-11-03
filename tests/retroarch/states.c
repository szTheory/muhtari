/* Bounded RetroArch save/load control engine and owned mock transport. */
#define _DARWIN_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if !defined(__wasi__) && !defined(_WIN32)
#define STATES_POSIX 1
#include <arpa/inet.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <net/if.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <libproc.h>
#endif
#else
#define STATES_POSIX 0
#endif

#define STATES_WORK_MS 45000u
#define STATES_CLEANUP_MS 5000u
#define STATES_TOTAL_MS (STATES_WORK_MS + STATES_CLEANUP_MS)
#define STATES_OBSERVE_MS 1000u
#define STATES_FRAME_CAP 64u
#define STATES_ADVANCE 8u
#define STATES_REWIND_ACTIONS 3u
#define STATES_REWIND_WINDOW_MS 100u
#define STATES_MAX_REFRESH_HZ 120u
#define STATES_STARTUP_FRAME_RESERVE 32u
#define STATES_HOST_FRAME_CAP 120u
#define STATES_CAPTURE_CAP 32u
#define STATES_MEDIA_BYTES 2048u
#define STATES_HISTORY_ADVANCES STATES_ADVANCE
#define STATES_BASELINE_ADVANCES STATES_ADVANCE
#define STATES_RUNAHEAD_ADVANCES STATES_ADVANCE
#define STATES_FINAL_ADVANCES 1u
#define STATES_TOTAL_EXPLICIT_ADVANCES \
    (2u * STATES_ADVANCE + STATES_HISTORY_ADVANCES + STATES_BASELINE_ADVANCES + \
     STATES_RUNAHEAD_ADVANCES + STATES_FINAL_ADVANCES)
#define STATES_REWIND_TICK_BUDGET \
    ((STATES_REWIND_WINDOW_MS * STATES_MAX_REFRESH_HZ + 999u) / 1000u)
#define STATES_PRESENTED_FRAME_BUDGET \
    (STATES_TOTAL_EXPLICIT_ADVANCES + STATES_REWIND_ACTIONS * STATES_REWIND_TICK_BUDGET + \
     STATES_STARTUP_FRAME_RESERVE)

_Static_assert(STATES_TOTAL_EXPLICIT_ADVANCES <= STATES_FRAME_CAP,
               "complete state sequence must fit the explicit frame-advance cap");
_Static_assert(STATES_PRESENTED_FRAME_BUDGET <= STATES_HOST_FRAME_CAP,
               "complete state sequence and bounded startup must fit the host frame cap");

typedef struct states_ops {
    void *user;
    uint64_t start_ms;
    uint64_t (*now_ms)(void *user);
    int (*preflight)(void *user, unsigned timeout_ms);
    int (*verify_identity)(void *user, unsigned timeout_ms);
    int (*read_progress)(void *user, unsigned *progress, unsigned timeout_ms);
    int (*state_absent)(void *user, int absent, unsigned timeout_ms);
    int (*save_once)(void *user);
    int (*state_created)(void *user, unsigned timeout_ms);
    int (*frame_once)(void *user);
    int (*load_once)(void *user);
    int (*capture)(void *user, unsigned expected, unsigned picture_id, int baseline,
                   unsigned timeout_ms);
    int (*status)(void *user, unsigned timeout_ms, int *paused);
    int (*pause_toggle_once)(void *user);
    int (*rewind_once)(void *user);
    int (*runahead_toggle_once)(void *user);
    int (*durable_sentinel)(void *user);
    int (*save_files_once)(void *user, unsigned timeout_ms);
    int (*durable_committed)(void *user, unsigned expected);
    int (*cleanup)(void *user, unsigned timeout_ms);
} states_ops;

typedef struct states_result {
    unsigned frames;
    unsigned startup_progress;
    unsigned baseline_frames;
    unsigned runahead_frames;
    unsigned rewind_reversals;
    unsigned rewind_progress[STATES_REWIND_ACTIONS];
    uint64_t elapsed_ms;
    uint64_t baseline_elapsed_ms;
    uint64_t runahead_elapsed_ms;
    const char *error;
} states_result;

enum states_identity_result {
    STATES_IDENTITY_OK = 0,
    STATES_IDENTITY_VERSION = 1,
    STATES_IDENTITY_CORE = 2,
    STATES_IDENTITY_CONTENT = 3,
    STATES_IDENTITY_CRC = 4,
    STATES_IDENTITY_STATUS = 5,
};

enum states_progress_result {
    STATES_PROGRESS_OK = 0,
    STATES_PROGRESS_PROTOCOL = 1,
    STATES_PROGRESS_MARKER = 2,
    STATES_PROGRESS_COUNTER = 3,
};

static uint64_t states_now_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return 0u;
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u;
}

static unsigned states_timeout(states_ops *ops, uint64_t deadline)
{
    uint64_t now = ops->now_ms(ops->user);
    if (now >= deadline)
        return 0u;
    uint64_t remaining = deadline - now;
    return remaining < STATES_OBSERVE_MS ? (unsigned)remaining : STATES_OBSERVE_MS;
}

static int states_fail(states_result *result, const char *message)
{
    result->error = message;
    return 1;
}

static int states_ascii_equal(const char *left, size_t left_len, const char *right)
{
    size_t right_len = strlen(right);
    if (left_len != right_len)
        return 0;
    for (size_t i = 0u; i < left_len; i++) {
        char a = left[i], b = right[i];
        if (a >= 'A' && a <= 'Z')
            a = (char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z')
            b = (char)(b + ('a' - 'A'));
        if (a != b)
            return 0;
    }
    return 1;
}

static int states_next_token(const char **cursor, const char **token, size_t *token_len)
{
    while (**cursor == ' ' || **cursor == '\t' || **cursor == '\r' || **cursor == '\n')
        (*cursor)++;
    if (**cursor == '\0')
        return 0;
    *token = *cursor;
    while (**cursor != '\0' && **cursor != ' ' && **cursor != '\t' && **cursor != '\r' &&
           **cursor != '\n')
        (*cursor)++;
    *token_len = (size_t)(*cursor - *token);
    return 1;
}

static int states_parse_version(const char *reply)
{
    return strcmp(reply, "1.22.2") == 0 ? STATES_IDENTITY_OK : STATES_IDENTITY_VERSION;
}

static int states_parse_status(const char *reply, uint32_t expected_crc, int *paused)
{
    static const char prefix[] = "GET_STATUS ";
    if (strncmp(reply, prefix, sizeof prefix - 1u) != 0)
        return STATES_IDENTITY_STATUS;
    const char *body = reply + sizeof prefix - 1u;
    const char *space = strchr(body, ' ');
    if (space == NULL)
        return STATES_IDENTITY_STATUS;
    size_t state_len = (size_t)(space - body);
    if (states_ascii_equal(body, state_len, "PAUSED"))
        *paused = 1;
    else if (states_ascii_equal(body, state_len, "PLAYING"))
        *paused = 0;
    else
        return STATES_IDENTITY_STATUS;
    body = space + 1;
    const char *comma = strchr(body, ',');
    if (comma == NULL)
        return STATES_IDENTITY_STATUS;
    if (!states_ascii_equal(body, (size_t)(comma - body), "atari_7800"))
        return STATES_IDENTITY_CORE;
    const char *name = comma + 1;
    const char *next = strchr(name, ',');
    if (next == NULL)
        return STATES_IDENTITY_STATUS;
    if ((size_t)(next - name) != strlen("frontend-states.a78") ||
        memcmp(name, "frontend-states.a78", strlen("frontend-states.a78")) != 0)
        return STATES_IDENTITY_CONTENT;
    const char *crc = next + 1;
    if (strncmp(crc, "crc32=", 6u) != 0 || strlen(crc + 6) != 8u)
        return STATES_IDENTITY_STATUS;
    uint32_t actual = 0u;
    for (unsigned i = 0u; i < 8u; i++) {
        char c = crc[6u + i];
        unsigned digit;
        if (c >= '0' && c <= '9')
            digit = (unsigned)(c - '0');
        else if (c >= 'a' && c <= 'f')
            digit = (unsigned)(c - 'a') + 10u;
        else if (c >= 'A' && c <= 'F')
            digit = (unsigned)(c - 'A') + 10u;
        else
            return STATES_IDENTITY_STATUS;
        actual = (actual << 4) | digit;
    }
    return actual == expected_crc ? STATES_IDENTITY_OK : STATES_IDENTITY_CRC;
}

static int states_hex_byte(const char *token, size_t token_len, unsigned *value)
{
    if (token_len != 2u)
        return 1;
    unsigned result = 0u;
    for (size_t i = 0u; i < token_len; i++) {
        char c = token[i];
        unsigned digit;
        if (c >= '0' && c <= '9')
            digit = (unsigned)(c - '0');
        else if (c >= 'a' && c <= 'f')
            digit = (unsigned)(c - 'a') + 10u;
        else if (c >= 'A' && c <= 'F')
            digit = (unsigned)(c - 'A') + 10u;
        else
            return 1;
        result = (result << 4) | digit;
    }
    *value = result;
    return 0;
}

static int states_parse_progress(const char *reply, unsigned *progress)
{
    const char *cursor = reply, *token = NULL;
    size_t length = 0u;
    if (!states_next_token(&cursor, &token, &length) ||
        !states_ascii_equal(token, length, "READ_CORE_MEMORY"))
        return STATES_PROGRESS_PROTOCOL;
    if (!states_next_token(&cursor, &token, &length) || length != 4u ||
        !states_ascii_equal(token, length, "2300"))
        return STATES_PROGRESS_PROTOCOL;
    unsigned bytes[6];
    for (size_t i = 0u; i < 6u; i++) {
        if (!states_next_token(&cursor, &token, &length))
            return STATES_PROGRESS_PROTOCOL;
        if (states_hex_byte(token, length, &bytes[i]) != 0)
            return STATES_PROGRESS_COUNTER;
    }
    if (states_next_token(&cursor, &token, &length))
        return STATES_PROGRESS_PROTOCOL;
    static const unsigned marker[4] = {0x4du, 0x55u, 0x48u, 0x54u};
    for (size_t i = 0u; i < 4u; i++)
        if (bytes[i] != marker[i])
            return STATES_PROGRESS_MARKER;
    *progress = bytes[4] | (bytes[5] << 8);
    return STATES_PROGRESS_OK;
}

static int states_parse_save_files_reply(const char *reply)
{
    return strcmp(reply, "OK") == 0 ? 0 : 1;
}

static const char *states_identity_error(int result)
{
    switch (result) {
    case STATES_IDENTITY_VERSION:
        return "RetroArch VERSION reply was not 1.22.2";
    case STATES_IDENTITY_CORE:
        return "GET_STATUS identified the wrong core system_id";
    case STATES_IDENTITY_CONTENT:
        return "GET_STATUS identified the wrong fixture basename";
    case STATES_IDENTITY_CRC:
        return "GET_STATUS fixture CRC did not match the admitted ROM";
    case STATES_IDENTITY_STATUS:
        return "GET_STATUS reply was missing or malformed";
    default:
        return "frontend process identity could not be verified";
    }
}

static int states_read_exact(states_ops *ops, unsigned expected, uint64_t deadline,
                             states_result *result)
{
    uint64_t observation_deadline = ops->now_ms(ops->user) + STATES_OBSERVE_MS;
    if (observation_deadline > deadline)
        observation_deadline = deadline;
    for (;;) {
        unsigned timeout = states_timeout(ops, observation_deadline);
        if (timeout == 0u)
            return states_fail(result, "progress observation timed out");
        unsigned actual = 0u;
        if (ops->read_progress(ops->user, &actual, timeout) != 0)
            return states_fail(result, "progress observation protocol failed");
        if (actual == expected)
            return 0;
        if (actual != ((expected - 1u) & 0xffffu))
            return states_fail(result, "progress observation was stale or skipped");
    }
}

static int states_advance_to(states_ops *ops, unsigned *frames, unsigned expected,
                             uint64_t deadline, states_result *result)
{
    if (*frames >= STATES_FRAME_CAP)
        return states_fail(result, "explicit FRAMEADVANCE cap exceeded");
    if (states_timeout(ops, deadline) == 0u)
        return states_fail(result, "work deadline exhausted before FRAMEADVANCE");
    if (ops->frame_once(ops->user) != 0)
        return states_fail(result, "FRAMEADVANCE send failed");
    (*frames)++;
    return states_read_exact(ops, expected, deadline, result);
}

static int states_capture(states_ops *ops, unsigned expected, unsigned picture_id, int baseline,
                          uint64_t deadline, states_result *result)
{
    unsigned timeout = states_timeout(ops, deadline);
    if (timeout == 0u)
        return states_fail(result, "work deadline exhausted before picture observation");
    if (ops->capture(ops->user, expected, picture_id, baseline, timeout) != 0)
        return states_fail(result, "fresh picture or independent pixel judgment failed");
    return 0;
}

static int states_wait_status(states_ops *ops, int expected_paused, uint64_t deadline,
                              states_result *result)
{
    uint64_t observation_deadline = ops->now_ms(ops->user) + STATES_OBSERVE_MS;
    if (observation_deadline > deadline)
        observation_deadline = deadline;
    for (;;) {
        unsigned timeout = states_timeout(ops, observation_deadline);
        if (timeout == 0u)
            return states_fail(result, "GET_STATUS did not confirm the requested pause state");
        int paused = -1;
        if (ops->status(ops->user, timeout, &paused) != 0)
            return states_fail(result, "GET_STATUS failed while observing a control transition");
        if (paused == expected_paused)
            return 0;
    }
}

static int states_restore_saved(states_ops *ops, unsigned saved, unsigned current,
                                uint64_t deadline, states_result *result)
{
    if (states_wait_status(ops, 1, deadline, result) != 0)
        return 1;
    if (ops->load_once(ops->user) != 0)
        return states_fail(result, "LOAD_STATE send failed before matched workload");
    uint64_t observation_deadline = ops->now_ms(ops->user) + STATES_OBSERVE_MS;
    if (observation_deadline > deadline)
        observation_deadline = deadline;
    for (;;) {
        unsigned timeout = states_timeout(ops, observation_deadline);
        if (timeout == 0u)
            return states_fail(result, "LOAD_STATE did not restore the common workload start");
        unsigned actual = 0u;
        if (ops->read_progress(ops->user, &actual, timeout) != 0)
            return states_fail(result, "restored workload progress observation failed");
        if (actual == saved)
            break;
        if (actual != current)
            return states_fail(result, "LOAD_STATE produced neither the prior nor saved workload value");
    }
    return states_wait_status(ops, 1, deadline, result);
}

static int states_rewind_once(states_ops *ops, unsigned previous, unsigned *restored,
                              uint64_t deadline, states_result *result)
{
    if (previous == 0u)
        return states_fail(result, "rewind progress cannot be judged without unsigned wrap");
    if (states_wait_status(ops, 1, deadline, result) != 0)
        return 1;
    if (ops->pause_toggle_once(ops->user) != 0 ||
        states_wait_status(ops, 0, deadline, result) != 0)
        return states_fail(result, "rewind did not enter a GET_STATUS-confirmed running tick");
    if (ops->rewind_once(ops->user) != 0)
        return states_fail(result, "REWIND send failed");

    uint64_t rewind_deadline = ops->now_ms(ops->user) + STATES_REWIND_WINDOW_MS;
    if (rewind_deadline > deadline)
        rewind_deadline = deadline;
    int reversed = 0;
    while (ops->now_ms(ops->user) < rewind_deadline) {
        unsigned timeout = states_timeout(ops, rewind_deadline);
        if (timeout == 0u)
            break;
        unsigned actual = 0u;
        if (ops->read_progress(ops->user, &actual, timeout) != 0)
            return states_fail(result, "rewind progress observation failed");
        if (actual < previous) {
            reversed = 1;
            break;
        }
    }
    if (!reversed)
        return states_fail(result, "REWIND did not reverse fixture progress");
    if (ops->pause_toggle_once(ops->user) != 0 ||
        states_wait_status(ops, 1, deadline, result) != 0)
        return states_fail(result, "rewind tick did not return to a confirmed pause");
    if (ops->read_progress(ops->user, restored, states_timeout(ops, deadline)) != 0 ||
        *restored >= previous)
        return states_fail(result, "paused rewind progress was not a strict no-wrap reversal");
    return 0;
}

static int states_measure_workload(states_ops *ops, unsigned saved, int runahead,
                                  unsigned *frames, uint64_t deadline,
                                  states_result *result, uint64_t *duration)
{
    uint64_t start = ops->now_ms(ops->user);
    unsigned current = 0u;
    if (ops->read_progress(ops->user, &current, states_timeout(ops, deadline)) != 0)
        return states_fail(result, "prior workload progress could not be observed");
    if (states_restore_saved(ops, saved, current, deadline, result) != 0)
        return 1;
    if (runahead && ops->runahead_toggle_once(ops->user) != 0)
        return states_fail(result, "RUNAHEAD_TOGGLE enable send failed");

    for (unsigned step = 1u; step <= STATES_ADVANCE; step++) {
        unsigned committed = saved + step;
        if (states_advance_to(ops, frames, committed, deadline, result) != 0)
            return 1;
        if (states_wait_status(ops, 1, deadline, result) != 0)
            return states_fail(result, "matched workload did not complete a paused committed frame");
        if (runahead) {
            if (committed == UINT16_MAX)
                return states_fail(result, "one-frame presentation would wrap the fixture counter");
            if (ops->durable_sentinel(ops->user) != 0)
                return states_fail(result, "speculative progress reached durable HSC bytes");
            if (states_capture(ops, committed + 1u, 3u, 0, deadline, result) != 0)
                return states_fail(result,
                                   "one-frame run-ahead presentation did not match committed memory");
        } else if (states_capture(ops, committed, 3u, step == 1u, deadline, result) != 0) {
            return 1;
        }
    }
    uint64_t end = ops->now_ms(ops->user);
    if (end < start)
        return states_fail(result, "matched workload clock moved backwards");
    *duration = end - start;
    return 0;
}

static int states_rewind_runahead(states_ops *ops, unsigned saved, unsigned *frames,
                                  uint64_t deadline, states_result *result)
{
    if (saved > STATES_STARTUP_FRAME_RESERVE ||
        saved > UINT16_MAX - (STATES_ADVANCE + STATES_HISTORY_ADVANCES))
        return states_fail(result, "fixture start exceeds the no-wrap presented-frame budget");
    unsigned current = saved + STATES_ADVANCE;
    for (unsigned step = 1u; step <= STATES_HISTORY_ADVANCES; step++) {
        current = saved + STATES_ADVANCE + step;
        if (states_advance_to(ops, frames, current, deadline, result) != 0)
            return 1;
    }
    if (states_capture(ops, current, 2u, 0, deadline, result) != 0)
        return 1;

    for (unsigned i = 0u; i < STATES_REWIND_ACTIONS; i++) {
        unsigned restored = 0u;
        if (states_rewind_once(ops, current, &restored, deadline, result) != 0)
            return 1;
        if (states_capture(ops, restored, 2u, 0, deadline, result) != 0)
            return 1;
        current = restored;
        result->rewind_progress[result->rewind_reversals] = restored;
        result->rewind_reversals++;
    }

    if (states_measure_workload(ops, saved, 0, frames, deadline, result,
                                &result->baseline_elapsed_ms) != 0)
        return 1;
    result->baseline_frames = STATES_BASELINE_ADVANCES;
    if (ops->durable_sentinel(ops->user) != 0)
        return states_fail(result, "durable HSC sentinel changed before speculative workload");
    if (states_measure_workload(ops, saved, 1, frames, deadline, result,
                                &result->runahead_elapsed_ms) != 0)
        return 1;
    result->runahead_frames = STATES_RUNAHEAD_ADVANCES;
    if (ops->runahead_toggle_once(ops->user) != 0)
        return states_fail(result, "RUNAHEAD_TOGGLE disable send failed");
    unsigned committed = saved + STATES_RUNAHEAD_ADVANCES + STATES_FINAL_ADVANCES;
    if (states_advance_to(ops, frames, committed, deadline, result) != 0 ||
        states_wait_status(ops, 1, deadline, result) != 0 ||
        states_capture(ops, committed, 3u, 0, deadline, result) != 0)
        return states_fail(result, "disabled run-ahead presentation was not accounted for");
    if (ops->durable_sentinel(ops->user) != 0)
        return states_fail(result, "durable HSC sentinel changed before explicit SAVE_FILES");
    if (ops->save_files_once(ops->user, states_timeout(ops, deadline)) != 0)
        return states_fail(result, "SAVE_FILES did not return exact OK");
    if (ops->durable_committed(ops->user, committed) != 0)
        return states_fail(result, "complete durable HSC image did not match committed progress");
    return 0;
}

/* Only state changes are sent here; callers observe their effects separately. */
static int states_save_load(states_ops *ops, states_result *result)
{
    memset(result, 0, sizeof *result);
    uint64_t start = ops->start_ms != 0u ? ops->start_ms : ops->now_ms(ops->user);
    uint64_t work_deadline = start + STATES_WORK_MS;
    uint64_t cleanup_deadline = start + STATES_TOTAL_MS;
    int failed = 0;
    unsigned saved = 0u;
    unsigned frames = 0u;

    if (ops->preflight(ops->user, states_timeout(ops, work_deadline)) != 0) {
        failed = states_fail(result, "sandbox and child ownership preflight failed");
        goto cleanup;
    }
    int identity = ops->verify_identity(ops->user, states_timeout(ops, work_deadline));
    if (identity != STATES_IDENTITY_OK) {
        failed = states_fail(result, states_identity_error(identity));
        goto cleanup;
    }
    if (ops->read_progress(ops->user, &saved, states_timeout(ops, work_deadline)) != 0) {
        failed = states_fail(result, "initial fixture progress could not be read");
        goto cleanup;
    }
    result->startup_progress = saved;
    if (ops->state_absent(ops->user, 1, states_timeout(ops, work_deadline)) != 0) {
        failed = states_fail(result, "state slot was not initially absent");
        goto cleanup;
    }
    if (ops->save_once(ops->user) != 0) {
        failed = states_fail(result, "SAVE_STATE send failed");
        goto cleanup;
    }
    if (ops->state_created(ops->user, states_timeout(ops, work_deadline)) != 0) {
        failed = states_fail(result, "fresh nonempty slot-zero state was not observed");
        goto cleanup;
    }

    for (unsigned step = 1u; step <= STATES_ADVANCE; step++) {
        unsigned expected = (saved + step) & 0xffffu;
        if (states_advance_to(ops, &frames, expected, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
        if (step == 1u && states_capture(ops, expected, 1u, 1, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
        if (step == STATES_ADVANCE &&
            states_capture(ops, expected, 2u, 1, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
    }

    if (ops->load_once(ops->user) != 0) {
        failed = states_fail(result, "LOAD_STATE send failed");
        goto cleanup;
    }
    {
        uint64_t restore_deadline = ops->now_ms(ops->user) + STATES_OBSERVE_MS;
        if (restore_deadline > work_deadline)
            restore_deadline = work_deadline;
        for (;;) {
            unsigned timeout = states_timeout(ops, restore_deadline);
            if (timeout == 0u) {
                failed = states_fail(result, "LOAD_STATE did not restore saved progress");
                goto cleanup;
            }
            unsigned actual = 0u;
            if (ops->read_progress(ops->user, &actual, timeout) != 0) {
                failed = states_fail(result, "restored progress observation failed");
                goto cleanup;
            }
            if (actual == saved)
                break;
            if (actual != ((saved + STATES_ADVANCE) & 0xffffu)) {
                failed = states_fail(result, "LOAD_STATE progress was neither saved nor current");
                goto cleanup;
            }
        }
    }

    for (unsigned step = 1u; step <= STATES_ADVANCE; step++) {
        unsigned expected = (saved + step) & 0xffffu;
        if (states_advance_to(ops, &frames, expected, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
        if (step == 1u && states_capture(ops, expected, 1u, 0, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
        if (step == STATES_ADVANCE &&
            states_capture(ops, expected, 2u, 0, work_deadline, result) != 0) {
            failed = 1;
            goto cleanup;
        }
    }
    if (states_rewind_runahead(ops, saved, &frames, work_deadline, result) != 0)
        failed = 1;

cleanup:
    {
        uint64_t now = ops->now_ms(ops->user);
        unsigned cleanup_ms = now >= cleanup_deadline ? 0u :
                              (cleanup_deadline - now < STATES_CLEANUP_MS
                                   ? (unsigned)(cleanup_deadline - now)
                                   : STATES_CLEANUP_MS);
        if (ops->cleanup(ops->user, cleanup_ms) != 0 && !failed)
            failed = states_fail(result, "owned child cleanup or reap failed");
    }
    result->frames = frames;
    result->elapsed_ms = ops->now_ms(ops->user) - start;
    return failed ? 1 : 0;
}

enum mock_fault {
    MOCK_FAULT_NONE = 0,
    MOCK_FAULT_NO_CONTAINMENT,
    MOCK_FAULT_VERSION,
    MOCK_FAULT_CORE_ID,
    MOCK_FAULT_CONTENT,
    MOCK_FAULT_CRC,
    MOCK_FAULT_SLOT_PRESENT,
    MOCK_FAULT_SAVE_SILENCE,
    MOCK_FAULT_SAVE_UNCHANGED,
    MOCK_FAULT_FRAME_SILENCE,
    MOCK_FAULT_DUPLICATE_PROGRESS,
    MOCK_FAULT_OUT_OF_ORDER_PROGRESS,
    MOCK_FAULT_PROGRESS_SILENCE,
    MOCK_FAULT_LOAD_SILENCE,
    MOCK_FAULT_LOAD_UNCHANGED,
    MOCK_FAULT_STALE_CAPTURE,
    MOCK_FAULT_INTERRUPTED_PROGRESS,
    MOCK_FAULT_REWIND_IGNORED,
    MOCK_FAULT_RUNAHEAD_TOGGLE_IGNORED,
    MOCK_FAULT_AHEAD_MEDIA,
    MOCK_FAULT_SAVE_FILES_NO,
    MOCK_FAULT_SAVE_FILES_SILENT,
    MOCK_FAULT_WRONG_REPLAY,
    MOCK_FAULT_INCOMPLETE_WORK,
};

typedef struct mock_session {
    uint64_t now;
    unsigned progress;
    unsigned saved;
    unsigned presented;
    unsigned picture_generation;
    unsigned pictures[4];
    uint8_t durable[STATES_MEDIA_BYTES];
    unsigned frames;
    unsigned progress_reads;
    unsigned save_calls;
    unsigned load_calls;
    unsigned frame_calls;
    unsigned capture_calls;
    unsigned rewind_calls;
    unsigned pause_calls;
    unsigned runahead_toggle_calls;
    unsigned save_files_calls;
    unsigned baseline_frames;
    unsigned runahead_frames;
    unsigned explicit_advances;
    enum mock_fault fault;
    int paused;
    int runahead_enabled;
    int child_owned;
    int child_hangs;
    int state_exists;
    int child_reaped;
#if STATES_POSIX
    pid_t child_pid;
    int child_stdin;
    char self_path[PATH_MAX];
#endif
} mock_session;

#if !STATES_POSIX
typedef struct protected_fixture {
    int unused;
} protected_fixture;
#endif

static uint64_t mock_now(void *user)
{
    mock_session *mock = user;
    return mock->now;
}

static int mock_preflight(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->now += 1u;
    return mock->child_owned && mock->fault != MOCK_FAULT_NO_CONTAINMENT ? 0 : 1;
}

static int mock_identity(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->now += 1u;
    switch (mock->fault) {
    case MOCK_FAULT_VERSION:
        return STATES_IDENTITY_VERSION;
    case MOCK_FAULT_CORE_ID:
        return STATES_IDENTITY_CORE;
    case MOCK_FAULT_CONTENT:
        return STATES_IDENTITY_CONTENT;
    case MOCK_FAULT_CRC:
        return STATES_IDENTITY_CRC;
    default:
        break;
    }
    return mock->child_owned ? 0 : 1;
}

static int mock_progress(void *user, unsigned *progress, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->now += 1u;
    mock->progress_reads++;
    if (mock->fault == MOCK_FAULT_PROGRESS_SILENCE && mock->frame_calls == 0u)
        return 1;
    if (mock->fault == MOCK_FAULT_INTERRUPTED_PROGRESS && mock->frame_calls != 0u)
        return 1;
    if (mock->fault == MOCK_FAULT_DUPLICATE_PROGRESS && mock->frame_calls != 0u) {
        *progress = mock->saved;
        return 0;
    }
    if (mock->fault == MOCK_FAULT_OUT_OF_ORDER_PROGRESS && mock->frame_calls != 0u) {
        *progress = (mock->progress + 1u) & 0xffffu;
        return 0;
    }
    *progress = mock->progress;
    return 0;
}

static int mock_state_absent(void *user, int absent, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->now += 1u;
    if (absent && mock->fault == MOCK_FAULT_SLOT_PRESENT)
        mock->state_exists = 1;
    return absent ? mock->state_exists : !mock->state_exists;
}

static int mock_save(void *user)
{
    mock_session *mock = user;
    mock->save_calls++;
    mock->saved = mock->progress;
    if (mock->fault == MOCK_FAULT_SAVE_UNCHANGED) {
        mock->now += 1u;
        return 0;
    }
    mock->state_exists = 1;
    mock->now += 1u;
    return mock->fault == MOCK_FAULT_SAVE_SILENCE ? 1 : 0;
}

static int mock_state_created(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->now += 1u;
    return mock->state_exists ? 0 : 1;
}

static int mock_frame(void *user)
{
    mock_session *mock = user;
    mock->frame_calls++;
    mock->explicit_advances++;
    if (mock->fault == MOCK_FAULT_FRAME_SILENCE ||
        (mock->fault == MOCK_FAULT_INCOMPLETE_WORK && mock->frame_calls >= 25u)) {
        mock->now += 1u;
        return 1;
    }
    mock->progress = (mock->progress + 1u) & 0xffffu;
    mock->presented = mock->runahead_enabled ? (mock->progress + 1u) & 0xffffu : mock->progress;
    if (mock->fault == MOCK_FAULT_OUT_OF_ORDER_PROGRESS)
        mock->progress = (mock->progress + 1u) & 0xffffu;
    if (mock->fault == MOCK_FAULT_AHEAD_MEDIA && mock->runahead_enabled) {
        memset(mock->durable, 0xff, sizeof mock->durable);
        mock->durable[0x100u] = (uint8_t)(mock->presented & 0xffu);
        mock->durable[0x101u] = (uint8_t)((mock->presented >> 8) & 0xffu);
    }
    mock->frames++;
    mock->now += 1u;
    return 0;
}

static int mock_load(void *user)
{
    mock_session *mock = user;
    mock->load_calls++;
    if (mock->fault == MOCK_FAULT_LOAD_UNCHANGED) {
        mock->now += 1u;
        return 0;
    }
    mock->progress = mock->saved;
    mock->presented = mock->progress;
    mock->now += 1u;
    return mock->fault == MOCK_FAULT_LOAD_SILENCE ? 1 : 0;
}

static int mock_capture(void *user, unsigned expected, unsigned picture_id, int baseline,
                        unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->capture_calls++;
    mock->now += 1u;
    if (mock->fault == MOCK_FAULT_STALE_CAPTURE)
        return 1;
    if (mock->presented != expected || picture_id == 0u || picture_id > 3u)
        return 1;
    mock->picture_generation++;
    if (baseline) {
        mock->pictures[picture_id] = 1u;
        return 0;
    }
    if (mock->fault == MOCK_FAULT_WRONG_REPLAY && mock->capture_calls <= 4u)
        return 1;
    return mock->pictures[picture_id] == 1u ? 0 : 1;
}

static void mock_media_set(mock_session *mock, unsigned progress)
{
    memset(mock->durable, 0xff, sizeof mock->durable);
    mock->durable[0x100u] = (uint8_t)(progress & 0xffu);
    mock->durable[0x101u] = (uint8_t)((progress >> 8) & 0xffu);
}

static int mock_status(void *user, unsigned timeout_ms, int *paused)
{
    mock_session *mock = user;
    if (timeout_ms == 0u)
        return 1;
    *paused = mock->paused;
    mock->now += 1u;
    return 0;
}

static int mock_pause_toggle(void *user)
{
    mock_session *mock = user;
    mock->pause_calls++;
    mock->paused = !mock->paused;
    mock->now += 1u;
    return 0;
}

static int mock_rewind(void *user)
{
    mock_session *mock = user;
    mock->rewind_calls++;
    mock->now += 1u;
    if (mock->paused)
        return 1;
    if (mock->fault != MOCK_FAULT_REWIND_IGNORED && mock->progress > 0u) {
        mock->progress--;
        mock->presented = mock->progress;
    }
    return 0;
}

static int mock_runahead_toggle(void *user)
{
    mock_session *mock = user;
    mock->runahead_toggle_calls++;
    mock->now += 1u;
    if (mock->fault != MOCK_FAULT_RUNAHEAD_TOGGLE_IGNORED)
        mock->runahead_enabled = !mock->runahead_enabled;
    return 0;
}

static int mock_durable_sentinel(void *user)
{
    mock_session *mock = user;
    for (size_t i = 0u; i < sizeof mock->durable; i++)
        if (mock->durable[i] != 0xffu)
            return 1;
    return 0;
}

static int mock_save_files(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    mock->save_files_calls++;
    mock->now += 1u;
    if (timeout_ms == 0u || mock->fault == MOCK_FAULT_SAVE_FILES_NO ||
        mock->fault == MOCK_FAULT_SAVE_FILES_SILENT)
        return 1;
    mock_media_set(mock, mock->progress);
    return 0;
}

static int mock_durable_committed(void *user, unsigned expected)
{
    mock_session *mock = user;
    for (size_t i = 0u; i < sizeof mock->durable; i++) {
        uint8_t wanted = 0xffu;
        if (i == 0x100u)
            wanted = (uint8_t)(expected & 0xffu);
        else if (i == 0x101u)
            wanted = (uint8_t)((expected >> 8) & 0xffu);
        if (mock->durable[i] != wanted)
            return 1;
    }
    return 0;
}

#if STATES_POSIX
static int mock_child_main(int hung)
{
    if (hung) {
        for (;;)
            (void)pause();
    }
    char command = 0;
    while (read(STDIN_FILENO, &command, 1u) == 1) {
        if (command == 'Q')
            return 0;
    }
    return 1;
}

static int process_path(pid_t pid, char *path, size_t capacity)
{
#if defined(__APPLE__)
    if (proc_pidpath(pid, path, (uint32_t)capacity) <= 0)
        return 1;
    return 0;
#else
    char link[64];
    int n = snprintf(link, sizeof link, "/proc/%ld/exe", (long)pid);
    if (n < 0 || (size_t)n >= sizeof link)
        return 1;
    ssize_t got = readlink(link, path, capacity - 1u);
    if (got <= 0 || (size_t)got >= capacity)
        return 1;
    path[got] = '\0';
    return 0;
#endif
}

typedef struct protected_fixture {
    pid_t child;
    char directory[PATH_MAX];
    char sentinel[PATH_MAX];
} protected_fixture;

static int wait_child(pid_t pid, unsigned timeout_ms, int *status);

static int protected_setup(protected_fixture *fixture, const char *self_path)
{
    char directory[] = "/tmp/muhtari-states-controls-XXXXXX";
    char *made = mkdtemp(directory);
    if (made == NULL || strlen(made) >= sizeof fixture->directory)
        return 1;
    memcpy(fixture->directory, made, strlen(made) + 1u);
    int n = snprintf(fixture->sentinel, sizeof fixture->sentinel, "%s/save.sentinel",
                     fixture->directory);
    if (n < 0 || (size_t)n >= sizeof fixture->sentinel)
        return 1;
    FILE *file = fopen(fixture->sentinel, "wb");
    if (file == NULL)
        return 1;
    int wrote = fputs("keep\n", file);
    if (fclose(file) != 0 || wrote < 0)
        return 1;
    pid_t child = fork();
    if (child < 0)
        return 1;
    if (child == 0) {
        execl(self_path, self_path, "--unrelated-child", (char *)NULL);
        _exit(127);
    }
    fixture->child = child;
    return 0;
}

static int protected_unchanged(const protected_fixture *fixture, const char *self_path)
{
    int status = 0;
    if (waitpid(fixture->child, &status, WNOHANG) != 0)
        return 0;
    char actual[PATH_MAX];
    if (process_path(fixture->child, actual, sizeof actual) != 0 ||
        strcmp(actual, self_path) != 0)
        return 0;
    FILE *file = fopen(fixture->sentinel, "rb");
    if (file == NULL)
        return 0;
    char content[16] = {0};
    size_t n = fread(content, 1u, sizeof content - 1u, file);
    int extra = fgetc(file);
    int read_failed = ferror(file);
    int close_failed = fclose(file) != 0;
    int failed = read_failed || close_failed;
    return !failed && n == 5u && extra == EOF && strcmp(content, "keep\n") == 0;
}

static int protected_destroy(protected_fixture *fixture, const char *self_path)
{
    int rc = 0;
    int status = 0;
    pid_t waited = waitpid(fixture->child, &status, WNOHANG);
    if (waited == 0) {
        char actual[PATH_MAX];
        if (process_path(fixture->child, actual, sizeof actual) != 0 ||
            strcmp(actual, self_path) != 0 || kill(fixture->child, SIGTERM) != 0 ||
            wait_child(fixture->child, 1000u, &status) != 0)
            rc = 1;
    } else if (waited != fixture->child) {
        rc = 1;
    }
    if (fixture->sentinel[0] != '\0' && unlink(fixture->sentinel) != 0)
        rc = 1;
    if (fixture->directory[0] != '\0' && rmdir(fixture->directory) != 0)
        rc = 1;
    return rc;
}

static int spawn_mock_child(mock_session *mock)
{
    int pipefd[2];
    if (pipe(pipefd) != 0)
        return 1;
    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    if (pid == 0) {
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) < 0)
            _exit(126);
        close(pipefd[0]);
        execl(mock->self_path, mock->self_path,
              mock->child_hangs ? "--mock-child-hung" : "--mock-child", (char *)NULL);
        _exit(127);
    }
    close(pipefd[0]);
    mock->child_pid = pid;
    mock->child_stdin = pipefd[1];
    mock->child_owned = 1;
    return 0;
}

static int child_is_ours(mock_session *mock)
{
    char actual[PATH_MAX];
    return mock->child_pid > 0 && process_path(mock->child_pid, actual, sizeof actual) == 0 &&
           strcmp(actual, mock->self_path) == 0;
}

static int wait_child(pid_t pid, unsigned timeout_ms, int *status)
{
    uint64_t deadline = states_now_ms() + timeout_ms;
    for (;;) {
        pid_t waited = waitpid(pid, status, WNOHANG);
        if (waited == pid)
            return 0;
        if (waited < 0 && errno != EINTR)
            return 1;
        if (states_now_ms() >= deadline)
            return 1;
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
        (void)nanosleep(&delay, NULL);
    }
}

static int mock_cleanup(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    if (!mock->child_owned)
        return 0;
    if (!child_is_ours(mock))
        return 1;
    if (write(mock->child_stdin, "Q", 1u) != 1)
        return 1;
    int status = 0;
    uint64_t deadline = states_now_ms() + timeout_ms;
    unsigned graceful = timeout_ms < 50u ? timeout_ms : 50u;
    if (wait_child(mock->child_pid, graceful, &status) != 0) {
        uint64_t now = states_now_ms();
        unsigned remaining = now >= deadline ? 0u : (unsigned)(deadline - now);
        unsigned term_wait = remaining < 50u ? remaining : 50u;
        if (!child_is_ours(mock) || kill(mock->child_pid, SIGTERM) != 0)
            return 1;
        if (wait_child(mock->child_pid, term_wait, &status) != 0) {
            now = states_now_ms();
            remaining = now >= deadline ? 0u : (unsigned)(deadline - now);
            if (!child_is_ours(mock) || kill(mock->child_pid, SIGKILL) != 0 ||
                wait_child(mock->child_pid, remaining, &status) != 0)
                return 1;
        }
    }
    close(mock->child_stdin);
    mock->child_owned = 0;
    mock->child_reaped = (WIFEXITED(status) && WEXITSTATUS(status) == 0) ||
                         (WIFSIGNALED(status) &&
                          (WTERMSIG(status) == SIGTERM || WTERMSIG(status) == SIGKILL));
    return mock->child_reaped ? 0 : 1;
}
#else
static int mock_cleanup(void *user, unsigned timeout_ms)
{
    mock_session *mock = user;
    (void)timeout_ms;
    mock->child_owned = 0;
    mock->child_reaped = 1;
    return 0;
}
#endif

static int run_controls(const char *self_path)
{
    mock_session mock;
    memset(&mock, 0, sizeof mock);
    mock.progress = 12u;
    mock.presented = mock.progress;
    mock.paused = 1;
    memset(mock.durable, 0xff, sizeof mock.durable);
#if STATES_POSIX
    if (strlen(self_path) >= sizeof mock.self_path)
        return 2;
    strcpy(mock.self_path, self_path);
    if (spawn_mock_child(&mock) != 0) {
        puts("TAP version 13\n1..1\nnot ok 1 - owned save load restores progress and judges fresh replay # process-start-failed");
        return 1;
    }
#else
    mock.child_owned = 1;
#endif
    states_ops ops = {
        .user = &mock,
        .now_ms = mock_now,
        .preflight = mock_preflight,
        .verify_identity = mock_identity,
        .read_progress = mock_progress,
        .state_absent = mock_state_absent,
        .save_once = mock_save,
        .state_created = mock_state_created,
        .frame_once = mock_frame,
        .load_once = mock_load,
        .capture = mock_capture,
        .status = mock_status,
        .pause_toggle_once = mock_pause_toggle,
        .rewind_once = mock_rewind,
        .runahead_toggle_once = mock_runahead_toggle,
        .durable_sentinel = mock_durable_sentinel,
        .save_files_once = mock_save_files,
        .durable_committed = mock_durable_committed,
        .cleanup = mock_cleanup,
    };
    states_result result;
    int rc = states_save_load(&ops, &result);
    puts("TAP version 13");
    puts("1..1");
    if (rc == 0 && mock.progress == 21u && mock.frames == STATES_TOTAL_EXPLICIT_ADVANCES &&
        result.rewind_reversals == STATES_REWIND_ACTIONS &&
        result.baseline_frames == STATES_BASELINE_ADVANCES &&
        result.runahead_frames == STATES_RUNAHEAD_ADVANCES && !mock.child_owned &&
        mock.child_reaped) {
        puts("ok 1 - owned state controller proves rewind and single-instance run-ahead");
        printf("# EXPLICIT_ADVANCES=%u PICTURE_OBSERVATIONS=%u REWIND_PROGRESS=%u,%u,%u "
               "BASELINE_MS=%" PRIu64 " RUNAHEAD_MS=%" PRIu64 " CHILD_REAPED=%d ELAPSED_MS=%" PRIu64 "\n",
               result.frames, mock.picture_generation, result.rewind_progress[0],
               result.rewind_progress[1], result.rewind_progress[2],
               result.baseline_elapsed_ms, result.runahead_elapsed_ms, mock.child_reaped,
               result.elapsed_ms);
        return 0;
    }
    printf("not ok 1 - owned state controller proves rewind and single-instance run-ahead\n"
           "  ---\n  error: %s progress=%u frames=%u child_owned=%d reaped=%d\n  ...\n",
           result.error != NULL ? result.error : "valid mock scenario produced wrong result",
           mock.progress, mock.frames, mock.child_owned, mock.child_reaped);
    return 1;
}

typedef struct mock_control_case {
    const char *name;
    enum mock_fault fault;
    const char *expected_error;
    unsigned saves;
    unsigned loads;
    unsigned frame_calls;
    int hang_child;
} mock_control_case;

static states_ops mock_operations(mock_session *mock)
{
    states_ops ops = {
        .user = mock,
        .now_ms = mock_now,
        .preflight = mock_preflight,
        .verify_identity = mock_identity,
        .read_progress = mock_progress,
        .state_absent = mock_state_absent,
        .save_once = mock_save,
        .state_created = mock_state_created,
        .frame_once = mock_frame,
        .load_once = mock_load,
        .capture = mock_capture,
        .status = mock_status,
        .pause_toggle_once = mock_pause_toggle,
        .rewind_once = mock_rewind,
        .runahead_toggle_once = mock_runahead_toggle,
        .durable_sentinel = mock_durable_sentinel,
        .save_files_once = mock_save_files,
        .durable_committed = mock_durable_committed,
        .cleanup = mock_cleanup,
    };
    return ops;
}

static void tap_result(unsigned *count, unsigned *failures, const char *name, int passed,
                       const char *detail)
{
    (*count)++;
    if (passed)
        printf("ok %u - %s\n", *count, name);
    else {
        (*failures)++;
        printf("not ok %u - %s\n  ---\n  error: %s\n  ...\n", *count, name,
               detail != NULL ? detail : "assertion failed");
    }
}

static int run_complete_workload_control(enum mock_fault fault, const char *expected_error,
                                         const char *self_path,
                                         const protected_fixture *protected,
                                         char *detail, size_t detail_capacity)
{
    mock_session mock;
    memset(&mock, 0, sizeof mock);
    mock.progress = 12u;
    mock.presented = mock.progress;
    mock.paused = 1;
    mock.fault = fault;
    memset(mock.durable, 0xff, sizeof mock.durable);
#if STATES_POSIX
    if (strlen(self_path) >= sizeof mock.self_path)
        return 0;
    memcpy(mock.self_path, self_path, strlen(self_path) + 1u);
    if (spawn_mock_child(&mock) != 0)
        return 0;
#else
    mock.child_owned = 1;
#endif
    states_ops ops = mock_operations(&mock);
    states_result result;
    int rc = states_save_load(&ops, &result);
    int passed = !mock.child_owned && mock.child_reaped;
    if (expected_error == NULL) {
        passed = passed && rc == 0 && result.error == NULL && mock.progress == 21u &&
                 mock.frames == 41u && result.frames == 41u &&
                 result.rewind_reversals == STATES_REWIND_ACTIONS && mock.rewind_calls == 3u &&
                 result.rewind_progress[0] < result.startup_progress + 16u &&
                 result.rewind_progress[1] < result.rewind_progress[0] &&
                 result.rewind_progress[2] < result.rewind_progress[1] &&
                 mock.runahead_toggle_calls == 2u && mock.save_files_calls == 1u &&
                 result.baseline_frames == STATES_ADVANCE &&
                 result.runahead_frames == STATES_ADVANCE &&
                 result.baseline_elapsed_ms > 0u && result.runahead_elapsed_ms > 0u &&
                 mock.paused && !mock.runahead_enabled &&
                 mock_durable_committed(&mock, mock.progress) == 0;
    } else {
        passed = passed && rc != 0 && result.error != NULL &&
                 strcmp(result.error, expected_error) == 0;
    }
#if STATES_POSIX
    passed = passed && protected_unchanged(protected, self_path);
#else
    (void)protected;
#endif
    if (!passed && detail != NULL && detail_capacity > 0u)
        (void)snprintf(detail, detail_capacity,
                       "error=%s rc=%d progress=%u frames=%u rewinds=%u toggles=%u flushes=%u baseline=%u/%llu enabled=%u/%llu reaped=%d",
                       result.error != NULL ? result.error : "valid full sequence was not observed",
                       rc, mock.progress, result.frames, mock.rewind_calls,
                       mock.runahead_toggle_calls, mock.save_files_calls,
                       result.baseline_frames, (unsigned long long)result.baseline_elapsed_ms,
                       result.runahead_frames, (unsigned long long)result.runahead_elapsed_ms,
                       mock.child_reaped);
    return passed;
}

static void complete_workload_controls(unsigned *count, unsigned *failures, const char *self_path,
                                       const protected_fixture *protected)
{
    char detail[512];
    memset(detail, 0, sizeof detail);
    int passed = run_complete_workload_control(MOCK_FAULT_NONE, NULL, self_path, protected,
                                               detail, sizeof detail);
    tap_result(count, failures,
               "complete controller observes three rewind reversals and active one-frame run-ahead",
               passed, passed ? NULL : detail);
    static const struct {
        const char *name;
        enum mock_fault fault;
        const char *error;
    } cases[] = {
        {"ignored REWIND action fails observed reversal", MOCK_FAULT_REWIND_IGNORED,
         "REWIND did not reverse fixture progress"},
        {"ignored run-ahead toggle fails the presented-ahead proof",
         MOCK_FAULT_RUNAHEAD_TOGGLE_IGNORED,
         "one-frame run-ahead presentation did not match committed memory"},
        {"speculative HSC bytes cannot replace the flush sentinel", MOCK_FAULT_AHEAD_MEDIA,
         "speculative progress reached durable HSC bytes"},
        {"wrong replay picture is rejected", MOCK_FAULT_WRONG_REPLAY,
         "fresh picture or independent pixel judgment failed"},
        {"incomplete eight-frame workload is rejected", MOCK_FAULT_INCOMPLETE_WORK,
         "FRAMEADVANCE send failed"},
        {"SAVE_FILES NO reply fails before durable judgement", MOCK_FAULT_SAVE_FILES_NO,
         "SAVE_FILES did not return exact OK"},
        {"silent SAVE_FILES fails before durable judgement", MOCK_FAULT_SAVE_FILES_SILENT,
         "SAVE_FILES did not return exact OK"},
    };
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        memset(detail, 0, sizeof detail);
        passed = run_complete_workload_control(cases[i].fault, cases[i].error, self_path,
                                               protected, detail, sizeof detail);
        tap_result(count, failures, cases[i].name, passed, passed ? NULL : detail);
    }
}

static void parser_controls(unsigned *count, unsigned *failures)
{
    static const uint32_t crc = 0x12ab34cdu;
    static const char status[] =
        "GET_STATUS PAUSED atari_7800,frontend-states.a78,crc32=12ab34cd";
    unsigned progress = 0u;
    int paused = 0;
    tap_result(count, failures, "VERSION 1.22.2 is accepted",
               states_parse_version("1.22.2") == STATES_IDENTITY_OK, NULL);
    tap_result(count, failures, "wrong or truncated VERSION is named",
               states_parse_version("1.22") == STATES_IDENTITY_VERSION, NULL);
    tap_result(count, failures, "paused status with exact system, fixture, and CRC is accepted",
               states_parse_status(status, crc, &paused) == STATES_IDENTITY_OK && paused, NULL);
    tap_result(count, failures, "wrong GET_STATUS system_id is rejected by name",
               states_parse_status(
                   "GET_STATUS PAUSED another_core,frontend-states.a78,crc32=12ab34cd", crc,
                   &paused) == STATES_IDENTITY_CORE,
               NULL);
    tap_result(count, failures, "wrong GET_STATUS fixture basename is rejected by name",
               states_parse_status("GET_STATUS PLAYING atari_7800,other.a78,crc32=12ab34cd",
                                   crc, &paused) == STATES_IDENTITY_CONTENT,
               NULL);
    tap_result(count, failures, "wrong GET_STATUS CRC is rejected by name",
               states_parse_status(
                   "GET_STATUS PAUSED atari_7800,frontend-states.a78,crc32=12ab34ce", crc,
                   &paused) == STATES_IDENTITY_CRC,
               NULL);
    tap_result(count, failures, "truncated or trailing GET_STATUS fields are rejected",
               states_parse_status("GET_STATUS PAUSED atari_7800,frontend-states.a78", crc,
                                   &paused) == STATES_IDENTITY_STATUS &&
                   states_parse_status("GET_STATUS PAUSED atari_7800,frontend-states.a78,crc32=12ab34cd,extra",
                                       crc, &paused) == STATES_IDENTITY_STATUS,
               NULL);
    tap_result(count, failures, "READ_CORE_MEMORY marker and little-endian progress are parsed",
               states_parse_progress("READ_CORE_MEMORY 2300 4D 55 48 54 0C 00", &progress) ==
                       STATES_PROGRESS_OK &&
                   progress == 12u,
               NULL);
    tap_result(count, failures, "wrong READ_CORE_MEMORY command or address is rejected",
               states_parse_progress("READ_CORE_RAM 2300 4D 55 48 54 0C 00", &progress) ==
                       STATES_PROGRESS_PROTOCOL &&
                   states_parse_progress("READ_CORE_MEMORY 2301 4D 55 48 54 0C 00", &progress) ==
                       STATES_PROGRESS_PROTOCOL,
               NULL);
    tap_result(count, failures, "wrong fixture marker is rejected",
               states_parse_progress("READ_CORE_MEMORY 2300 4D 55 00 54 0C 00", &progress) ==
                   STATES_PROGRESS_MARKER,
               NULL);
    tap_result(count, failures, "truncated, duplicated, or failed counter replies are rejected",
               states_parse_progress("READ_CORE_MEMORY 2300 4D 55 48 54 0C", &progress) ==
                       STATES_PROGRESS_PROTOCOL &&
                   states_parse_progress("READ_CORE_MEMORY 2300 4D 55 48 54 0C 00 01", &progress) ==
                       STATES_PROGRESS_PROTOCOL &&
                   states_parse_progress("READ_CORE_MEMORY 2300 4D 55 48 54 -1", &progress) ==
                       STATES_PROGRESS_COUNTER,
               NULL);
    tap_result(count, failures, "SAVE_FILES succeeds only on exact OK",
               states_parse_save_files_reply("OK") == 0 &&
                   states_parse_save_files_reply("NO") != 0 &&
                   states_parse_save_files_reply("") != 0 &&
                   states_parse_save_files_reply("OK trailing") != 0,
               NULL);
}

static int run_engine_control(const mock_control_case *test, const char *self_path,
                              const protected_fixture *protected)
{
    mock_session mock;
    memset(&mock, 0, sizeof mock);
    mock.progress = 12u;
    mock.presented = mock.progress;
    mock.paused = 1;
    memset(mock.durable, 0xff, sizeof mock.durable);
    mock.fault = test->fault;
    mock.child_hangs = test->hang_child;
#if STATES_POSIX
    if (strlen(self_path) >= sizeof mock.self_path)
        return 0;
    memcpy(mock.self_path, self_path, strlen(self_path) + 1u);
    if (spawn_mock_child(&mock) != 0)
        return 0;
#else
    mock.child_owned = 1;
#endif
    states_ops ops = mock_operations(&mock);
    states_result result;
    int rc = states_save_load(&ops, &result);
    int passed = !mock.child_owned && mock.child_reaped &&
                 mock.save_calls == test->saves && mock.load_calls == test->loads &&
                 mock.frame_calls == test->frame_calls;
    if (test->expected_error == NULL) {
        passed = passed && rc == 0 && result.error == NULL && mock.progress == 21u &&
                 mock.frames == STATES_TOTAL_EXPLICIT_ADVANCES &&
                 result.frames == STATES_TOTAL_EXPLICIT_ADVANCES &&
                 mock.picture_generation == 25u &&
                 result.rewind_reversals == STATES_REWIND_ACTIONS &&
                 result.rewind_progress[0] < result.startup_progress + 16u &&
                 result.rewind_progress[1] < result.rewind_progress[0] &&
                 result.rewind_progress[2] < result.rewind_progress[1] &&
                 result.baseline_frames == STATES_BASELINE_ADVANCES &&
                 result.runahead_frames == STATES_RUNAHEAD_ADVANCES &&
                 mock.save_files_calls == 1u && mock_durable_committed(&mock, mock.progress) == 0;
    } else {
        passed = passed && rc != 0 && result.error != NULL &&
                 strcmp(result.error, test->expected_error) == 0;
    }
#if STATES_POSIX
    passed = passed && protected_unchanged(protected, self_path);
#else
    (void)protected;
#endif
    return passed;
}

static int run_fault_controls(const char *self_path)
{
    unsigned count = 0u, failures = 0u;
    puts("TAP version 13");
    parser_controls(&count, &failures);
    static const mock_control_case cases[] = {
        {"valid owned save-load, rewind, run-ahead, and flush passes", MOCK_FAULT_NONE, NULL,
         1u, 3u, STATES_TOTAL_EXPLICIT_ADVANCES, 0},
        {"missing containment fails before mutation", MOCK_FAULT_NO_CONTAINMENT,
         "sandbox and child ownership preflight failed", 0u, 0u, 0u, 0},
        {"wrong version has a specific cause", MOCK_FAULT_VERSION,
         "RetroArch VERSION reply was not 1.22.2", 0u, 0u, 0u, 0},
        {"wrong core identity has a specific cause", MOCK_FAULT_CORE_ID,
         "GET_STATUS identified the wrong core system_id", 0u, 0u, 0u, 0},
        {"wrong fixture identity has a specific cause", MOCK_FAULT_CONTENT,
         "GET_STATUS identified the wrong fixture basename", 0u, 0u, 0u, 0},
        {"wrong fixture CRC has a specific cause", MOCK_FAULT_CRC,
         "GET_STATUS fixture CRC did not match the admitted ROM", 0u, 0u, 0u, 0},
        {"preexisting slot-zero state is rejected", MOCK_FAULT_SLOT_PRESENT,
         "state slot was not initially absent", 0u, 0u, 0u, 0},
        {"ambiguous SAVE_STATE is sent once", MOCK_FAULT_SAVE_SILENCE,
         "SAVE_STATE send failed", 1u, 0u, 0u, 0},
        {"unchanged state slot fails creation observation", MOCK_FAULT_SAVE_UNCHANGED,
         "fresh nonempty slot-zero state was not observed", 1u, 0u, 0u, 0},
        {"ambiguous FRAMEADVANCE is sent once", MOCK_FAULT_FRAME_SILENCE,
         "FRAMEADVANCE send failed", 1u, 0u, 1u, 0},
        {"duplicate stale progress times out without another advance", MOCK_FAULT_DUPLICATE_PROGRESS,
         "progress observation timed out", 1u, 0u, 1u, 0},
        {"out-of-order progress is rejected", MOCK_FAULT_OUT_OF_ORDER_PROGRESS,
         "progress observation was stale or skipped", 1u, 0u, 1u, 0},
        {"silent initial memory observation fails closed", MOCK_FAULT_PROGRESS_SILENCE,
         "initial fixture progress could not be read", 0u, 0u, 0u, 0},
        {"ambiguous LOAD_STATE is sent once", MOCK_FAULT_LOAD_SILENCE,
         "LOAD_STATE send failed", 1u, 1u, 8u, 0},
        {"unchanged LOAD_STATE fails restoration observation", MOCK_FAULT_LOAD_UNCHANGED,
         "LOAD_STATE did not restore saved progress", 1u, 1u, 8u, 0},
        {"stale picture capture cannot pass", MOCK_FAULT_STALE_CAPTURE,
         "fresh picture or independent pixel judgment failed", 1u, 0u, 1u, 0},
        {"interrupted observation reaps the hung owned child", MOCK_FAULT_INTERRUPTED_PROGRESS,
         "progress observation protocol failed", 1u, 0u, 1u, 1},
    };
    protected_fixture protected;
    memset(&protected, 0, sizeof protected);
#if STATES_POSIX
    if (protected_setup(&protected, self_path) != 0) {
        tap_result(&count, &failures, "unrelated child and save sentinel setup", 0,
                   "could not create owned protection fixtures");
        printf("1..%u\n", count);
        return 1;
    }
#else
    (void)self_path;
#endif
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        int passed = run_engine_control(&cases[i], self_path, &protected);
        tap_result(&count, &failures, cases[i].name, passed,
                   "sequence cause, single-send count, child reap, or unrelated sentinel check failed");
    }
    complete_workload_controls(&count, &failures, self_path, &protected);
#if STATES_POSIX
    int cleanup_ok = protected_destroy(&protected, self_path) == 0;
    tap_result(&count, &failures, "test-owned unrelated child and save sentinel clean up", cleanup_ok,
               "control fixture cleanup failed");
#endif
    printf("1..%u\n", count);
    return failures == 0u ? 0 : 1;
}

#if defined(__APPLE__)
typedef struct host_session {
    char retroarch[PATH_MAX];
    char core[PATH_MAX];
    char core_sha256[65];
    char info[PATH_MAX];
    char rom[PATH_MAX];
    char ra_dir[PATH_MAX];
    char save_dir[PATH_MAX];
    char output_root[PATH_MAX];
    char compare[PATH_MAX];
    char final_shot[PATH_MAX];
    char log_path[PATH_MAX];
    char profile_path[PATH_MAX];
    char config_path[PATH_MAX];
    char state_path[PATH_MAX];
    char shot_dir[PATH_MAX];
    char media_path[PATH_MAX];
    char captures[4][PATH_MAX];
    char last_capture[PATH_MAX];
    unsigned capture_count;
    unsigned frames;
    uint32_t fixture_crc;
    uint16_t port;
    pid_t child;
    int child_owned;
    int child_reaped;
    int child_status;
    int udp_fd;
    int reserved_fd;
    uint64_t start_ms;
    char child_path[PATH_MAX];
} host_session;

static int copy_path(char *dst, size_t capacity, const char *src)
{
    size_t n = strlen(src);
    if (n == 0u || n >= capacity)
        return 1;
    memcpy(dst, src, n + 1u);
    return 0;
}

static int path_is_within(const char *root, const char *path)
{
    size_t n = strlen(root);
    return strcmp(root, path) == 0 ||
           (strncmp(root, path, n) == 0 && path[n] == '/');
}

static int paths_overlap(const char *a, const char *b)
{
    return path_is_within(a, b) || path_is_within(b, a);
}

static int canonical_path(const char *path, char *out, size_t capacity);

static int directory_empty(const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
        return 0;
    int empty = 1;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            empty = 0;
            break;
        }
    }
    closedir(dir);
    return empty;
}

static int normalize_output_file(const char *path, const char *root, char *out, size_t capacity)
{
    const char *slash = strrchr(path, '/');
    if (slash == NULL || slash[1] == '\0')
        return 1;
    char parent[PATH_MAX];
    size_t parent_len = (size_t)(slash - path);
    if (parent_len == 0u || parent_len >= sizeof parent)
        return 1;
    memcpy(parent, path, parent_len);
    parent[parent_len] = '\0';
    char resolved_parent[PATH_MAX];
    if (canonical_path(parent, resolved_parent, sizeof resolved_parent) != 0 ||
        strcmp(parent, resolved_parent) != 0 || !path_is_within(root, resolved_parent))
        return 1;
    int n = snprintf(out, capacity, "%s/%s", resolved_parent, slash + 1);
    if (n < 0 || (size_t)n >= capacity || !path_is_within(root, out))
        return 1;
    struct stat st;
    if (lstat(out, &st) == 0 || errno != ENOENT)
        return 1;
    return 0;
}

static int canonical_path(const char *path, char *out, size_t capacity)
{
    char resolved[PATH_MAX];
    if (realpath(path, resolved) == NULL || strlen(resolved) >= capacity)
        return 1;
    return copy_path(out, capacity, resolved);
}

static int validate_host_paths(host_session *host)
{
    char root[PATH_MAX], ra[PATH_MAX], save[PATH_MAX], retroarch[PATH_MAX];
    char core[PATH_MAX], info[PATH_MAX], rom[PATH_MAX], compare[PATH_MAX];
    if (canonical_path(host->output_root, root, sizeof root) != 0 ||
        canonical_path(host->ra_dir, ra, sizeof ra) != 0 ||
        canonical_path(host->save_dir, save, sizeof save) != 0 ||
        canonical_path(host->retroarch, retroarch, sizeof retroarch) != 0 ||
        canonical_path(host->core, core, sizeof core) != 0 ||
        canonical_path(host->info, info, sizeof info) != 0 ||
        canonical_path(host->rom, rom, sizeof rom) != 0 ||
        canonical_path(host->compare, compare, sizeof compare) != 0)
        return 1;
    if (strcmp(root, host->output_root) != 0 || strcmp(ra, host->ra_dir) != 0 ||
        strcmp(save, host->save_dir) != 0 || strcmp(retroarch, host->retroarch) != 0 ||
        strcmp(core, host->core) != 0 || strcmp(info, host->info) != 0 ||
        strcmp(rom, host->rom) != 0 || strcmp(compare, host->compare) != 0)
        return 1;
    static const char fixture_suffix[] = "tests/diag/frontend-states.a78";
    size_t rom_len = strlen(rom), suffix_len = sizeof fixture_suffix - 1u;
    if (strcmp(root, "/") == 0 || strcmp(ra, "/") == 0 || strcmp(save, "/") == 0 ||
        !path_is_within(root, ra) || !path_is_within(root, save) || paths_overlap(ra, save) ||
        rom_len <= suffix_len || strcmp(rom + rom_len - suffix_len, fixture_suffix) != 0)
        return 1;
    if (strcmp(strrchr(rom, '/') != NULL ? strrchr(rom, '/') + 1 : rom,
               "frontend-states.a78") != 0)
        return 1;
    if (copy_path(host->retroarch, sizeof host->retroarch, retroarch) != 0 ||
        copy_path(host->core, sizeof host->core, core) != 0 ||
        copy_path(host->info, sizeof host->info, info) != 0 ||
        copy_path(host->rom, sizeof host->rom, rom) != 0 ||
        copy_path(host->ra_dir, sizeof host->ra_dir, ra) != 0 ||
        copy_path(host->save_dir, sizeof host->save_dir, save) != 0 ||
        copy_path(host->output_root, sizeof host->output_root, root) != 0 ||
        copy_path(host->compare, sizeof host->compare, compare) != 0)
        return 1;
    struct stat st;
    const char *dirs[] = {host->output_root, host->ra_dir, host->save_dir};
    for (size_t i = 0u; i < sizeof dirs / sizeof dirs[0]; i++) {
        if (lstat(dirs[i], &st) != 0 || !S_ISDIR(st.st_mode) || S_ISLNK(st.st_mode))
            return 1;
    }
    int n1 = snprintf(host->shot_dir, sizeof host->shot_dir, "%s/shots", host->ra_dir);
    int n2 = snprintf(host->state_path, sizeof host->state_path,
                      "%s/states/frontend-states.state", host->ra_dir);
    int n3 = snprintf(host->config_path, sizeof host->config_path, "%s/test.cfg", host->ra_dir);
    int n4 = snprintf(host->profile_path, sizeof host->profile_path,
                      "%s/states-network.sb", host->ra_dir);
    int n5 = snprintf(host->log_path, sizeof host->log_path, "%s/retroarch.log", host->ra_dir);
    int n6 = snprintf(host->media_path, sizeof host->media_path,
                      "%s/frontend-states.srm", host->save_dir);
    if (n1 < 0 || (size_t)n1 >= sizeof host->shot_dir || n2 < 0 ||
        (size_t)n2 >= sizeof host->state_path || n3 < 0 || (size_t)n3 >= sizeof host->config_path ||
        n4 < 0 || (size_t)n4 >= sizeof host->profile_path || n5 < 0 ||
        (size_t)n5 >= sizeof host->log_path || n6 < 0 ||
        (size_t)n6 >= sizeof host->media_path || !path_is_within(host->save_dir, host->media_path))
        return 1;
    char final_shot[PATH_MAX];
    if (normalize_output_file(host->final_shot, root, final_shot, sizeof final_shot) != 0 ||
        copy_path(host->final_shot, sizeof host->final_shot, final_shot) != 0)
        return 1;
    struct stat nested;
    char state_dir[PATH_MAX];
    int ns = snprintf(state_dir, sizeof state_dir, "%s/states", host->ra_dir);
    if (ns < 0 || (size_t)ns >= sizeof state_dir ||
        lstat(host->shot_dir, &nested) != 0 || !S_ISDIR(nested.st_mode) || S_ISLNK(nested.st_mode) ||
        lstat(state_dir, &nested) != 0 || !S_ISDIR(nested.st_mode) || S_ISLNK(nested.st_mode) ||
        !directory_empty(host->save_dir) || !directory_empty(host->shot_dir) ||
        !directory_empty(state_dir))
        return 1;
    if (paths_overlap(host->final_shot, host->ra_dir) ||
        paths_overlap(host->final_shot, host->save_dir))
        return 1;
    return 0;
}

static int run_capture(char *const argv[], char *output, size_t capacity, unsigned timeout_ms,
                       int *exit_status)
{
    int pipefd[2];
    if (pipe(pipefd) != 0)
        return 1;
    pid_t pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    if (pid == 0) {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) < 0)
            _exit(126);
        close(pipefd[1]);
        execv(argv[0], argv);
        _exit(127);
    }
    close(pipefd[1]);
    int flags = fcntl(pipefd[0], F_GETFL, 0);
    if (flags >= 0)
        (void)fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    size_t used = 0u;
    int eof = 0, status = 0, done = 0, failed = 0;
    uint64_t start = states_now_ms();
    uint64_t deadline = start + timeout_ms;
    uint64_t observe_deadline = timeout_ms > 100u ? deadline - 100u : start;
    while (!eof || !done) {
        uint64_t now = states_now_ms();
        if (now >= observe_deadline) {
            failed = 1;
            break;
        }
        int wait_ms = (int)(observe_deadline - now);
        if (wait_ms > 50)
            wait_ms = 50;
        struct pollfd pfd = {.fd = pipefd[0], .events = POLLIN | POLLHUP};
        int polled = poll(&pfd, 1u, wait_ms);
        if (polled < 0 && errno != EINTR) {
            failed = 1;
            break;
        }
        if (polled > 0 && (pfd.revents & (POLLIN | POLLHUP)) != 0) {
            char chunk[512];
            ssize_t n = read(pipefd[0], chunk, sizeof chunk);
            if (n == 0) {
                eof = 1;
            } else if (n > 0) {
                if ((size_t)n >= capacity - used) {
                    failed = 1;
                    break;
                }
                memcpy(output + used, chunk, (size_t)n);
                used += (size_t)n;
                output[used] = '\0';
            } else if (errno != EAGAIN && errno != EINTR) {
                failed = 1;
                break;
            }
        }
        if (!done) {
            pid_t waited = waitpid(pid, &status, WNOHANG);
            if (waited == pid)
                done = 1;
            else if (waited < 0 && errno != EINTR) {
                failed = 1;
                break;
            }
        }
    }
    close(pipefd[0]);
    if (failed) {
        if (kill(pid, SIGTERM) != 0 && errno != ESRCH)
            return 1;
        uint64_t term_deadline = states_now_ms() + 30u;
        if (term_deadline > deadline)
            term_deadline = deadline;
        while (states_now_ms() < term_deadline) {
            pid_t waited = waitpid(pid, &status, WNOHANG);
            if (waited == pid)
                return 1;
            if (waited < 0 && errno != EINTR)
                return 1;
            struct timespec delay = {.tv_sec = 0, .tv_nsec = 5000000L};
            (void)nanosleep(&delay, NULL);
        }
        if (kill(pid, SIGKILL) != 0 && errno != ESRCH)
            return 1;
        while (states_now_ms() < deadline) {
            pid_t waited = waitpid(pid, &status, WNOHANG);
            if (waited == pid)
                return 1;
            if (waited < 0 && errno != EINTR)
                return 1;
            struct timespec delay = {.tv_sec = 0, .tv_nsec = 5000000L};
            (void)nanosleep(&delay, NULL);
        }
        return 1;
    }
    if (!done)
        return 1;
    if (!WIFEXITED(status))
        return 1;
    *exit_status = WEXITSTATUS(status);
    return 0;
}

static int parse_owner_pids(const char *text, unsigned port, pid_t *pids, size_t capacity,
                            size_t *count)
{
    char copy[4096];
    size_t n = strlen(text);
    if (n >= sizeof copy)
        return 1;
    memcpy(copy, text, n + 1u);
    *count = 0u;
    pid_t current = -1;
    int local_match = 0;
    char port_text[16];
    int plen = snprintf(port_text, sizeof port_text, ":%u", port);
    if (plen < 0 || (size_t)plen >= sizeof port_text)
        return 1;
    for (char *line = strtok(copy, "\n"); line != NULL; line = strtok(NULL, "\n")) {
        if (line[0] == 'p') {
            if (current > 0 && local_match) {
                int seen = 0;
                for (size_t i = 0u; i < *count; i++)
                    seen |= pids[i] == current;
                if (!seen) {
                    if (*count >= capacity)
                        return 1;
                    pids[(*count)++] = current;
                }
            }
            char *end = NULL;
            long value = strtol(line + 1, &end, 10);
            if (end == line + 1 || *end != '\0' || value <= 0 || value > INT_MAX)
                return 1;
            current = (pid_t)value;
            local_match = 0;
        } else if (line[0] == 'n') {
            const char *name = line + 1;
            const char *arrow = strstr(name, "->");
            size_t local_len = arrow == NULL ? strlen(name) : (size_t)(arrow - name);
            if (local_len >= (size_t)plen &&
                strncmp(name + local_len - (size_t)plen, port_text, (size_t)plen) == 0)
                local_match = 1;
        }
    }
    if (current > 0 && local_match) {
        int seen = 0;
        for (size_t i = 0u; i < *count; i++)
            seen |= pids[i] == current;
        if (!seen) {
            if (*count >= capacity)
                return 1;
            pids[(*count)++] = current;
        }
    }
    return 0;
}

static int socket_owners(unsigned port, pid_t *pids, size_t capacity, size_t *count)
{
    char selector[32], output[4096] = {0};
    int n = snprintf(selector, sizeof selector, "-iUDP:%u", port);
    if (n < 0 || (size_t)n >= sizeof selector)
        return 1;
    char *argv[] = {"/usr/sbin/lsof", "-nP", "-Fpn", selector, NULL};
    int status = 0;
    if (run_capture(argv, output, sizeof output, 750u, &status) != 0)
        return 1;
    if (status == 1 && output[0] == '\0') {
        *count = 0u;
        return 0;
    }
    if (status != 0)
        return 1;
    return parse_owner_pids(output, port, pids, capacity, count);
}

static int process_is_child(host_session *host)
{
    char actual[PATH_MAX];
    return host->child > 0 && process_path(host->child, actual, sizeof actual) == 0 &&
           strcmp(actual, host->retroarch) == 0;
}

static int reap_if_exited(host_session *host)
{
    if (host->child_reaped)
        return 1;
    int status = 0;
    pid_t waited = waitpid(host->child, &status, WNOHANG);
    if (waited == host->child) {
        host->child_reaped = 1;
        host->child_owned = 0;
        host->child_status = status;
        return 1;
    }
    return waited == 0;
}

static int endpoint_owned(host_session *host)
{
    if (!host->child_owned || !reap_if_exited(host) || host->child_reaped ||
        !process_is_child(host))
        return 0;
    pid_t owners[8];
    size_t count = 0u;
    return socket_owners(host->port, owners, sizeof owners / sizeof owners[0], &count) == 0 &&
           count == 1u && owners[0] == host->child;
}

static int reserve_port(uint16_t wanted, int *fd_out, uint16_t *port_out)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return 1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(wanted);
    if (bind(fd, (struct sockaddr *)&addr, sizeof addr) != 0) {
        close(fd);
        return 1;
    }
    socklen_t len = sizeof addr;
    if (getsockname(fd, (struct sockaddr *)&addr, &len) != 0) {
        close(fd);
        return 1;
    }
    *fd_out = fd;
    *port_out = ntohs(addr.sin_port);
    return 0;
}

static int write_profile(host_session *host)
{
    FILE *file = fopen(host->profile_path, "wx");
    if (file == NULL)
        return 1;
    int rc = fprintf(file,
                     "(version 1)\n"
                     "(allow default)\n"
                     "(deny network-outbound)\n"
                     "(deny network-inbound)\n"
                     "(deny network-bind)\n"
                     "(allow network-bind (local udp \"*:%u\"))\n"
                     "(allow network-outbound (remote udp \"localhost:*\"))\n"
                     "(allow network-inbound (local udp \"localhost:%u\"))\n",
                     (unsigned)host->port, (unsigned)host->port);
    int close_rc = fclose(file);
    if (rc < 0 || close_rc != 0)
        return 1;
    return 0;
}

static int parse_port(const char *text, uint16_t *port)
{
    char *end = NULL;
    unsigned long n = strtoul(text, &end, 10);
    if (end == text || *end != '\0' || n == 0u || n > 65535u)
        return 1;
    *port = (uint16_t)n;
    return 0;
}

static int helper_write(const char *text)
{
    size_t n = strlen(text);
    return write(STDOUT_FILENO, text, n) == (ssize_t)n ? 0 : 1;
}

static int udp_helper_main(const char *port_text)
{
    uint16_t port;
    if (parse_port(port_text, &port) != 0)
        return 2;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return 1;
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof bind_addr);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&bind_addr, sizeof bind_addr) != 0 ||
        helper_write("READY\n") != 0) {
        close(fd);
        return 1;
    }
    for (unsigned stage = 0u; stage < 2u; stage++) {
        struct pollfd pfd = {.fd = fd, .events = POLLIN};
        int rc = poll(&pfd, 1u, stage == 0u ? 1000 : 800);
        if (rc <= 0)
            break;
        char bytes[32] = {0};
        struct sockaddr_in from;
        socklen_t from_len = sizeof from;
        ssize_t n = recvfrom(fd, bytes, sizeof bytes - 1u, 0, (struct sockaddr *)&from, &from_len);
        if (n <= 0)
            break;
        bytes[n] = '\0';
        if (stage == 0u && strcmp(bytes, "LOOPBACK") == 0 &&
            ntohl(from.sin_addr.s_addr) == INADDR_LOOPBACK) {
            if (helper_write("LOOPBACK\n") != 0 ||
                sendto(fd, "LOOPBACK", 8u, 0, (struct sockaddr *)&from, from_len) != 8)
                break;
            continue;
        }
        if (stage == 1u && strcmp(bytes, "EXTERNAL") == 0) {
            (void)helper_write("EXTERNAL\n");
            (void)sendto(fd, "EXTERNAL", 8u, 0, (struct sockaddr *)&from, from_len);
            close(fd);
            return 1;
        }
        break;
    }
    close(fd);
    (void)helper_write("REJECTED\n");
    return 0;
}

static int read_line_fd(int fd, char *line, size_t capacity, unsigned timeout_ms)
{
    size_t used = 0u;
    uint64_t deadline = states_now_ms() + timeout_ms;
    while (used + 1u < capacity) {
        uint64_t now = states_now_ms();
        if (now >= deadline)
            return 1;
        struct pollfd pfd = {.fd = fd, .events = POLLIN | POLLHUP};
        int left = (int)(deadline - now);
        int rc = poll(&pfd, 1u, left);
        if (rc <= 0)
            return 1;
        char c;
        ssize_t n = read(fd, &c, 1u);
        if (n != 1)
            return 1;
        if (c == '\n') {
            line[used] = '\0';
            return 0;
        }
        line[used++] = c;
    }
    return 1;
}

static int nonloopback_ipv4(struct in_addr *address)
{
    struct ifaddrs *list = NULL;
    if (getifaddrs(&list) != 0)
        return 1;
    int found = 1;
    for (struct ifaddrs *it = list; it != NULL; it = it->ifa_next) {
        if (it->ifa_addr == NULL || it->ifa_addr->sa_family != AF_INET ||
            (it->ifa_flags & (IFF_UP | IFF_LOOPBACK)) != IFF_UP)
            continue;
        struct in_addr candidate = ((struct sockaddr_in *)it->ifa_addr)->sin_addr;
        uint32_t host = ntohl(candidate.s_addr);
        if ((host >> 24) == 127u || host == 0u)
            continue;
        *address = candidate;
        found = 0;
        break;
    }
    freeifaddrs(list);
    return found;
}

static int sandbox_helper(host_session *host, const char *self_path)
{
    char self_exec[PATH_MAX];
    if (copy_path(self_exec, sizeof self_exec, self_path) != 0)
        return 1;
    int port_fd = -1;
    uint16_t selected = 0u;
    if (reserve_port(0u, &port_fd, &selected) != 0)
        return 1;
    host->port = selected;
    pid_t owners[8];
    size_t owner_count = 0u;
    if (socket_owners(host->port, owners, 8u, &owner_count) != 0 || owner_count != 1u ||
        owners[0] != getpid() || write_profile(host) != 0) {
        close(port_fd);
        return 1;
    }
    close(port_fd);
    port_fd = -1;
    int out_pipe[2];
    if (pipe(out_pipe) != 0)
        return 1;
    char port_text[16];
    (void)snprintf(port_text, sizeof port_text, "%u", (unsigned)host->port);
    pid_t helper = fork();
    if (helper < 0) {
        close(out_pipe[0]);
        close(out_pipe[1]);
        return 1;
    }
    if (helper == 0) {
        close(out_pipe[0]);
        if (dup2(out_pipe[1], STDOUT_FILENO) < 0)
            _exit(126);
        close(out_pipe[1]);
        char *argv[] = {"/usr/bin/sandbox-exec", "-f", host->profile_path,
                        self_exec, "--udp-helper", port_text, NULL};
        execv(argv[0], argv);
        _exit(127);
    }
    close(out_pipe[1]);
    char line[64];
    if (read_line_fd(out_pipe[0], line, sizeof line, 1000u) != 0 || strcmp(line, "READY") != 0) {
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    int client = socket(AF_INET, SOCK_DGRAM, 0);
    if (client < 0) {
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    struct sockaddr_in loopback;
    memset(&loopback, 0, sizeof loopback);
    loopback.sin_family = AF_INET;
    loopback.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback.sin_port = htons(host->port);
    if (sendto(client, "LOOPBACK", 8u, 0, (struct sockaddr *)&loopback, sizeof loopback) != 8) {
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    struct pollfd pfd = {.fd = client, .events = POLLIN};
    if (poll(&pfd, 1u, 1000) <= 0) {
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    char response[32] = {0};
    struct sockaddr_in from;
    socklen_t from_len = sizeof from;
    ssize_t n = recvfrom(client, response, sizeof response - 1u, 0,
                         (struct sockaddr *)&from, &from_len);
    if (n != 8 || strcmp(response, "LOOPBACK") != 0 ||
        ntohl(from.sin_addr.s_addr) != INADDR_LOOPBACK || ntohs(from.sin_port) != host->port ||
        read_line_fd(out_pipe[0], line, sizeof line, 250u) != 0 || strcmp(line, "LOOPBACK") != 0) {
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    struct in_addr external;
    if (nonloopback_ipv4(&external) != 0) {
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    int external_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (external_client < 0) {
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    struct sockaddr_in source = {.sin_family = AF_INET, .sin_addr = external, .sin_port = 0};
    if (bind(external_client, (struct sockaddr *)&source, sizeof source) != 0) {
        close(external_client);
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    struct sockaddr_in external_target = {.sin_family = AF_INET, .sin_addr = external,
                                          .sin_port = htons(host->port)};
    if (sendto(external_client, "EXTERNAL", 8u, 0, (struct sockaddr *)&external_target,
               sizeof external_target) != 8) {
        close(external_client);
        close(client);
        close(out_pipe[0]);
        (void)kill(helper, SIGTERM);
        (void)waitpid(helper, NULL, 0);
        return 1;
    }
    pfd.fd = external_client;
    pfd.events = POLLIN;
    int external_reply = poll(&pfd, 1u, 1000);
    int helper_report = read_line_fd(out_pipe[0], line, sizeof line, 1000u);
    close(external_client);
    close(client);
    close(out_pipe[0]);
    int helper_status = 0;
    uint64_t helper_deadline = states_now_ms() + 1000u;
    pid_t helper_waited = 0;
    while (states_now_ms() < helper_deadline) {
        helper_waited = waitpid(helper, &helper_status, WNOHANG);
        if (helper_waited == helper || helper_waited < 0)
            break;
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
        (void)nanosleep(&delay, NULL);
    }
    if (helper_waited != helper) {
        if (helper_waited == 0)
            (void)kill(helper, SIGTERM);
        if (helper_waited == 0)
            (void)waitpid(helper, &helper_status, 0);
        return 1;
    }
    if (helper_report != 0 || strcmp(line, "REJECTED") != 0 || external_reply > 0 ||
        !WIFEXITED(helper_status) || WEXITSTATUS(helper_status) != 0)
        return 1;
    int final_reserve = -1;
    uint16_t final_port = 0u;
    if (reserve_port(host->port, &final_reserve, &final_port) != 0 || final_port != host->port)
        return 1;
    owner_count = 0u;
    int owner_ok = socket_owners(host->port, owners, 8u, &owner_count) == 0 &&
                   owner_count == 1u && owners[0] == getpid();
    if (!owner_ok) {
        close(final_reserve);
        return 1;
    }
    host->reserved_fd = final_reserve;
    return 0;
}

static int preflight_only(const char *self_path, const char *output_dir)
{
    host_session host;
    memset(&host, 0, sizeof host);
    host.reserved_fd = -1;
    if (canonical_path(output_dir, host.ra_dir, sizeof host.ra_dir) != 0 ||
        strcmp(host.ra_dir, "/") == 0)
        return 2;
    int n = snprintf(host.profile_path, sizeof host.profile_path, "%s/states-network.sb",
                     host.ra_dir);
    if (n < 0 || (size_t)n >= sizeof host.profile_path ||
        access("/usr/bin/sandbox-exec", X_OK) != 0 || sandbox_helper(&host, self_path) != 0) {
        if (host.reserved_fd >= 0)
            close(host.reserved_fd);
        fprintf(stderr, "states preflight: external UDP containment not proven\n");
        return 1;
    }
    close(host.reserved_fd);
    printf("states preflight: loopback UDP replied; non-loopback request was not delivered; port=%u\n",
           (unsigned)host.port);
    return 0;
}

static int update_config_port(host_session *host)
{
    FILE *file = fopen(host->config_path, "rb");
    if (file == NULL)
        return 1;
    char content[32768];
    size_t n = fread(content, 1u, sizeof content - 1u, file);
    int read_error = ferror(file);
    fclose(file);
    if (read_error || n == sizeof content - 1u)
        return 1;
    content[n] = '\0';
    const char *old = "network_cmd_port = \"0\"";
    char *where = strstr(content, old);
    if (where == NULL || strstr(where + strlen(old), old) != NULL)
        return 1;
    char replacement[64];
    int m = snprintf(replacement, sizeof replacement, "network_cmd_port = \"%u\"",
                     (unsigned)host->port);
    if (m < 0 || (size_t)m >= sizeof replacement)
        return 1;
    char result[32768];
    size_t before = (size_t)(where - content), after = n - before - strlen(old);
    if (before + (size_t)m + after >= sizeof result)
        return 1;
    memcpy(result, content, before);
    memcpy(result + before, replacement, (size_t)m);
    memcpy(result + before + (size_t)m, where + strlen(old), after);
    result[before + (size_t)m + after] = '\0';
    int fd = open(host->config_path, O_WRONLY | O_TRUNC | O_NOFOLLOW);
    if (fd < 0)
        return 1;
    size_t left = strlen(result), offset = 0u;
    while (offset < left) {
        ssize_t wrote = write(fd, result + offset, left - offset);
        if (wrote <= 0) {
            close(fd);
            return 1;
        }
        offset += (size_t)wrote;
    }
    return close(fd) == 0 ? 0 : 1;
}

static int host_validate_runahead_config(host_session *host)
{
    static const struct {
        const char *key;
        const char *line;
    } required[] = {
        {"run_ahead_enabled", "run_ahead_enabled = \"false\""},
        {"run_ahead_frames", "run_ahead_frames = \"1\""},
        {"run_ahead_secondary_instance", "run_ahead_secondary_instance = \"false\""},
        {"preemptive_frames_enable", "preemptive_frames_enable = \"false\""},
    };
    FILE *file = fopen(host->config_path, "rb");
    if (file == NULL)
        return 1;
    unsigned matches[sizeof required / sizeof required[0]] = {0u};
    char line[512];
    while (fgets(line, sizeof line, file) != NULL) {
        size_t length = strcspn(line, "\r\n");
        line[length] = '\0';
        for (size_t i = 0u; i < sizeof required / sizeof required[0]; i++) {
            size_t key_length = strlen(required[i].key);
            if (strncmp(line, required[i].key, key_length) == 0 && line[key_length] == ' ') {
                if (strcmp(line, required[i].line) != 0 || ++matches[i] != 1u) {
                    fclose(file);
                    return 1;
                }
            }
        }
    }
    int failed = ferror(file);
    if (fclose(file) != 0)
        failed = 1;
    for (size_t i = 0u; i < sizeof required / sizeof required[0]; i++)
        failed |= matches[i] != 1u;
    return failed;
}

static int host_prepare_media(host_session *host)
{
    if (!path_is_within(host->save_dir, host->media_path))
        return 1;
    int fd = open(host->media_path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (fd < 0)
        return 1;
    uint8_t bytes[STATES_MEDIA_BYTES];
    memset(bytes, 0xff, sizeof bytes);
    size_t offset = 0u;
    int failed = 0;
    while (offset < sizeof bytes) {
        ssize_t n = write(fd, bytes + offset, sizeof bytes - offset);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            failed = 1;
            break;
        }
        offset += (size_t)n;
    }
    if (fsync(fd) != 0)
        failed = 1;
    if (close(fd) != 0)
        failed = 1;
    return failed;
}

static int host_media_matches(host_session *host, unsigned expected, int sentinel)
{
    if (!path_is_within(host->save_dir, host->media_path))
        return 1;
    struct stat before, opened;
    if (lstat(host->media_path, &before) != 0 || !S_ISREG(before.st_mode) ||
        S_ISLNK(before.st_mode) || before.st_size != (off_t)STATES_MEDIA_BYTES)
        return 1;
    int fd = open(host->media_path, O_RDONLY | O_NOFOLLOW);
    if (fd < 0 || fstat(fd, &opened) != 0 || !S_ISREG(opened.st_mode) ||
        opened.st_dev != before.st_dev || opened.st_ino != before.st_ino ||
        opened.st_size != (off_t)STATES_MEDIA_BYTES) {
        if (fd >= 0)
            close(fd);
        return 1;
    }
    uint8_t bytes[STATES_MEDIA_BYTES];
    size_t offset = 0u;
    while (offset < sizeof bytes) {
        ssize_t n = read(fd, bytes + offset, sizeof bytes - offset);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            close(fd);
            return 1;
        }
        offset += (size_t)n;
    }
    uint8_t extra = 0u;
    ssize_t tail;
    do {
        tail = read(fd, &extra, 1u);
    } while (tail < 0 && errno == EINTR);
    if (tail != 0 || close(fd) != 0)
        return 1;
    for (size_t i = 0u; i < sizeof bytes; i++) {
        uint8_t wanted = 0xffu;
        if (!sentinel && i == 0x100u)
            wanted = (uint8_t)(expected & 0xffu);
        else if (!sentinel && i == 0x101u)
            wanted = (uint8_t)((expected >> 8) & 0xffu);
        if (bytes[i] != wanted)
            return 1;
    }
    return 0;
}

static int spawn_retroarch(host_session *host)
{
    char frames_arg[32];
    (void)snprintf(frames_arg, sizeof frames_arg, "--max-frames=%u", host->frames);
    int log_fd = open(host->log_path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (log_fd < 0)
        return 1;
    char *argv[] = {"/usr/bin/sandbox-exec", "-f", host->profile_path, host->retroarch,
                    "--verbose", "--config", host->config_path, "-L", host->core,
                    frames_arg, host->rom, NULL};
    close(host->reserved_fd);
    host->reserved_fd = -1;
    pid_t pid = fork();
    if (pid < 0) {
        close(log_fd);
        return 1;
    }
    if (pid == 0) {
        if (dup2(log_fd, STDOUT_FILENO) < 0 || dup2(log_fd, STDERR_FILENO) < 0)
            _exit(126);
        close(log_fd);
        char home[PATH_MAX];
        int home_len = snprintf(home, sizeof home, "%s/home", host->ra_dir);
        if (home_len < 0 || (size_t)home_len >= sizeof home)
            _exit(126);
        (void)setenv("HOME", home, 1);
        (void)setenv("CFFIXED_USER_HOME", home, 1);
        execv(argv[0], argv);
        _exit(127);
    }
    close(log_fd);
    host->child = pid;
    host->child_owned = 1;
    host->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (host->udp_fd < 0)
        return 1;
    struct sockaddr_in local;
    memset(&local, 0, sizeof local);
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(host->udp_fd, (struct sockaddr *)&local, sizeof local) != 0)
        return 1;
    uint64_t deadline = states_now_ms() + STATES_OBSERVE_MS * 5u;
    while (states_now_ms() < deadline) {
        if (endpoint_owned(host))
            return 0;
        if (host->child_reaped)
            return 1;
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 20000000L};
        (void)nanosleep(&delay, NULL);
    }
    return 1;
}

static int host_nci_send(host_session *host, const char *command)
{
    if (!endpoint_owned(host))
        return 1;
    struct sockaddr_in target;
    memset(&target, 0, sizeof target);
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    target.sin_port = htons(host->port);
    size_t n = strlen(command);
    return sendto(host->udp_fd, command, n, 0, (struct sockaddr *)&target, sizeof target) ==
                   (ssize_t)n
               ? 0
               : 1;
}

static int host_nci_reply(host_session *host, const char *command, unsigned timeout_ms,
                          char *reply, size_t capacity)
{
    if (host_nci_send(host, command) != 0)
        return 1;
    struct pollfd pfd = {.fd = host->udp_fd, .events = POLLIN};
    int rc = poll(&pfd, 1u, (int)timeout_ms);
    if (rc <= 0 || (pfd.revents & POLLIN) == 0)
        return 1;
    struct sockaddr_in from;
    struct iovec iov = {.iov_base = reply, .iov_len = capacity - 1u};
    struct msghdr msg;
    memset(&msg, 0, sizeof msg);
    msg.msg_name = &from;
    msg.msg_namelen = sizeof from;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1u;
    ssize_t got = recvmsg(host->udp_fd, &msg, 0);
    if (got <= 0 || (msg.msg_flags & MSG_TRUNC) != 0 || (size_t)got >= capacity ||
        from.sin_family != AF_INET || ntohl(from.sin_addr.s_addr) != INADDR_LOOPBACK ||
        ntohs(from.sin_port) != host->port)
        return 1;
    reply[got] = '\0';
    return 0;
}

static int host_status(host_session *host, unsigned timeout_ms, int *paused)
{
    char reply[512];
    if (host_nci_reply(host, "GET_STATUS", timeout_ms, reply, sizeof reply) != 0)
        return STATES_IDENTITY_STATUS;
    return states_parse_status(reply, host->fixture_crc, paused);
}

static int host_status_op(void *user, unsigned timeout_ms, int *paused)
{
    return host_status(user, timeout_ms, paused) == STATES_IDENTITY_OK ? 0 : 1;
}

static int host_pause_toggle_once(void *user)
{
    return host_nci_send(user, "PAUSE_TOGGLE");
}

static int host_rewind_once(void *user)
{
    return host_nci_send(user, "REWIND");
}

static int host_runahead_toggle_once(void *user)
{
    return host_nci_send(user, "RUNAHEAD_TOGGLE");
}

static uint64_t host_now(void *user)
{
    (void)user;
    return states_now_ms();
}

static int host_preflight(void *user, unsigned timeout_ms)
{
    host_session *host = user;
    (void)timeout_ms;
    return endpoint_owned(host) ? 0 : 1;
}

static int host_verify_identity(void *user, unsigned timeout_ms)
{
    host_session *host = user;
    char reply[256];
    if (host_nci_reply(host, "VERSION", timeout_ms, reply, sizeof reply) != 0)
        return STATES_IDENTITY_VERSION;
    int version = states_parse_version(reply);
    if (version != STATES_IDENTITY_OK)
        return version;
    int paused = 0;
    uint64_t deadline = states_now_ms() + timeout_ms;
    int status = host_status(host, timeout_ms, &paused);
    if (status != STATES_IDENTITY_OK)
        return status;
    if (!paused) {
        if (host_nci_send(host, "PAUSE_TOGGLE") != 0)
            return STATES_IDENTITY_STATUS;
        while (states_now_ms() < deadline) {
            uint64_t now = states_now_ms();
            unsigned left = (unsigned)(deadline - now);
            if (left > STATES_OBSERVE_MS)
                left = STATES_OBSERVE_MS;
            status = host_status(host, left, &paused);
            if (status != STATES_IDENTITY_OK)
                return status;
            if (paused)
                break;
        }
    }
    return paused ? STATES_IDENTITY_OK : STATES_IDENTITY_STATUS;
}

static int host_read_progress(void *user, unsigned *progress, unsigned timeout_ms)
{
    host_session *host = user;
    char reply[256];
    if (host_nci_reply(host, "READ_CORE_MEMORY 2300 6", timeout_ms, reply, sizeof reply) != 0)
        return 1;
    return states_parse_progress(reply, progress) == STATES_PROGRESS_OK ? 0 : 1;
}

static int host_state_absent(void *user, int absent, unsigned timeout_ms)
{
    host_session *host = user;
    (void)timeout_ms;
    struct stat st;
    if (lstat(host->state_path, &st) == 0)
        return absent || !S_ISREG(st.st_mode) || S_ISLNK(st.st_mode) || st.st_size <= 0;
    return errno == ENOENT ? !absent : 1;
}

static int host_save_once(void *user)
{
    return host_nci_send(user, "SAVE_STATE");
}

static int host_state_created(void *user, unsigned timeout_ms)
{
    host_session *host = user;
    uint64_t deadline = states_now_ms() + timeout_ms;
    do {
        struct stat st;
        if (lstat(host->state_path, &st) == 0)
            return S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode) && st.st_size > 0 ? 0 : 1;
        if (errno != ENOENT)
            return 1;
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
        (void)nanosleep(&delay, NULL);
    } while (states_now_ms() < deadline);
    return 1;
}

static int host_frame_once(void *user)
{
    host_session *host = user;
    if (host_nci_send(host, "FRAMEADVANCE") != 0)
        return 1;
    host->frames++;
    return 0;
}

static int host_load_once(void *user)
{
    return host_nci_send(user, "LOAD_STATE");
}

static int host_save_files_once(void *user, unsigned timeout_ms)
{
    host_session *host = user;
    char reply[128];
    if (host_nci_reply(host, "SAVE_FILES", timeout_ms, reply, sizeof reply) != 0)
        return 1;
    return states_parse_save_files_reply(reply);
}

static int host_durable_sentinel(void *user)
{
    return host_media_matches(user, 0u, 1);
}

static int host_durable_committed(void *user, unsigned expected)
{
    return host_media_matches(user, expected, 0);
}

static int compare_capture(host_session *host, unsigned expected, unsigned picture_id,
                           int baseline, const char *path)
{
    char value[8];
    char reference_path[PATH_MAX], image_path[PATH_MAX];
    (void)snprintf(value, sizeof value, "%04X", expected & 0xffffu);
    const char *reference = baseline ? path : host->captures[picture_id];
    if (baseline && picture_id == 3u && host->captures[2][0] != '\0')
        reference = host->captures[2];
    if (copy_path(reference_path, sizeof reference_path, reference) != 0 ||
        copy_path(image_path, sizeof image_path, path) != 0)
        return 1;
    char *argv[] = {host->compare, "--progress", value, reference_path, image_path, NULL};
    char output[1024] = {0};
    int status = 0;
    if (run_capture(argv, output, sizeof output, STATES_OBSERVE_MS, &status) != 0 || status != 0)
        return 1;
    return 0;
}

static int host_capture(void *user, unsigned expected, unsigned picture_id, int baseline,
                        unsigned timeout_ms)
{
    host_session *host = user;
    if (picture_id == 0u || picture_id > 3u || host->capture_count >= STATES_CAPTURE_CAP ||
        host_nci_send(host, "SCREENSHOT") != 0)
        return 1;
    uint64_t deadline = states_now_ms() + timeout_ms;
    char source[PATH_MAX] = "";
    while (states_now_ms() < deadline) {
        DIR *dir = opendir(host->shot_dir);
        if (dir == NULL)
            return 1;
        struct dirent *entry;
        unsigned fresh = 0u;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.' || strncmp(entry->d_name, "capture-", 8u) == 0)
                continue;
            char candidate[PATH_MAX];
            if (snprintf(candidate, sizeof candidate, "%s/%s", host->shot_dir, entry->d_name) < 0)
                continue;
            struct stat st;
            if (lstat(candidate, &st) != 0 || !S_ISREG(st.st_mode) || S_ISLNK(st.st_mode) ||
                st.st_size <= 0)
                continue;
            if (copy_path(source, sizeof source, candidate) != 0)
                continue;
            fresh++;
        }
        closedir(dir);
        if (fresh == 1u)
            break;
        source[0] = '\0';
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
        (void)nanosleep(&delay, NULL);
    }
    if (source[0] == '\0')
        return 1;
    char target[PATH_MAX];
    int n = snprintf(target, sizeof target, "%s/capture-%02u.bmp", host->shot_dir,
                     host->capture_count + 1u);
    if (n < 0 || (size_t)n >= sizeof target || rename(source, target) != 0)
        return 1;
    struct stat st;
    if (lstat(target, &st) != 0 || !S_ISREG(st.st_mode) || S_ISLNK(st.st_mode) || st.st_size <= 70)
        return 1;
    if (!baseline && host->captures[picture_id][0] == '\0')
        return 1;
    int rc = compare_capture(host, expected, picture_id, baseline, target);
    if (rc == 0 && baseline &&
        copy_path(host->captures[picture_id], sizeof host->captures[picture_id], target) != 0)
        rc = 1;
    if (rc == 0 && copy_path(host->last_capture, sizeof host->last_capture, target) != 0)
        rc = 1;
    host->capture_count++;
    return rc;
}

static int host_wait_child(host_session *host, unsigned timeout_ms)
{
    uint64_t deadline = states_now_ms() + timeout_ms;
    while (states_now_ms() < deadline) {
        if (!reap_if_exited(host))
            return 1;
        if (host->child_reaped)
            return 0;
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 10000000L};
        (void)nanosleep(&delay, NULL);
    }
    return reap_if_exited(host) && host->child_reaped ? 0 : 1;
}

static int host_signal_owned_child(host_session *host, int signal_number, unsigned timeout_ms)
{
    if (!host->child_owned || host->child_reaped || !process_is_child(host))
        return 1;
    if (!reap_if_exited(host))
        return 1;
    if (host->child_reaped)
        return 0;
    if (!process_is_child(host) || kill(host->child, signal_number) != 0)
        return 1;
    return host_wait_child(host, timeout_ms);
}

static int host_cleanup_without_endpoint(host_session *host, unsigned timeout_ms)
{
    unsigned graceful = timeout_ms < 1000u ? timeout_ms : 1000u;
    if (host_wait_child(host, graceful) == 0)
        return 0;
    unsigned remaining = timeout_ms - graceful;
    unsigned term_wait = remaining < 500u ? remaining : 500u;
    if (host_signal_owned_child(host, SIGTERM, term_wait) == 0)
        return 0;
    remaining -= term_wait;
    return host_signal_owned_child(host, SIGKILL, remaining);
}

static unsigned cleanup_remaining(uint64_t deadline)
{
    uint64_t now = states_now_ms();
    if (now >= deadline)
        return 0u;
    uint64_t left = deadline - now;
    return left > UINT_MAX ? UINT_MAX : (unsigned)left;
}

static int host_cleanup(void *user, unsigned timeout_ms)
{
    host_session *host = user;
    uint64_t deadline = states_now_ms() + timeout_ms;
    int rc = 0;
    if (!host->child_owned) {
        if (host->udp_fd >= 0)
            close(host->udp_fd);
        host->udp_fd = -1;
        return 0;
    }
    int endpoint = endpoint_owned(host);
    unsigned remaining = cleanup_remaining(deadline);
    if (!endpoint) {
        rc = host_cleanup_without_endpoint(host, remaining);
    } else if (host_nci_send(host, "QUIT") != 0) {
        rc = 1;
        (void)host_cleanup_without_endpoint(host, cleanup_remaining(deadline));
    } else {
        remaining = cleanup_remaining(deadline);
        unsigned graceful = remaining < 1750u ? remaining : 1750u;
        if (host_wait_child(host, graceful) != 0) {
            remaining = cleanup_remaining(deadline);
            unsigned term_wait = remaining < 750u ? remaining : 750u;
            if (host_signal_owned_child(host, SIGTERM, term_wait) != 0) {
                if (host_signal_owned_child(host, SIGKILL, cleanup_remaining(deadline)) != 0)
                    rc = 1;
            }
        }
    }
    if (host->udp_fd >= 0)
        close(host->udp_fd);
    host->udp_fd = -1;
    return rc;
}

static int file_contains(const char *path, const char *needle)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return 1;
    char buffer[65536];
    size_t n = fread(buffer, 1u, sizeof buffer - 1u, file);
    int err = ferror(file);
    fclose(file);
    if (err || n == sizeof buffer - 1u)
        return 1;
    buffer[n] = '\0';
    return strstr(buffer, needle) == NULL;
}

static int copy_file(const char *from, const char *to)
{
    int src = open(from, O_RDONLY | O_NOFOLLOW);
    if (src < 0)
        return 1;
    int dst = open(to, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (dst < 0) {
        close(src);
        return 1;
    }
    char buffer[16384];
    int failed = 0;
    for (;;) {
        ssize_t n = read(src, buffer, sizeof buffer);
        if (n == 0)
            break;
        if (n < 0) {
            failed = 1;
            break;
        }
        ssize_t offset = 0;
        while (offset < n) {
            ssize_t wrote = write(dst, buffer + offset, (size_t)(n - offset));
            if (wrote <= 0) {
                failed = 1;
                break;
            }
            offset += wrote;
        }
        if (failed)
            break;
    }
    if (close(src) != 0 || close(dst) != 0)
        failed = 1;
    return failed;
}

static int digest_is_hex(const char *digest)
{
    if (strlen(digest) != 64u)
        return 0;
    for (size_t i = 0u; i < 64u; i++)
        if (!((digest[i] >= '0' && digest[i] <= '9') ||
              (digest[i] >= 'a' && digest[i] <= 'f') ||
              (digest[i] >= 'A' && digest[i] <= 'F')))
            return 0;
    return 1;
}

static int core_digest_matches(const char *path, const char *expected)
{
    char image_path[PATH_MAX];
    if (copy_path(image_path, sizeof image_path, path) != 0)
        return 1;
    char *argv[] = {"/usr/bin/shasum", "-a", "256", image_path, NULL};
    char output[1024] = {0};
    int status = 0;
    if (!digest_is_hex(expected) ||
        run_capture(argv, output, sizeof output, STATES_OBSERVE_MS, &status) != 0 || status != 0)
        return 1;
    for (size_t i = 0u; i < 64u; i++) {
        char c = output[i];
        if (c >= 'A' && c <= 'F')
            c = (char)(c + ('a' - 'A'));
        char e = expected[i];
        if (e >= 'A' && e <= 'F')
            e = (char)(e + ('a' - 'A'));
        if (c != e)
            return 1;
    }
    return output[64] == ' ' || output[64] == '\t' ? 0 : 1;
}

static int host_run(int argc, char **argv, const char *self_path)
{
    if (argc != 13)
        return 2;
    host_session host;
    memset(&host, 0, sizeof host);
    host.udp_fd = -1;
    host.reserved_fd = -1;
    if (copy_path(host.retroarch, sizeof host.retroarch, argv[2]) != 0 ||
        copy_path(host.core, sizeof host.core, argv[3]) != 0 ||
        copy_path(host.core_sha256, sizeof host.core_sha256, argv[4]) != 0 ||
        copy_path(host.info, sizeof host.info, argv[5]) != 0 ||
        copy_path(host.rom, sizeof host.rom, argv[6]) != 0 ||
        copy_path(host.ra_dir, sizeof host.ra_dir, argv[7]) != 0 ||
        copy_path(host.save_dir, sizeof host.save_dir, argv[8]) != 0 ||
        copy_path(host.output_root, sizeof host.output_root, argv[9]) != 0 ||
        copy_path(host.compare, sizeof host.compare, argv[10]) != 0 ||
        copy_path(host.final_shot, sizeof host.final_shot, argv[11]) != 0)
        return 2;
    char *end = NULL;
    unsigned long frames = strtoul(argv[12], &end, 10);
    if (end == argv[12] || *end != '\0' || frames != STATES_HOST_FRAME_CAP ||
        !digest_is_hex(host.core_sha256))
        return 2;
    host.frames = (unsigned)frames;
    FILE *fixture = fopen(host.rom, "rb");
    if (fixture == NULL)
        return 1;
    uint32_t crc = 0xffffffffu;
    int byte;
    while ((byte = fgetc(fixture)) != EOF) {
        crc ^= (uint32_t)(uint8_t)byte;
        for (unsigned bit = 0u; bit < 8u; bit++)
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    if (ferror(fixture) || fclose(fixture) != 0)
        return 1;
    host.fixture_crc = ~crc;
    if (validate_host_paths(&host) != 0 || core_digest_matches(host.core, host.core_sha256) != 0)
        return 1;
    host.start_ms = states_now_ms();
    if (access("/usr/bin/sandbox-exec", X_OK) != 0 ||
        sandbox_helper(&host, self_path) != 0 ||
        states_now_ms() >= host.start_ms + STATES_WORK_MS || update_config_port(&host) != 0 ||
        host_validate_runahead_config(&host) != 0) {
        fprintf(stderr, "states: fail-closed sandbox UDP containment preflight\n");
        if (host.reserved_fd >= 0)
            close(host.reserved_fd);
        return 1;
    }
    if (host_prepare_media(&host) != 0) {
        fprintf(stderr, "states: could not create the complete HSC flush sentinel\n");
        if (host.reserved_fd >= 0)
            close(host.reserved_fd);
        return 1;
    }
    if (spawn_retroarch(&host) != 0) {
        fprintf(stderr, "states: could not prove the unique owned RetroArch endpoint\n");
        (void)host_cleanup(&host, STATES_CLEANUP_MS);
        if (host.reserved_fd >= 0)
            close(host.reserved_fd);
        return 1;
    }
    states_ops ops = {
        .user = &host,
        .start_ms = host.start_ms,
        .now_ms = host_now,
        .preflight = host_preflight,
        .verify_identity = host_verify_identity,
        .read_progress = host_read_progress,
        .state_absent = host_state_absent,
        .save_once = host_save_once,
        .state_created = host_state_created,
        .frame_once = host_frame_once,
        .load_once = host_load_once,
        .capture = host_capture,
        .status = host_status_op,
        .pause_toggle_once = host_pause_toggle_once,
        .rewind_once = host_rewind_once,
        .runahead_toggle_once = host_runahead_toggle_once,
        .durable_sentinel = host_durable_sentinel,
        .save_files_once = host_save_files_once,
        .durable_committed = host_durable_committed,
        .cleanup = host_cleanup,
    };
    states_result result;
    int rc = states_save_load(&ops, &result);
    if (file_contains(host.log_path, host.core) != 0 ||
        core_digest_matches(host.core, host.core_sha256) != 0)
        rc = 1;
    if (host.last_capture[0] != '\0' &&
        copy_file(host.last_capture, host.final_shot) != 0)
        rc = 1;
    printf("states host: explicit_advances=%u/%u presented_budget=%u/%u startup_progress=%u "
           "rewind_progress=%u,%u,%u runahead_secondary_instance=false owned_retroarch_children=1 "
           "baseline_frames=%u baseline_ms=%" PRIu64 " runahead_frames=%u runahead_ms=%" PRIu64
           " save_files_flush=1 duration_ms=%" PRIu64 " core_sha256=%s core_path=%s\n",
           result.frames, STATES_FRAME_CAP, STATES_PRESENTED_FRAME_BUDGET, host.frames,
           result.startup_progress,
           result.rewind_progress[0], result.rewind_progress[1], result.rewind_progress[2],
           result.baseline_frames, result.baseline_elapsed_ms, result.runahead_frames,
           result.runahead_elapsed_ms, result.elapsed_ms, host.core_sha256, host.core);
    if (rc != 0)
        fprintf(stderr, "states host failed: %s\n", result.error != NULL ? result.error :
                "host log, final artifact, or cleanup validation failed");
    return rc;
}
#endif

int main(int argc, char **argv)
{
#if STATES_POSIX
    if (argc == 2 && strcmp(argv[1], "--mock-child") == 0)
        return mock_child_main(0);
    if (argc == 2 && strcmp(argv[1], "--mock-child-hung") == 0)
        return mock_child_main(1);
    if (argc == 2 && strcmp(argv[1], "--unrelated-child") == 0) {
        for (;;)
            (void)pause();
    }
#if defined(__APPLE__)
    if (argc == 3 && strcmp(argv[1], "--udp-helper") == 0)
        return udp_helper_main(argv[2]);
#endif
    char resolved[PATH_MAX];
    const char *self = argc > 0 && realpath(argv[0], resolved) != NULL ? resolved : argv[0];
#else
    const char *self = "portable-mock";
#endif
    if (argc == 2 && strcmp(argv[1], "--controls") == 0)
        return run_controls(self);
    if (argc == 2 && strcmp(argv[1], "--fault-controls") == 0)
        return run_fault_controls(self);
#if defined(__APPLE__)
    if (argc == 3 && strcmp(argv[1], "--preflight") == 0)
        return preflight_only(self, argv[2]);
    if (argc > 1 && strcmp(argv[1], "--host") == 0)
        return host_run(argc, argv, self);
#endif
    fprintf(stderr, "usage: states --controls | --host ...\n");
    return 2;
}
