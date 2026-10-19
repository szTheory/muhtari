/* Cartridge identification: the A78 header, the 7800 and 2600 tables, the
   footer test of a headerless image and the board geometry
   (docs/hardware/cartridge.md). Identification is pure; only a load that
   identification accepts allocates. */
#include <string.h>

#include "cartdb.h"
#include "internal.h"
#include "sha1.h"

#define CART_HEADER_SIZE 128u
#define CART_KIB 1024u

/* A78 header offsets (docs/hardware/cartridge.md, The A78 header). */
#define A78_VERSION 0u
#define A78_MAGIC 1u
#define A78_TITLE 17u
#define A78_ROM_SIZE 49u
#define A78_CART_TYPE 53u
#define A78_PORT1 55u
#define A78_PORT2 56u
#define A78_TV 57u
#define A78_SAVE 58u
#define A78_XM 63u
#define A78_MAPPER 64u
#define A78_MAPPER_OPTIONS 65u
#define A78_AUDIO 66u
#define A78_IRQ 68u

/* Bytes 1-9 of an A78 header. An array, not a string literal: MSVC on ARM64
   gives a literal an external COMDAT name outside the muhtari_ namespace. */
static const uint8_t cart_magic[9] = {'A', 'T', 'A', 'R', 'I', '7', '8', '0', '0'};

/* The cart-type word's bits (docs/hardware/cartridge.md, Cart type). */
#define CT_POKEY_4000 0x0001u
#define CT_SUPERGAME 0x0002u
#define CT_RAM_4000 0x0004u
#define CT_ROM_4000 0x0008u
#define CT_BANK6_4000 0x0010u
#define CT_BANKED_RAM 0x0020u
#define CT_POKEY_450 0x0040u
#define CT_FRACTALUS 0x0080u
#define CT_ACTIVISION 0x0100u
#define CT_ABSOLUTE 0x0200u
#define CT_POKEY_440 0x0400u
#define CT_YM2151 0x0800u
#define CT_SOUPER 0x1000u
#define CT_BANKSET 0x2000u
#define CT_HALT_RAM 0x4000u
#define CT_POKEY_800 0x8000u

/* The tables identification reads, passed in by the caller. */
struct cart_tables {
    const struct muhtari__cartdb_key *keys;
    uint32_t key_count;
    const struct muhtari__cartdb_entry *entries;
    uint32_t entry_count;
    const struct muhtari__cartdb_layout *layouts;
    uint32_t layout_count;
    const uint8_t (*prefix)[8];
    uint32_t prefix_count;
};

/* What a header or a table entry says the cartridge holds. */
struct cart_board {
    uint32_t board, options, ram, sound;
};

static uint32_t be16(const uint8_t *p)
{
    return ((uint32_t)p[0] << 8) | (uint32_t)p[1];
}

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* Whether a body of n bytes fits the board's geometry
   (docs/hardware/cartridge.md, Geometry). A refused board has none here. */
static int cart_fits(const struct cart_board *b, size_t n)
{
    if (b->board == MUHTARI_BOARD_LINEAR) {
        return (n % (4u * CART_KIB) == 0u && n >= 4u * CART_KIB && n <= 48u * CART_KIB) ||
               n == 64u * CART_KIB;
    }
    if (b->board == MUHTARI_BOARD_SUPERGAME) {
        if (n % (16u * CART_KIB) != 0u || n > 1024u * CART_KIB) {
            return 0;
        }
        size_t banks = n / (16u * CART_KIB);
        if ((b->options & MUHTARI_BOARD_ROM_AT_4000) != 0u) {
            /* 2^k banks behind one fixed bank at $4000. */
            return banks >= 3u && ((banks - 1u) & (banks - 2u)) == 0u;
        }
        return banks >= 2u;
    }
    return 0;
}

/* Clears the cart-type bits that cannot hold together, in MAME's order,
   and reports whether any were (docs/hardware/cartridge.md, Cart type). */
