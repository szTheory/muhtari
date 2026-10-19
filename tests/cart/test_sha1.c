/* SHA-1 against the FIPS 180-4 example messages and the one-million-'a'
   message, through the core archive. */
#include "sha1.h"

#include "../check.h"

#include <string.h>

static void hex(const uint8_t digest[20], char text[41])
{
    static const char digits[] = "0123456789abcdef";
    for (uint32_t i = 0; i < 20u; i++) {
        text[2u * i] = digits[digest[i] >> 4];
        text[2u * i + 1u] = digits[digest[i] & 0x0fu];
    }
    text[40] = '\0';
}

/* The digest of msg fed in one call must read want. */
static void check_message(const char *msg, const char *want)
{
    struct muhtari__sha1 ctx;
    uint8_t digest[20];
    char got[41];
    muhtari__sha1_init(&ctx);
    muhtari__sha1_update(&ctx, msg, strlen(msg));
    muhtari__sha1_final(&ctx, digest);
    hex(digest, got);
    if (strcmp(got, want) != 0) {
        fprintf(stderr, "SHA-1(\"%.20s\"): expected %s got %s\n", msg, want, got);
        check_failures++;
    }
}

/* One million 'a' bytes in chunks of 1, 63, 64 and 65 bytes, in turn, so
   updates start and end at every place in a block. */
static void check_million_a(void)
{
    static const size_t chunks[4] = {1, 63, 64, 65};
    uint8_t buf[65];
    struct muhtari__sha1 ctx;
    uint8_t digest[20];
    char got[41];
    size_t left = 1000000u;
    memset(buf, 'a', sizeof buf);
    muhtari__sha1_init(&ctx);
    for (uint32_t i = 0; left > 0u; i = (i + 1u) % 4u) {
        size_t n = chunks[i] < left ? chunks[i] : left;
        muhtari__sha1_update(&ctx, buf, n);
        left -= n;
    }
    muhtari__sha1_final(&ctx, digest);
    hex(digest, got);
    if (strcmp(got, "34aa973cd4c4daa4f61eeb2bdbad27316534016f") != 0) {
        fprintf(stderr,
                "SHA-1(1000000 x 'a'): expected 34aa973cd4c4daa4f61eeb2bdbad27316534016f "
                "got %s\n",
                got);
        check_failures++;
    }
}

int main(void)
{
    check_message("abc", "a9993e364706816aba3e25717850c26c9cd0d89d");
    check_message("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                  "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
    check_message("", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    check_million_a();
    CHECK_DONE();
}
