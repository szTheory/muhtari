/* muhtari-test-images DIR: writes every seed image (tests/fuzz/seeds.h, from
   seeds.c; the fuzz preset also builds it with the run target's run_seeds.c)
   into DIR, an existing directory, one file per seed under its stable name,
   and linear-v3-16k-body, that seed's image without its 128-byte header,
   whose hashes the runner tests compare with --probe's headerless ones, and
   magic-only-128, a header and nothing more, the one length at which the
   headerless hashes cover the whole file although the magic is there, and
   not-a-cartridge-4k, an image the test hash list identifies and the core
   refuses (the local driver's fixture). The images exist only in the build directory; none is
   tracked. Exit 0 when every file is written, 1 on a write failure, 2 on usage. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cart/images.h"
#include "../fuzz/seeds.h"

static int write_one(const char *dir, const char *name, const uint8_t *p, size_t n)
{
    size_t len = strlen(dir) + 1u + strlen(name) + 1u;
    char *path = (char *)malloc(len);
    if (path == NULL) {
        fputs("muhtari-test-images: out of memory\n", stderr);
        return 0;
    }
    (void)snprintf(path, len, "%s/%s", dir, name);
    FILE *f = fopen(path, "wb");
    int ok = f != NULL && fwrite(p, 1u, n, f) == n;
    if (f != NULL && fclose(f) != 0) {
        ok = 0;
    }
    if (!ok) {
        fprintf(stderr, "muhtari-test-images: cannot write %s\n", path);
    }
    free(path);
    return ok;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fputs("usage: muhtari-test-images DIR\n", stderr);
        return 2;
    }
    uint8_t *buf = (uint8_t *)malloc(SEEDS_SIZE_MAX);
    if (buf == NULL) {
        fputs("muhtari-test-images: out of memory\n", stderr);
        return 1;
    }
    int ok = 1;
    uint32_t written = 0;
    for (uint32_t i = 0; ok && i < seeds_count(); i++) {
        size_t n = seeds_build(i, buf);
        ok = write_one(argv[1], seeds_name(i), buf, n);
        written++;
        if (ok && strcmp(seeds_name(i), "activision") == 0) {
            ok = write_one(argv[1], "activision-unknown-body", buf + IMAGES_HEADER,
                           n - IMAGES_HEADER);
            written++;
        }
        if (ok && strcmp(seeds_name(i), "linear-v3-16k") == 0) {
            ok = write_one(argv[1], "linear-v3-16k-body", buf + IMAGES_HEADER, n - IMAGES_HEADER);
            written++;
            /* The same cartridge with its header's TV byte saying PAL. */
            buf[57] = 1u;
            ok = ok && write_one(argv[1], "linear-v3-16k-pal", buf, n);
            written++;
        }
    }
    if (ok) {
        images_header(buf, 3u, "Magic only", 16u * IMAGES_KIB, 0u);
        ok = write_one(argv[1], "magic-only-128", buf, IMAGES_HEADER);
        written++;
    }
    if (ok) {
        size_t n = images_seeded(buf, 4u * IMAGES_KIB, IMAGES_SEED_NOT_CART);
        ok = write_one(argv[1], "not-a-cartridge-4k", buf, n);
        written++;
    }
    for (uint32_t swapped = 0u; ok && swapped < 2u; swapped++) {
        images_header(buf, 4u, "Activision tags", 131072u, 0x0200u);
        buf[55] = 5u;
        (void)images_activision(buf + IMAGES_HEADER);
        if (swapped != 0u) {
            images_swap_halves(buf + IMAGES_HEADER, 131072u);
        }
        ok = write_one(argv[1], swapped != 0u ? "activision-am" : "activision-om",
                       buf + IMAGES_HEADER, 131072u);
        ok = ok && write_one(argv[1], swapped != 0u ? "activision-am-a78" : "activision-om-a78",
                             buf, IMAGES_HEADER + 131072u);
        written += 2u;
        if (swapped != 0u) {
            buf[IMAGES_HEADER + 131072u] = 0xAAu;
            buf[IMAGES_HEADER + 131073u] = 0x55u;
            buf[IMAGES_HEADER + 131074u] = 0x01u;
            ok = ok && write_one(argv[1], "activision-am-trailer", buf + IMAGES_HEADER, 131075u);
            ok =
                ok && write_one(argv[1], "activision-am-trailer-a78", buf, IMAGES_HEADER + 131075u);
            written += 2u;
        }
    }
    free(buf);
    if (ok) {
        printf("muhtari-test-images: %u images in %s\n", (unsigned)written, argv[1]);
    }
    return ok ? 0 : 1;
}