static uint32_t cart_resolve_v3(uint32_t t, int *conflict)
{
    uint32_t special = t & (CT_FRACTALUS | CT_ACTIVISION | CT_ABSOLUTE | CT_SOUPER | CT_BANKSET);
    uint32_t at_4000 = CT_RAM_4000 | CT_BANK6_4000 | CT_BANKED_RAM;
    *conflict = (special & (special - 1u)) != 0u;
    if ((t & (CT_ACTIVISION | CT_ABSOLUTE)) != 0u && (t & 0x00FFu) != 0u) {
        *conflict = 1;
        t &= ~0x00FFu;
    }
    if (special == 0u && (t & CT_SUPERGAME) == 0u &&
        (t & (CT_ROM_4000 | CT_BANK6_4000 | CT_BANKED_RAM)) != 0u) {
        *conflict = 1;
        t |= CT_SUPERGAME;
    }
    /* POKEY at $4000 stays beside ROM at $4000, write-only. */
    if ((t & CT_POKEY_4000) != 0u && (t & at_4000) != 0u) {
        *conflict = 1;
        t &= ~CT_POKEY_4000;
    }
    if ((t & CT_RAM_4000) != 0u && (t & (CT_ROM_4000 | CT_BANK6_4000 | CT_BANKED_RAM)) != 0u) {
        *conflict = 1;
        t &= ~CT_RAM_4000;
    }
    if ((t & CT_ROM_4000) != 0u && (t & (CT_BANK6_4000 | CT_BANKED_RAM)) != 0u) {
        *conflict = 1;
        t &= ~CT_ROM_4000;
    }
    if ((t & CT_BANK6_4000) != 0u && (t & CT_BANKED_RAM) != 0u) {
        *conflict = 1;
        t &= ~CT_BANK6_4000;
    }
    return t;
}

/* The board a resolved cart-type word describes (docs/hardware/cartridge.md,
   Cart type). */
static void cart_decode_v3(uint32_t t, struct cart_board *b)
{
    memset(b, 0, sizeof *b);
    b->board = (t & CT_SUPERGAME) != 0u ? MUHTARI_BOARD_SUPERGAME : MUHTARI_BOARD_LINEAR;
    if ((t & CT_POKEY_4000) != 0u) {
        b->sound |= MUHTARI_SOUND_POKEY_4000;
    }
    if ((t & CT_POKEY_450) != 0u) {
        b->sound |= MUHTARI_SOUND_POKEY_450;
    }
    if ((t & CT_POKEY_440) != 0u) {
        b->sound |= MUHTARI_SOUND_POKEY_440;
    }
    if ((t & CT_POKEY_800) != 0u) {
        b->sound |= MUHTARI_SOUND_POKEY_800;
    }
    if ((t & CT_YM2151) != 0u) {
        b->sound |= MUHTARI_SOUND_YM2151;
    }
    if ((t & CT_RAM_4000) != 0u) {
        b->ram = 16u * CART_KIB;
    }
    if ((t & CT_ROM_4000) != 0u) {
        b->options |= MUHTARI_BOARD_ROM_AT_4000;
    }
    if ((t & CT_BANK6_4000) != 0u) {
        b->options |= MUHTARI_BOARD_BANK6_AT_4000;
    }
    if ((t & CT_BANKED_RAM) != 0u) {
        b->options |= MUHTARI_BOARD_BANKED_RAM;
        b->ram = 32u * CART_KIB;
    }
    if ((t & CT_HALT_RAM) != 0u) {
        b->options |= MUHTARI_BOARD_HALT_BANKED_RAM;
        b->ram = 32u * CART_KIB;
    }
    if ((t & CT_FRACTALUS) != 0u) {
        b->board = MUHTARI_BOARD_FRACTALUS;
        b->ram = 2u * CART_KIB;
    }
    if ((t & CT_ABSOLUTE) != 0u) {
        b->board = MUHTARI_BOARD_ABSOLUTE;
    }
    if ((t & CT_ACTIVISION) != 0u) {
        b->board = MUHTARI_BOARD_ACTIVISION;
    }
    if ((t & CT_BANKSET) != 0u) {
        b->board = MUHTARI_BOARD_BANKSET;
    }
    if ((t & CT_SOUPER) != 0u) {
        b->board = MUHTARI_BOARD_SOUPER;
    }
}

