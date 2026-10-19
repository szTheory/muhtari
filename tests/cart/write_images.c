/* muhtari-test-images DIR: writes every seed image (tests/fuzz/seeds.c) into
   DIR, an existing directory, one file per seed under its stable name, and
   linear-v3-16k-body, that seed's image without its 128-byte header, whose
   hashes the runner tests compare with --probe's headerless ones, and
   magic-only-128, a header and nothing more, the one length at which the
   headerless hashes cover the whole file although the magic is there. The
   images exist only in the build directory; none is tracked. Exit 0 when
   every file is written, 1 on a write failure, 2 on usage. */
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
        if (ok && strcmp(seeds_name(i), "linear-v3-16k") == 0) {
            ok = write_one(argv[1], "linear-v3-16k-body", buf + IMAGES_HEADER, n - IMAGES_HEADER);
            written++;
        }
    }
    if (ok) {
        images_header(buf, 3u, "Magic only", 16u * IMAGES_KIB, 0u);
        ok = write_one(argv[1], "magic-only-128", buf, IMAGES_HEADER);
        written++;
    }
    free(buf);
    if (ok) {
        printf("muhtari-test-images: %u images in %s\n", (unsigned)written, argv[1]);
    }
    return ok ? 0 : 1;
}
