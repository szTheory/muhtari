/* Owned maintainer generator; formula and MIT architecture attribution in
   docs/hardware/audio.md. Floating point never enters the shipped core. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "sha256.h"

#define PI 3.14159265358979323846

static double i0(double x)
{
    double sum = 1.0, term = 1.0;
    for (unsigned k = 1u; k < 100u; k++) {
        term *= x * x / (4.0 * k * k);
        sum += term;
        if (term < 1e-18 * sum) {
            break;
        }
    }
    return sum;
}

static double response(double x)
{
    double sinc = x == 0.0 ? 1.0 : sin(PI * x) / (PI * x);
    return sinc * i0(8.0 * sqrt(fmax(0.0, 1.0 - (x / 7.5) * (x / 7.5)))) / i0(8.0);
}

static void generate(unsigned phases, unsigned subdivisions, int32_t rows[128][16])
{
    for (unsigned p = 0; p < phases; p++) {
        int32_t sum = 0;
        for (unsigned j = 0; j < 16u; j++) {
            double center = (double)j - 7.0 - (double)p / phases;
            double lo = fmax(-7.5, center - 0.5), hi = fmin(7.5, center + 0.5);
            double area = 0.0;
            if (hi > lo) {
                double interior = 0.0;
                for (unsigned k = 1u; k < subdivisions; k++) {
                    interior +=
                        (k % 2u != 0u ? 4.0 : 2.0) * response(lo + k * (hi - lo) / subdivisions);
                }
                area = (hi - lo) / (3.0 * subdivisions) * (response(lo) + response(hi) + interior);
            }
            rows[p][j] = (int32_t)floor(area * 8388608.0 + 0.5);
            sum += rows[p][j];
        }
        rows[p][p < phases / 2u ? 7u : 8u] += 8388608 - sum;
    }
}

static void fingerprint(const int32_t rows[128][16], unsigned phases, char hex[65])
{
    muhtari__sha256 sha;
    uint8_t digest[32];
    muhtari__sha256_init(&sha);
    for (unsigned p = 0; p < phases; p++) {
        for (unsigned j = 0; j < 16u; j++) {
            uint32_t value = (uint32_t)rows[p][j];
            uint8_t bytes[4];
            for (unsigned b = 0; b < 4u; b++) {
                bytes[b] = (uint8_t)(value >> (8u * b));
            }
            muhtari__sha256_update(&sha, bytes, sizeof bytes);
        }
    }
    muhtari__sha256_final(&sha, digest);
    muhtari__sha256_hex(digest, hex);
}

int main(int argc, char **argv)
{
    int32_t rows[128][16], refined[128][16];
    if (argc == 2 && strcmp(argv[1], "--check") == 0) {
        generate(64u, 128u, rows);
        for (unsigned subdivisions = 256u; subdivisions <= 512u; subdivisions *= 2u) {
            generate(64u, subdivisions, refined);
            if (memcmp(rows, refined, 64u * sizeof rows[0]) != 0) {
                fprintf(stderr, "Q23 convergence failed at %u subdivisions\n", subdivisions);
                return 1;
            }
        }
        if (MUHTARI__SYNTH_PHASES != 64u ||
            memcmp(rows, muhtari__synth_kernel, 64u * sizeof rows[0]) != 0) {
            fprintf(stderr, "generated coefficients differ from production\n");
            return 1;
        }
        char hex[65];
        fingerprint(rows, 64u, hex);
        if (strcmp(hex, "8fdfc75effc7565eff4996b71ad966f4c0d327909ede230d052dc58bc239d66e") != 0) {
            fprintf(stderr, "coefficient fingerprint mismatch: %s\n", hex);
            return 1;
        }
        printf("1024 production cells agree at Simpson128/256/512; LE-int32 SHA256 %s\n", hex);
        return 0;
    }
    if (argc == 4 && strcmp(argv[1], "--emit") == 0 &&
        (strcmp(argv[2], "64") == 0 || strcmp(argv[2], "128") == 0) &&
        (strcmp(argv[3], "128") == 0 || strcmp(argv[3], "256") == 0 ||
         strcmp(argv[3], "512") == 0)) {
        unsigned phases = (unsigned)atoi(argv[2]);
        generate(phases, (unsigned)atoi(argv[3]), rows);
        puts("const int32_t muhtari__synth_kernel[MUHTARI__SYNTH_PHASES][16] = {");
        for (unsigned p = 0; p < phases; p++) {
            printf("    {");
            for (unsigned j = 0; j < 16u; j++) {
                printf("%s%d", j == 0u ? "" : ", ", (int)rows[p][j]);
            }
            puts("},");
        }
        puts("};");
        return ferror(stdout) != 0;
    }
    fprintf(stderr, "usage: muhtari-audio-coefficients --check | --emit 64|128 128|256|512\n");
    return 2;
}