/* The board the v4 block at bytes 64-69 describes; 0 when the block does
   not validate (docs/hardware/cartridge.md, The v4 block). */
static int cart_decode_v4(const uint8_t *h, struct cart_board *b)
{
    static const uint32_t boards[5] = {MUHTARI_BOARD_LINEAR, MUHTARI_BOARD_SUPERGAME,
                                       MUHTARI_BOARD_ACTIVISION, MUHTARI_BOARD_ABSOLUTE,
                                       MUHTARI_BOARD_SOUPER};
    /* POKEY codes 0-5 of the audio word's bits 0-2. */
    static const uint32_t pokeys[6] = {0u,
                                       MUHTARI_SOUND_POKEY_440,
                                       MUHTARI_SOUND_POKEY_450,
                                       MUHTARI_SOUND_POKEY_450 | MUHTARI_SOUND_POKEY_440,
                                       MUHTARI_SOUND_POKEY_800,
                                       MUHTARI_SOUND_POKEY_4000};
    uint32_t mapper = h[A78_MAPPER];
    uint32_t opt = h[A78_MAPPER_OPTIONS];
    uint32_t audio = be16(h + A78_AUDIO);
    uint32_t irq = be16(h + A78_IRQ);
    uint32_t device;
    if (mapper > 4u || (audio & ~0x3Fu) != 0u || (audio & 7u) > 5u || (irq & ~7u) != 0u) {
        return 0;
    }
    if (mapper == 0u) {
        if ((opt & 0x7Cu) != 0u) {
            return 0;
        }
        device = opt & 3u;
    } else if (mapper == 1u) {
        device = opt & 7u;
        if ((opt & 0x78u) != 0u || device > 6u) {
            return 0;
        }
    } else {
        if ((opt & 0x7Fu) != 0u) {
            return 0;
        }
        device = 0u;
    }
    memset(b, 0, sizeof *b);
    b->board = boards[mapper];
    b->sound = pokeys[audio & 7u];
    if ((audio & 0x08u) != 0u) {
        b->sound |= MUHTARI_SOUND_YM2151;
    }
    if ((audio & 0x10u) != 0u) {
        b->sound |= MUHTARI_SOUND_COVOX;
    }
    switch (device) {
    case 1u: /* 16K RAM */
        b->ram = 16u * CART_KIB;
        break;
    case 2u: /* 8K EXRAM/A8, read as the Fractalus mirror RAM */
        b->board = MUHTARI_BOARD_FRACTALUS;
        b->ram = 2u * CART_KIB;
        break;
    case 3u: /* 32K EXRAM/M2, read as halt-banked RAM */
        b->options = MUHTARI_BOARD_HALT_BANKED_RAM;
        b->ram = 32u * CART_KIB;
        break;
    case 4u: /* EXROM */
        b->options = MUHTARI_BOARD_ROM_AT_4000;
        break;
    case 5u: /* EXFIX */
        b->options = MUHTARI_BOARD_BANK6_AT_4000;
        break;
    case 6u: /* 32K EXRAM/X2 */
        b->options = MUHTARI_BOARD_BANKED_RAM;
        b->ram = 32u * CART_KIB;
        break;
    default:
        break;
    }
    if ((opt & 0x80u) != 0u) {
        b->board = MUHTARI_BOARD_BANKSET;
    }
    return 1;
}

/* Copies at most 32 title bytes up to the first NUL, a byte outside
   $20-$7E as '?', then drops trailing spaces. out->title is zero. */
static void cart_title(const uint8_t *t, muhtari_cart_info *out)
{
    uint32_t len = 0u;
    for (uint32_t i = 0u; i < 32u && t[i] != 0u; i++) {
        uint8_t c = t[i];
        if (c < 0x20u || c > 0x7Eu) {
            out->warnings |= MUHTARI_CART_WARN_TITLE_NONPRINTABLE;
            c = (uint8_t)'?';
        }
        out->title[len++] = c;
    }
    while (len > 0u && out->title[len - 1u] == (uint8_t)' ') {
        out->title[--len] = 0u;
    }
}

