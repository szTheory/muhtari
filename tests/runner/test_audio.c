/* Exercise the actual runner with deterministic frame-boundary signals.
   Only its frame call is substituted; the wrapper invokes the public core. */
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "internal.h"
#include "../check.h"

static unsigned calls, stop_after, fail_after;
static muhtari_status wrapped_frame(muhtari *m, muhtari_frame *io)
{
    muhtari_status st = muhtari_run_frame(m, io);
    calls++;
    if (calls == fail_after) {
        /* A refused frame can have supplied tentative raw evidence. */
        if (m->observer != NULL)
            m->observer(m->observer_user, UINT64_C(0x123456789), 7u, 15u);
        io->audio_count = 0u;
        return MUHTARI_STATUS_ERR_BIOS_REJECTED_CART;
    }
    if (calls == stop_after)
        CHECK_EQ_U64(raise(SIGINT), 0);
    return st;
}
int runner_entry(int argc, char **argv);
#define main runner_entry
#define muhtari_run_frame wrapped_frame
#include "../../runner/main.c"
#undef muhtari_run_frame
#undef main
/* Test-local access resets the signal flag between independent runs. */
#include "../../runner/stop.c"

static void canonical(void)
{
    static const uint8_t raw_bytes[] = {0x89, 0x67, 0x45, 0x23, 0x01, 0, 0, 0, 7, 15, 0};
    static const int16_t samples[] = {0, 1, -1, INT16_MIN, INT16_MAX};
    static const uint8_t pcm_bytes[] = {0, 0, 1, 0, 255, 255, 0, 128, 255, 127};
    muhtari__sha256 a, b;
    uint8_t x[32], y[32];
    muhtari__sha256_init(&a);
    muhtari__sha256_init(&b);
    audio_raw(&a, UINT64_C(0x123456789), 7, 15);
    muhtari__sha256_update(&b, raw_bytes, sizeof raw_bytes);
    muhtari__sha256_final(&a, x);
    muhtari__sha256_final(&b, y);
    CHECK(memcmp(x, y, 32) == 0);
    muhtari__sha256_init(&a);
    muhtari__sha256_init(&b);
    audio_pcm(&a, samples, 5);
    muhtari__sha256_update(&b, pcm_bytes, sizeof pcm_bytes);
    muhtari__sha256_final(&a, x);
    muhtari__sha256_final(&b, y);
    CHECK(memcmp(x, y, 32) == 0);
}
static void run_case(const char *capture, unsigned count, unsigned failure, int dump)
{
    CHECK(freopen(capture, "w", stdout) != NULL);
    stop_flag = 0;
    calls = 0;
    stop_after = count;
    fail_after = failure;
    muhtari__stop_install();
    if (count == 0u && failure == 0u && !dump)
        CHECK_EQ_U64(raise(SIGINT), 0);
    options opt;
    memset(&opt, 0, sizeof opt);
    opt.frames = 10;
    opt.hash_audio = 1;
    uint32_t dump_n = 1;
    const char *path = "";
    if (dump) {
        opt.dump_count = 1;
        opt.dump_frames = &dump_n;
        opt.dump_paths = &path;
    }
    int rc = run(&opt, NULL);
    CHECK_EQ_U64(rc, failure != 0u ? MUHTARI__EXIT_REFUSED : dump ? 1u : 0u);
    CHECK_EQ_U64(fflush(stdout), 0);
    FILE *f = fopen(capture, "r");
    CHECK(f != NULL);
    if (f == NULL)
        return;
    char text[2048];
    size_t n = fread(text, 1, sizeof text - 1u, f);
    text[n] = '\0';
    CHECK_EQ_U64(fclose(f), 0);
    if (failure != 0u || dump) {
        CHECK(strstr(text, "audio_") == NULL);
        return;
    }
    CHECK_EQ_U64(calls, count);
    char expected[100];
    snprintf(expected, sizeof expected, "audio_frames %u\n", count);
    CHECK(strstr(text, expected) != NULL);
    uint32_t samples_count = (uint32_t)((uint64_t)count * 119402u * 88u / 13125u);
    snprintf(expected, sizeof expected, "audio_samples %u\n", samples_count);
    CHECK(strstr(text, expected) != NULL);
    muhtari__sha256 h;
    uint8_t digest[32], zero[2] = {0, 0};
    char hex[65];
    muhtari__sha256_init(&h);
    for (uint32_t j = 0; j < samples_count; j++)
        muhtari__sha256_update(&h, zero, 2);
    muhtari__sha256_final(&h, digest);
    muhtari__sha256_hex(digest, hex);
    snprintf(expected, sizeof expected, "audio_pcm_sha256 %s\n", hex);
    CHECK(strstr(text, expected) != NULL);
    CHECK(strstr(text,
                 "audio_transitions_sha256 "
                 "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\n") != NULL);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    canonical();
    run_case(argv[1], 0, 0, 0);
    run_case(argv[1], 1, 0, 0);
    run_case(argv[1], 3, 0, 0);
    run_case(argv[1], 10, 1, 0);
    run_case(argv[1], 10, 3, 0);
    run_case(argv[1], 10, 0, 1);
    CHECK_DONE();
}
