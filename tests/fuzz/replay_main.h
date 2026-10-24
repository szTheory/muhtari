/* Replay main shared by the fuzz harnesses, seeded from szTheory/nesturbator
   @9f9386c661f6 tests/fuzz/replay_main.h (same author, MIT). Included at the
   end of a harness that defines LLVMFuzzerTestOneInput. Built only with
   MUHTARI_FUZZ_REPLAY, so the libFuzzer binary keeps libFuzzer's own main.

   Usage: <replay> [--seeds] [file...]. --seeds first runs every in-memory
   seed from seeds.c, then each file is run in order. With no input at all
   it prints "no corpus inputs" and exits 1, so a replay that silently ran
   nothing cannot pass. MUHTARI_REPLAY_NAME prefixes the diagnostics. */
#ifndef MUHTARI_FUZZ_REPLAY_MAIN_H
#define MUHTARI_FUZZ_REPLAY_MAIN_H

#ifdef MUHTARI_FUZZ_REPLAY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "seeds.h"

#ifndef MUHTARI_REPLAY_NAME
#define MUHTARI_REPLAY_NAME "fuzz.regress"
#endif

static int replay_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    long length;
    uint8_t *bytes;
    if (file == NULL) {
        fprintf(stderr, MUHTARI_REPLAY_NAME ": cannot open corpus input: %s\n", path);
        return 0;
    }
    if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET) != 0 || (unsigned long)length > 64ul * 1024ul * 1024ul) {
        fprintf(stderr, MUHTARI_REPLAY_NAME ": invalid corpus input: %s\n", path);
        (void)fclose(file);
        return 0;
    }
    bytes = (uint8_t *)malloc(length == 0 ? 1u : (size_t)length);
    if (bytes == NULL) {
        fprintf(stderr, MUHTARI_REPLAY_NAME ": out of memory reading input: %s\n", path);
        (void)fclose(file);
        return 0;
    }
    if ((size_t)length != 0u && fread(bytes, 1u, (size_t)length, file) != (size_t)length) {
        fprintf(stderr, MUHTARI_REPLAY_NAME ": short read: %s\n", path);
        free(bytes);
        (void)fclose(file);
        return 0;
    }
    if (fclose(file) != 0) {
        fprintf(stderr, MUHTARI_REPLAY_NAME ": close failed: %s\n", path);
        free(bytes);
        return 0;
    }
    (void)LLVMFuzzerTestOneInput(bytes, (size_t)length);
    free(bytes);
    return 1;
}

/* Runs every in-memory seed; returns the number run, 0 on failure. */
static uint32_t replay_seeds(void)
{
    uint8_t *buf = (uint8_t *)calloc(SEEDS_SIZE_MAX, 1u);
    if (buf == NULL) {
        fputs(MUHTARI_REPLAY_NAME ": out of memory building seeds\n", stderr);
        return 0;
    }
    uint32_t i = 0;
    for (; i < seeds_count(); i++) {
        size_t n = seeds_build(i, buf);
        (void)LLVMFuzzerTestOneInput(buf, n);
    }
    free(buf);
    return i;
}

int main(int argc, char **argv)
{
    int first = 1;
    unsigned long inputs = 0;
    if (argc > 1 && strcmp(argv[1], "--seeds") == 0) {
        uint32_t ran = replay_seeds();
        if (ran == 0u) {
            return 1;
        }
        inputs += ran;
        first = 2;
    }
    for (int i = first; i < argc; ++i) {
        if (!replay_file(argv[i])) {
            return 1;
        }
        inputs++;
    }
    if (inputs == 0u) {
        fputs(MUHTARI_REPLAY_NAME ": no corpus inputs\n", stderr);
        return 1;
    }
    printf(MUHTARI_REPLAY_NAME ": %lu inputs\n", inputs);
    return 0;
}
#endif

#endif