static void cart_sha1(const uint8_t *p, size_t n, uint8_t digest[20])
{
    struct muhtari__sha1 ctx;
    muhtari__sha1_init(&ctx);
    muhtari__sha1_update(&ctx, p, n);
    muhtari__sha1_final(&ctx, digest);
}

/* The first key equal to digest, or key_count when there is none. Equal
   keys are sorted by entry, so the first is the lower entry. */
static uint32_t cart_first_key(const uint8_t digest[20], const struct cart_tables *t)
{
    uint32_t lo = 0u;
    uint32_t hi = t->key_count;
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2u;
        if (memcmp(t->keys[mid].sha1, digest, 20u) < 0) {
            lo = mid + 1u;
        } else {
            hi = mid;
        }
    }
    return lo;
}

/* The single-chip 7800 entry whose key is digest, or NULL. Of two equal
   keys the first, the lower entry, wins. */
static const struct muhtari__cartdb_entry *cart_find_7800(const uint8_t digest[20],
                                                          const struct cart_tables *t)
{
    for (uint32_t k = cart_first_key(digest, t);
         k < t->key_count && memcmp(t->keys[k].sha1, digest, 20u) == 0; k++) {
        uint32_t e = t->keys[k].entry;
        if (e < t->entry_count && t->entries[e].parts <= 1u) {
            return &t->entries[e];
        }
    }
    return NULL;
}

/* Whether digest is keyed as part of entry e. */
static int cart_has_part(const uint8_t digest[20], uint32_t e, uint32_t part,
                         const struct cart_tables *t)
{
    for (uint32_t k = cart_first_key(digest, t);
         k < t->key_count && memcmp(t->keys[k].sha1, digest, 20u) == 0; k++) {
        if (t->keys[k].entry == e && t->keys[k].part == part) {
            return 1;
        }
    }
    return 0;
}

/* The multi-chip entry whose chips are the slices of body under layout li,
   in order, or NULL. One SHA-1 per slice. */
static const struct muhtari__cartdb_entry *cart_find_slices(const uint8_t *body, uint32_t li,
                                                            const struct cart_tables *t)
{
    const struct muhtari__cartdb_layout *l = &t->layouts[li];
    uint8_t digest[MUHTARI__CARTDB_SLICES_MAX][20];
    uint32_t slices = 0u;
    size_t at = 0u;
    for (; slices < MUHTARI__CARTDB_SLICES_MAX && l->size[slices] != 0u; slices++) {
        cart_sha1(body + at, l->size[slices], digest[slices]);
        at += l->size[slices];
    }
    if (slices == 0u) {
        return NULL;
    }
    for (uint32_t k = cart_first_key(digest[0], t);
         k < t->key_count && memcmp(t->keys[k].sha1, digest[0], 20u) == 0; k++) {
        uint32_t e = t->keys[k].entry;
        if (e >= t->entry_count || t->entries[e].parts <= 1u || t->entries[e].layout != li + 1u ||
            t->keys[k].part != l->part[0]) {
            continue;
        }
        uint32_t i = 1u;
        while (i < slices && cart_has_part(digest[i], e, l->part[i], t)) {
            i++;
        }
        if (i == slices) {
            return &t->entries[e];
        }
    }
    return NULL;
}

/* The 7800 entry for n bytes at body, with digest set to their SHA-1. The
   passes are bounded (docs/hardware/cartridge.md, Hash list): the whole
   body; each slice of every layout whose sizes sum to n; and for a 128 KiB
   miss, the body again with each 16 KiB bank's 8 KiB halves swapped, fed in
   that order with no copy, which only an Activision entry may match. */
