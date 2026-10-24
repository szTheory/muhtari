/* Actual runner consumer and generated, owned cartridge; runtime bytes stay in build/. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../check.h"

int runner_entry(int argc, char **argv);
#define main runner_entry
#include "../../runner/main.c"
#undef main
#include "../../runner/stop.c"

static void write_bytes(const char *path, const void *p, size_t n)
{
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(p, 1, n, f), n);
        CHECK_EQ_U64(fclose(f), 0);
    }
}

static void cartridge(const char *path)
{
    uint8_t bytes[128 + 4096];
    memset(bytes, 0, sizeof bytes);
    bytes[0] = 4;
    memcpy(bytes + 1, "ATARI7800", 9);
    bytes[50] = 0x10;
    bytes[55] = bytes[56] = 1;
    bytes[58] = 1;
    memcpy(bytes + 100, "ACTUAL CART DATA STARTS HERE", 28);
    memset(bytes + 128, 0xEA, 4096);
    /* Increment imported SRAM once, then loop for the rest of the frame. */
    static const uint8_t code[] = {0xEE, 0x00, 0x10, 0x4C, 0x03, 0xF0};
    memcpy(bytes + 128, code, sizeof code);
    bytes[128 + 0xFFC] = 0;
    bytes[128 + 0xFFD] = 0xF0;
    write_bytes(path, bytes, sizeof bytes);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    char rom[1024], save[1024];
    snprintf(rom, sizeof rom, "%s/game.a78", argv[1]);
    snprintf(save, sizeof save, "%s/game.sav", argv[1]);
    cartridge(rom);
    uint8_t original[2048], got[2049];
    memset(original, 0x5A, sizeof original);
    write_bytes(save, original, sizeof original);
    char *args[] = {"muhtari-run", "--frames", "1", "--save-dir", argv[1], rom};
    CHECK_EQ_U64(runner_entry(6, args), 0);
    FILE *f = fopen(save, "rb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fread(got, 1, sizeof got, f), sizeof original);
        CHECK_EQ_U64(fclose(f), 0);
        original[0]++;
        CHECK(memcmp(original, got, sizeof original) == 0);
    }
    CHECK_DONE();
}