static const struct muhtari__cartdb_entry *
cart_lookup(const uint8_t *body, size_t n, const struct cart_tables *t, uint8_t digest[20])
{
    cart_sha1(body, n, digest);
    const struct muhtari__cartdb_entry *e = cart_find_7800(digest, t);
    for (uint32_t li = 0u; e == NULL && li < t->layout_count; li++) {
        uint64_t sum = 0u;
        for (uint32_t i = 0u; i < MUHTARI__CARTDB_SLICES_MAX && t->layouts[li].size[i] != 0u; i++) {
            sum += t->layouts[li].size[i];
        }
        if (sum == n) {
            e = cart_find_slices(body, li, t);
        }
    }
    if (e == NULL && n == 128u * CART_KIB) {
        struct muhtari__sha1 ctx;
        uint8_t swapped[20];
        muhtari__sha1_init(&ctx);
        for (size_t bank = 0u; bank < n; bank += 16u * CART_KIB) {
            muhtari__sha1_update(&ctx, body + bank + 8u * CART_KIB, 8u * CART_KIB);
            muhtari__sha1_update(&ctx, body + bank, 8u * CART_KIB);
        }
        muhtari__sha1_final(&ctx, swapped);
        e = cart_find_7800(swapped, t);
        if (e != NULL && e->board != MUHTARI_BOARD_ACTIVISION) {
            e = NULL;
        }
    }
    return e;
}

static void cart_entry_board(const struct muhtari__cartdb_entry *e, struct cart_board *b)
{
    b->board = e->board;
    b->options = e->board_options;
    b->ram = (uint32_t)e->ram_kib * CART_KIB;
    b->sound = e->sound;
}

/* The region a list entry gives: PAL for a PAL-only entry, else NTSC. */
static uint32_t cart_entry_region(const struct muhtari__cartdb_entry *e)
{
    return (e->flags & (MUHTARI__CARTDB_NTSC | MUHTARI__CARTDB_PAL)) == MUHTARI__CARTDB_PAL
               ? MUHTARI_REGION_PAL
               : MUHTARI_REGION_NTSC;
}

static void cart_set_board(muhtari_cart_info *out, const struct cart_board *b)
{
    out->board = b->board;
    out->board_options = b->options;
    out->ram_size = b->ram;
    out->sound = b->sound;
}

/* Boards a loaded cartridge cannot run yet. */
static int cart_refused(const struct cart_board *b)
{
    return (b->board != MUHTARI_BOARD_LINEAR && b->board != MUHTARI_BOARD_SUPERGAME) ||
           (b->options & MUHTARI_BOARD_HALT_BANKED_RAM) != 0u;
}

static uint32_t cart_port(uint32_t code, muhtari_cart_info *out)
{
    if (code > MUHTARI_PORT_MEGA7800) {
        out->warnings |= MUHTARI_CART_WARN_UNKNOWN_PORT;
        return MUHTARI_PORT_NONE;
    }
    return code;
}

/* An image whose bytes 1-9 are the A78 magic. */
static muhtari_status cart_header(const uint8_t *img, size_t n, const struct cart_tables *t,
                                  muhtari_cart_info *out)
{
    if (n < CART_HEADER_SIZE) {
        return MUHTARI_STATUS_ERR_TRUNCATED_IMAGE;
    }
    uint32_t field = be32(img + A78_ROM_SIZE);
    if (field == 0u) {
        return MUHTARI_STATUS_ERR_INVALID_IMAGE;
    }
    size_t body = n - CART_HEADER_SIZE;
    if (body == 0u) {
        return MUHTARI_STATUS_ERR_TRUNCATED_IMAGE;
    }
    size_t mapped = field < body ? field : body;

    out->kind = MUHTARI_KIND_7800;
    out->source = MUHTARI_SOURCE_HEADER;
    out->flags = MUHTARI_CART_HEADER;
    out->rom_size = (uint32_t)mapped;
    uint32_t version = img[A78_VERSION];
    out->a78_version = version;
    out->a78_rom_size = field;
    out->a78_cart_type = be16(img + A78_CART_TYPE);
    out->a78_port1 = img[A78_PORT1];
    out->a78_port2 = img[A78_PORT2];
    out->a78_tv = img[A78_TV];
    out->a78_save = img[A78_SAVE];
    out->a78_xm = img[A78_XM];
    /* A version above 4 is read as 4; below 4 the v4 block is reserved. */
    if (version > 4u) {
        out->warnings |= MUHTARI_CART_WARN_VERSION_ABOVE_4;
    }
    if (version >= 4u) {
        out->a78_mapper = img[A78_MAPPER];
        out->a78_mapper_options = img[A78_MAPPER_OPTIONS];
        out->a78_audio = be16(img + A78_AUDIO);
        out->a78_irq = be16(img + A78_IRQ);
    }
    int reserved = (out->a78_tv & 0xF8u) != 0u || (out->a78_save & 0xF8u) != 0u;
    for (uint32_t i = 59u; i < 100u; i++) {
        if ((i <= 62u || i >= 70u || (i >= A78_MAPPER && version < 4u)) && img[i] != 0u) {
            reserved = 1;
        }
    }
    if (reserved) {
        out->warnings |= MUHTARI_CART_WARN_RESERVED_NONZERO;
    }
    if (field != body) {
        out->warnings |= MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH;
    }
    cart_title(img + A78_TITLE, out);

    /* The v4 block wins when it validates and its board fits the body;
       otherwise the cart type applies. */
    int conflict;
    struct cart_board b;
    struct cart_board v4;
    cart_decode_v3(cart_resolve_v3(out->a78_cart_type, &conflict), &b);
    if (conflict) {
        out->warnings |= MUHTARI_CART_WARN_CART_TYPE_CONFLICT;
    }
    if (version >= 4u) {
        if (cart_decode_v4(img, &v4) && (cart_refused(&v4) || cart_fits(&v4, mapped))) {
            b = v4;
        } else {
            out->warnings |= MUHTARI_CART_WARN_HEADER_V4_IGNORED;
        }
    }

    /* The old wiki text put the HSC in bit 2. */
    if ((out->a78_save & 0x05u) != 0u) {
        out->devices |= MUHTARI_DEVICE_HSC;
    }
    if ((out->a78_save & 0x04u) != 0u) {
        out->warnings |= MUHTARI_CART_WARN_LEGACY_SAVE_BIT;
    }
    if ((out->a78_save & 0x02u) != 0u) {
        out->devices |= MUHTARI_DEVICE_SAVEKEY;
    }
    if (out->a78_xm != 0u) {
        out->devices |= MUHTARI_DEVICE_XM;
    }
    if (out->a78_xm > 1u) {
        out->warnings |= MUHTARI_CART_WARN_XM_VALUE;
    }
    out->port1 = cart_port(out->a78_port1, out);
    out->port2 = cart_port(out->a78_port2, out);
    out->region = (out->a78_tv & 1u) != 0u ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;

    /* A table match overrides the header's board, RAM and sound, never its
       controllers. The region stays the header's unless TV bit 2 says
       multi-region, which names none; then the entry's applies (cartridge
       notes, section 1.4). */
    uint8_t digest[20];
    const struct muhtari__cartdb_entry *e = cart_lookup(img + CART_HEADER_SIZE, mapped, t, digest);
    if (e != NULL) {
        if ((e->flags & MUHTARI__CARTDB_NOT_CART) != 0u) {
            return MUHTARI_STATUS_ERR_INVALID_IMAGE;
        }
        struct cart_board listed;
        cart_entry_board(e, &listed);
        if (memcmp(&listed, &b, sizeof b) != 0) {
            out->warnings |= MUHTARI_CART_WARN_HEADER_CORRECTED;
        }
        b = listed;
        out->source = MUHTARI_SOURCE_HASH;
        out->flags |= MUHTARI_CART_HASH_MATCH;
        if ((out->a78_tv & 0x04u) != 0u) {
            out->region = cart_entry_region(e);
        }
    }
    cart_set_board(out, &b);
    if (cart_refused(&b)) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
    }
    if (!cart_fits(&b, mapped)) {
        return field > body ? MUHTARI_STATUS_ERR_TRUNCATED_IMAGE : MUHTARI_STATUS_ERR_INVALID_IMAGE;
    }
    return MUHTARI_STATUS_OK;
}

/* Whether the 8-byte SHA-1 prefix of digest is in the sorted 2600 list. */
static int cart_find_2600(const uint8_t digest[20], const uint8_t (*prefix)[8],
                          uint32_t prefix_count)
{
    uint32_t lo = 0u;
    uint32_t hi = prefix_count;
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2u;
        int c = memcmp(prefix[mid], digest, 8u);
        if (c == 0) {
            return 1;
        }
        if (c < 0) {
            lo = mid + 1u;
        } else {
            hi = mid;
        }
    }
    return 0;
}

/* The NTSC BIOS's test of the last 8 bytes, $FFF8-$FFFF, with the region bit
   ignored: $FFF8's high nibble is $F; $FFF9's low nibble is 3 or 7 and its
   high nibble, the start page, is at least 4 and at or below the reset
   vector's page; the reset vector is neither $0000 nor $FFFF
   (A7800-CARTRIDGES-AND-PERIPHERALS section 1.5). */
static int cart_footer(const uint8_t *f)
{
    uint32_t reset = (uint32_t)f[4] | ((uint32_t)f[5] << 8);
    uint32_t start = f[1] & 0xF0u;
    uint32_t low = f[1] & 0x0Fu;
    return (f[0] & 0xF0u) == 0xF0u && (low == 3u || low == 7u) && start >= 0x40u &&
           start <= (reset >> 8) && reset != 0x0000u && reset != 0xFFFFu;
}

/* The sizes of 2600 images: 2 KiB multiples up to 64 KiB, 10,495 bytes, and
   multiples of the 8,448-byte Supercharger load. */
static int cart_2600_size(size_t n)
{
    return (n % (2u * CART_KIB) == 0u && n <= 64u * CART_KIB) || n == 10495u || n % 8448u == 0u;
}

/* An image without the A78 magic: the 7800 list, the 2600 list, the footer
   test and the 2600 sizes, in that order. n is at least 1. */
static muhtari_status cart_headerless(const uint8_t *img, size_t n, const struct cart_tables *t,
                                      muhtari_cart_info *out)
{
    uint8_t digest[20];
    out->rom_size = (uint32_t)n;
    const struct muhtari__cartdb_entry *e = cart_lookup(img, n, t, digest);
    if (e != NULL) {
        if ((e->flags & MUHTARI__CARTDB_NOT_CART) != 0u) {
            return MUHTARI_STATUS_ERR_INVALID_IMAGE;
        }
        struct cart_board b;
        cart_entry_board(e, &b);
        cart_set_board(out, &b);
        out->kind = MUHTARI_KIND_7800;
        out->source = MUHTARI_SOURCE_HASH;
        out->flags = MUHTARI_CART_HASH_MATCH;
        out->region = cart_entry_region(e);
        out->port1 = (e->flags & MUHTARI__CARTDB_LIGHTGUN) != 0u ? MUHTARI_PORT_LIGHTGUN
                                                                 : MUHTARI_PORT_PROLINE;
        out->port2 = MUHTARI_PORT_PROLINE;
        return cart_refused(&b) ? MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD : MUHTARI_STATUS_OK;
    }
    if (cart_find_2600(digest, t->prefix, t->prefix_count)) {
        out->kind = MUHTARI_KIND_2600;
        out->source = MUHTARI_SOURCE_HASH;
        out->flags = MUHTARI_CART_HASH_MATCH;
        return MUHTARI_STATUS_ERR_UNSUPPORTED_2600;
    }
    if (n >= 8u && cart_footer(img + n - 8u)) {
        struct cart_board b;
        memset(&b, 0, sizeof b);
        b.board = n <= 48u * CART_KIB ? MUHTARI_BOARD_LINEAR : MUHTARI_BOARD_SUPERGAME;
        if (!cart_fits(&b, n)) {
            return MUHTARI_STATUS_ERR_INVALID_IMAGE;
        }
        cart_set_board(out, &b);
        out->kind = MUHTARI_KIND_7800;
        out->source = MUHTARI_SOURCE_FOOTER;
        out->region = MUHTARI_REGION_NTSC;
        out->port1 = MUHTARI_PORT_PROLINE;
        out->port2 = MUHTARI_PORT_PROLINE;
        return MUHTARI_STATUS_OK;
    }
    if (cart_2600_size(n)) {
        out->kind = MUHTARI_KIND_2600;
        out->source = MUHTARI_SOURCE_SIZE;
        return MUHTARI_STATUS_ERR_UNSUPPORTED_2600;
    }
    return MUHTARI_STATUS_ERR_INVALID_IMAGE;
}

muhtari_status muhtari__cart_identify(const uint8_t *img, size_t n,
                                      const struct muhtari__cartdb_key *keys, uint32_t key_count,
                                      const struct muhtari__cartdb_entry *entries,
                                      uint32_t entry_count,
                                      const struct muhtari__cartdb_layout *layouts,
                                      uint32_t layout_count, const uint8_t (*prefix)[8],
                                      uint32_t prefix_count, muhtari_cart_info *out)
{
    if (out == NULL || (img == NULL && n > 0u)) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    if (n > MUHTARI_IMAGE_SIZE_MAX) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE;
    }
    memset(out, 0, sizeof *out);
    out->size = (uint32_t)sizeof *out;
    if (n == 0u) {
        return MUHTARI_STATUS_ERR_INVALID_IMAGE;
    }
    struct cart_tables t = {keys,    key_count,    entries, entry_count,
                            layouts, layout_count, prefix,  prefix_count};
    if (n >= 10u && memcmp(img + A78_MAGIC, cart_magic, sizeof cart_magic) == 0) {
        return cart_header(img, n, &t, out);
    }
    return cart_headerless(img, n, &t, out);
}

/* Identification against the bundled tables. */
static muhtari_status cart_identify_bundled(const void *image, size_t n, muhtari_cart_info *out)
{
    return muhtari__cart_identify((const uint8_t *)image, n, muhtari__cartdb_7800_keys,
                                  muhtari__cartdb_7800_key_count, muhtari__cartdb_7800_entries,
                                  muhtari__cartdb_7800_entry_count, muhtari__cartdb_7800_layouts,
                                  muhtari__cartdb_7800_layout_count, muhtari__cartdb_2600_prefix,
                                  muhtari__cartdb_2600_prefix_count, out);
}

muhtari_status muhtari_probe(const void *image, size_t n, muhtari_cart_info *out)
{
    if (out == NULL || (image == NULL && n > 0u)) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st =
        muhtari__check_size(out, MUHTARI__CART_INFO_SIZE_FIRST, (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    if (n > MUHTARI_IMAGE_SIZE_MAX) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE;
    }
    muhtari_cart_info info;
    st = cart_identify_bundled(image, n, &info);
    /* A refusal a host can show is written; every other error writes nothing. */
    if (st == MUHTARI_STATUS_OK || st == MUHTARI_STATUS_ERR_UNSUPPORTED_2600 ||
        st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD) {
        info.size = out->size;
        muhtari__write_out(out, &info, (uint32_t)sizeof info);
    }
    return st;
}

muhtari_status muhtari__cart_load(muhtari *m, const void *image, size_t n)
{
    if (m == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    /* Validation comes first, so a refused image allocates nothing. */
    muhtari_cart_info info;
    muhtari_status st = cart_identify_bundled(image, n, &info);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_allocator a = m->allocator;
    uint8_t *rom = (uint8_t *)a.alloc(a.user, info.rom_size);
    if (rom == NULL) {
        return MUHTARI_STATUS_ERR_NO_MEMORY;
    }
    /* The mapped bytes start after the header, when there is one. */
    size_t at = (info.flags & MUHTARI_CART_HEADER) != 0u ? CART_HEADER_SIZE : 0u;
    memcpy(rom, (const uint8_t *)image + at, info.rom_size);
    if (m->cart.rom != NULL) {
        a.free(a.user, m->cart.rom, m->cart.info.rom_size);
    }
    m->cart.rom = rom;
    m->cart.info = info;
    return MUHTARI_STATUS_OK;
}
