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
/* Activision OM banks and compatibility hotspot range (cartridge preparation
   sections Headerless images and Every scheme; docs/hardware/cartridge.md, Activision). */
#define CART_ACT_BANK 0x4000u
#define CART_ACT_HALF 0x2000u
#define CART_ACT_SELECT_FIRST 0xFF80u
#define CART_ACT_SELECT_LAST 0xFF8Fu

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
    uint32_t unsupported;
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
    if (b->board == MUHTARI_BOARD_BANKSET) {
        /* Equal CPU-then-MARIA sets; sizes and RAM matrix are checked before
           allocation (cartridge.md, BankSet). All runtime offsets use n/2. */
        if ((n & 1u) != 0u || n > 1024u * CART_KIB) {
            return 0;
        }
        size_t half = n / 2u;
        int linear = half == 32u * CART_KIB || half == 48u * CART_KIB || half == 52u * CART_KIB;
        int switched =
            half == 128u * CART_KIB || half == 256u * CART_KIB || half == 512u * CART_KIB;
        int ram = (b->options == 0u && (b->ram == 0u || b->ram == 16u * CART_KIB)) ||
                  (b->options == MUHTARI_BOARD_HALT_BANKED_RAM && b->ram == 32u * CART_KIB);
        return (linear || switched) && ram && (b->ram == 0u || half == 32u * CART_KIB || switched);
    }
    if (b->board == MUHTARI_BOARD_FRACTALUS) {
        return n == 32u * CART_KIB && b->ram == 2u * CART_KIB && b->options == 0u;
    }
    if (b->board == MUHTARI_BOARD_ABSOLUTE) {
        return n == 64u * CART_KIB && b->options == 0u && b->ram == 0u;
    }
    if (b->board == MUHTARI_BOARD_ACTIVISION) {
        return n == 128u * CART_KIB && b->options == 0u && b->ram == 0u;
    }
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
    if ((t & CT_BANKSET) == 0u && (t & CT_POKEY_4000) != 0u && (t & at_4000) != 0u) {
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
    if ((audio & 0x20u) != 0u) {
        b->sound |= MUHTARI_SOUND_ADPCM;
    }
    switch (device) {
    case 1u: /* 16K RAM */
        b->ram = 16u * CART_KIB;
        break;
    case 2u: /* 8K EXRAM/A8, read as the Fractalus mirror RAM */
        b->board = MUHTARI_BOARD_FRACTALUS;
        b->ram = 2u * CART_KIB;
        /* Only Linear/A8 identifies this board. Valid SuperGame/A8 has
           undocumented wiring; refuse it without v3 fallback (cartridge.md,
           The v4 block). Hash metadata can still override this decision. */
        b->unsupported = mapper != 0u;
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
        b->unsupported = mapper > 1u || device == 2u;
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
static const struct muhtari__cartdb_entry *cart_lookup(const uint8_t *body, size_t n,
                                                       const struct cart_tables *t,
                                                       uint8_t digest[20], uint32_t *orientation)
{
    *orientation = 0u;
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
        if (e != NULL) {
            *orientation = MUHTARI_CART_ACTIVISION_HALF_SWAPPED;
        }
    }
    return e;
}

static void cart_entry_board(const struct muhtari__cartdb_entry *e, struct cart_board *b)
{
    b->unsupported = 0u;
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
    if (b->board == MUHTARI_BOARD_BANKSET) {
        return b->unsupported != 0u ||
               (b->options & ~(uint32_t)MUHTARI_BOARD_HALT_BANKED_RAM) != 0u ||
               (b->ram != 0u && b->ram != 16u * CART_KIB && b->ram != 32u * CART_KIB);
    }
    return (b->board != MUHTARI_BOARD_LINEAR && b->board != MUHTARI_BOARD_SUPERGAME &&
            b->board != MUHTARI_BOARD_ABSOLUTE && b->board != MUHTARI_BOARD_ACTIVISION &&
            b->board != MUHTARI_BOARD_FRACTALUS && b->board != MUHTARI_BOARD_BANKSET) ||
           b->unsupported != 0u || (b->options & MUHTARI_BOARD_HALT_BANKED_RAM) != 0u;
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
                                  muhtari_cart_info *out, uint32_t *entry)
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
        if (cart_decode_v4(img, &v4) &&
            (v4.board == MUHTARI_BOARD_BANKSET || cart_refused(&v4) || cart_fits(&v4, mapped))) {
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
    uint32_t orientation;
    const struct muhtari__cartdb_entry *e =
        cart_lookup(img + CART_HEADER_SIZE, mapped, t, digest, &orientation);
    if (e != NULL) {
        *entry = (uint32_t)(e - t->entries);
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
        out->flags |= MUHTARI_CART_HASH_MATCH | orientation;
        if ((out->a78_tv & 0x04u) != 0u) {
            out->region = cart_entry_region(e);
        }
    }
    if (e == NULL && b.board == MUHTARI_BOARD_ACTIVISION) {
        out->warnings |= MUHTARI_CART_WARN_ACTIVISION_LAYOUT_ASSUMED;
    }
    cart_set_board(out, &b);
    if (cart_refused(&b)) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
    }
    if (b.board == MUHTARI_BOARD_BANKSET && field != body) {
        return field > body ? MUHTARI_STATUS_ERR_TRUNCATED_IMAGE : MUHTARI_STATUS_ERR_INVALID_IMAGE;
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
                                      muhtari_cart_info *out, uint32_t *entry)
{
    uint8_t digest[20];
    out->rom_size = (uint32_t)n;
    uint32_t orientation;
    const struct muhtari__cartdb_entry *e = cart_lookup(img, n, t, digest, &orientation);
    if (e != NULL) {
        *entry = (uint32_t)(e - t->entries);
        if ((e->flags & MUHTARI__CARTDB_NOT_CART) != 0u) {
            return MUHTARI_STATUS_ERR_INVALID_IMAGE;
        }
        struct cart_board b;
        cart_entry_board(e, &b);
        cart_set_board(out, &b);
        out->kind = MUHTARI_KIND_7800;
        out->source = MUHTARI_SOURCE_HASH;
        out->flags = MUHTARI_CART_HASH_MATCH | orientation;
        out->region = cart_entry_region(e);
        out->port1 = (e->flags & MUHTARI__CARTDB_LIGHTGUN) != 0u ? MUHTARI_PORT_LIGHTGUN
                                                                 : MUHTARI_PORT_PROLINE;
        out->port2 = MUHTARI_PORT_PROLINE;
        if (cart_refused(&b)) {
            return MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
        }
        return cart_fits(&b, n) ? MUHTARI_STATUS_OK : MUHTARI_STATUS_ERR_INVALID_IMAGE;
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

muhtari_status
muhtari__cart_identify_entry(const uint8_t *img, size_t n, const struct muhtari__cartdb_key *keys,
                             uint32_t key_count, const struct muhtari__cartdb_entry *entries,
                             uint32_t entry_count, const struct muhtari__cartdb_layout *layouts,
                             uint32_t layout_count, const uint8_t (*prefix)[8],
                             uint32_t prefix_count, muhtari_cart_info *out, uint32_t *entry)
{
    uint32_t matched = UINT32_MAX;
    if (entry != NULL) {
        *entry = UINT32_MAX;
    }
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
    muhtari_status st;
    if (n >= 10u && memcmp(img + A78_MAGIC, cart_magic, sizeof cart_magic) == 0) {
        st = cart_header(img, n, &t, out, &matched);
    } else {
        st = cart_headerless(img, n, &t, out, &matched);
    }
    if (entry != NULL) {
        *entry = matched;
    }
    return st;
}

muhtari_status muhtari__cart_identify(const uint8_t *img, size_t n,
                                      const struct muhtari__cartdb_key *keys, uint32_t key_count,
                                      const struct muhtari__cartdb_entry *entries,
                                      uint32_t entry_count,
                                      const struct muhtari__cartdb_layout *layouts,
                                      uint32_t layout_count, const uint8_t (*prefix)[8],
                                      uint32_t prefix_count, muhtari_cart_info *out)
{
    return muhtari__cart_identify_entry(img, n, keys, key_count, entries, entry_count, layouts,
                                        layout_count, prefix, prefix_count, out, NULL);
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
    return muhtari__cart_install(m, image, &info, MUHTARI_EXPANSION_OFF);
}

muhtari_status muhtari__cart_install(muhtari *m, const void *image, const muhtari_cart_info *info,
                                     uint32_t expansion)
{
    muhtari_allocator a = m->allocator;
    /* ROM, cartridge RAM and physical expansion RAM share one checked
       allocation. Record its exact length for replacement and destroy. */
    size_t bytes = info->rom_size;
    size_t expansion_bytes = expansion != MUHTARI_EXPANSION_OFF ? 131072u : 0u;
    if (bytes > SIZE_MAX - info->ram_size) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE;
    }
    bytes += info->ram_size;
    if (bytes > SIZE_MAX - expansion_bytes) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE;
    }
    bytes += expansion_bytes;
    uint8_t *rom = (uint8_t *)a.alloc(a.user, bytes);
    if (rom == NULL) {
        return MUHTARI_STATUS_ERR_NO_MEMORY;
    }
    /* The mapped bytes start after the header, when there is one. */
    size_t at = (info->flags & MUHTARI_CART_HEADER) != 0u ? CART_HEADER_SIZE : 0u;
    memcpy(rom, (const uint8_t *)image + at, info->rom_size);
    /* Only a matched Activision swap pass authorizes conversion of this new
       private copy. Raw caller/host identity remains unchanged (section 1.4). */
    if ((info->flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) != 0u &&
        info->board == MUHTARI_BOARD_ACTIVISION && info->rom_size == 128u * CART_KIB) {
        for (uint32_t bank = 0u; bank < info->rom_size; bank += CART_ACT_BANK) {
            for (uint32_t i = 0u; i < CART_ACT_HALF; i++) {
                uint8_t byte = rom[bank + i];
                rom[bank + i] = rom[bank + CART_ACT_HALF + i];
                rom[bank + CART_ACT_HALF + i] = byte;
            }
        }
    }
    if (m->cart.rom != NULL) {
        a.free(a.user, m->cart.rom, m->cart.allocation_bytes);
    }
    m->cart.rom = rom;
    m->cart.ram = info->ram_size != 0u ? rom + info->rom_size : NULL;
    m->cart.allocation_bytes = bytes;
    m->cart.expansion.ram = expansion_bytes != 0u ? rom + info->rom_size + info->ram_size : NULL;
    m->cart.expansion.kind = expansion;
    m->cart.info = *info;
    /* Only an accepted v4 block supplies wiring; raw probe fields survive
       an ignored block (cartridge.md, The v4 block). */
    m->cart.irq_mask =
        info->a78_version >= 4u && (info->warnings & MUHTARI_CART_WARN_HEADER_V4_IGNORED) == 0u
            ? (uint8_t)(info->a78_irq & 3u)
            : 0u;
    muhtari__cart_reset(m);
    return MUHTARI_STATUS_OK;
}

#define CART_BANK 0x4000u /* a SuperGame bank, 16 KiB */

void muhtari__cart_reset(muhtari *m)
{
    m->cart.bank = 0u;
    m->cart.ram_bank = 0u;
    m->cart.expansion.control = 0u;
    m->cart.expansion.cpu_pages = 0u;
    m->cart.expansion.maria_pages = 0u;
    uint32_t sound = m->cart.info.sound;
    m->cart.pokey_at[0] = m->cart.expansion.kind >= MUHTARI_EXPANSION_XBOARD ||
                                  (sound & MUHTARI_SOUND_POKEY_450) != 0u
                              ? 0x0450u
                          : (sound & MUHTARI_SOUND_POKEY_4000) != 0u ? 0x4000u
                          : (sound & MUHTARI_SOUND_POKEY_800) != 0u  ? 0x0800u
                                                                     : 0u;
    m->cart.pokey_at[1] = (sound & MUHTARI_SOUND_POKEY_440) != 0u ? 0x0440u : 0u;
    uint8_t count = (uint8_t)((m->cart.pokey_at[0] != 0u) + (m->cart.pokey_at[1] != 0u));
    muhtari__synth_reset(m, m->bus.clock_origin_tick, count);
    m->synth.secondary = (uint8_t)(count == 1u && m->cart.pokey_at[1] != 0u);
    muhtari__pokey_reset(m, 0u);
    muhtari__pokey_reset(m, 1u);
    /* Only the zero fill exists so far; every RAM_FILL value gives it. */
    if (m->cart.ram != NULL) {
        memset(m->cart.ram, 0, m->cart.info.ram_size);
    }
    if (m->cart.expansion.ram != NULL) {
        memset(m->cart.expansion.ram, 0, 131072u);
    }
}

/* XBoard's eight 16 KiB banks and RAM chip select (cartridge preparation
   section 2.1, XBoard). CPU and MARIA use the same physical bank. */
static uint8_t *cart_expansion_at(const struct muhtari__cart *c, uint16_t addr, int maria)
{
    if (c->expansion.kind == MUHTARI_EXPANSION_XBOARD && (c->expansion.control & 8u) != 0u &&
        addr >= 0x4000u && addr < 0x8000u) {
        return &c->expansion.ram[(uint32_t)(c->expansion.control & 7u) * CART_BANK +
                                 (addr & (CART_BANK - 1u))];
    }
    /* XM has two independently enabled 8 KiB windows and separate page
       nibbles for each electrical owner (cartridge.md, XM subset). */
    if (c->expansion.kind == MUHTARI_EXPANSION_XM && addr >= 0x4000u && addr < 0x8000u) {
        unsigned window = addr >= 0x6000u ? 1u : 0u;
        if ((c->expansion.control & (32u << window)) != 0u) {
            uint8_t pages = maria ? c->expansion.maria_pages : c->expansion.cpu_pages;
            uint32_t page = (pages >> (window * 4u)) & 15u;
            return &c->expansion.ram[page * 8192u + (addr & 8191u)];
        }
    }
    return NULL;
}

/* Expansion's chip select gates register access, never the installed chip
   or its clocks/mixer identity (cartridge preparation section 2.1). */
static int cart_pokey_decoded(const struct muhtari__cart *c, unsigned chip)
{
    return chip != 0u || c->expansion.kind < MUHTARI_EXPANSION_XBOARD ||
           (c->expansion.control & 16u) != 0u;
}

/* XM gates both HSC windows together, independently of physical battery
   attachment (cartridge.md, XM subset and HSC physical inference). */
static int cart_hsc_decoded(const muhtari *m)
{
    return m->battery.hsc_attached != 0u && (m->cart.expansion.kind != MUHTARI_EXPANSION_XM ||
                                             (m->cart.expansion.control & 8u) != 0u);
}

/* The RAM byte a board drives at addr, or NULL. A board with RAM maps it
   at $4000-$7FFF; a banked-RAM board shows the 16 KiB half ram_bank there.
   Fractalus compresses the disconnected address bits; other boards mask
   with ram_size - 1, a power of two, so no header can reach outside RAM
   (A7800-CARTRIDGES-AND-PERIPHERALS section 2.1,
   and section 7 for hostile input). */
static uint8_t *cart_ram_at(const struct muhtari__cart *c, uint16_t addr, int maria)
{
    if (c->ram == NULL || addr < 0x4000u || addr >= 0x8000u) {
        return NULL;
    }
    uint32_t i = (uint32_t)addr - 0x4000u;
    if (c->info.board == MUHTARI_BOARD_FRACTALUS) {
        /* Physical 2 KiB: A8 and upper mirror bits are disconnected;
           A9-A11 compress into RAM A8-A10 (cartridge.md, Fractalus). */
        return &c->ram[((addr & 0x0E00u) >> 1) | (addr & 0x00FFu)];
    }
    if ((c->info.board_options & MUHTARI_BOARD_BANKED_RAM) != 0u) {
        i += (uint32_t)c->ram_bank * CART_BANK;
    }
    if (maria && (c->info.board_options & MUHTARI_BOARD_HALT_BANKED_RAM) != 0u) {
        i += CART_BANK;
    }
    return &c->ram[i & (c->info.ram_size - 1u)];
}

/* The SuperGame bank at addr, $4000-$FFFF (A7800-CARTRIDGES-AND-PERIPHERALS
   section 2.1): the last bank at $C000, the register's at $8000, and at
   $4000 bank 6 with that option, bank 0 on the 2^n + 1 board, else the
   second-to-last. Every number is reduced modulo the bank count. */
static uint32_t cart_sg_bank(const struct muhtari__cart *c, uint16_t addr)
{
    uint32_t banks = c->info.rom_size / CART_BANK;
    if (addr >= 0xC000u) {
        return banks - 1u;
    }
    if (addr >= 0x8000u) {
        /* The 2^n + 1 board's register selects among the banks after bank 0. */
        return (c->info.board_options & MUHTARI_BOARD_ROM_AT_4000) != 0u ? 1u + c->bank : c->bank;
    }
    if ((c->info.board_options & MUHTARI_BOARD_ROM_AT_4000) != 0u) {
        return 0u;
    }
    if ((c->info.board_options & MUHTARI_BOARD_BANK6_AT_4000) != 0u) {
        return 6u % banks;
    }
    return banks - 2u;
}

/* BankSet geometry was checked before install. Fixed and switched offsets
   are within one half, never the doubled image (cartridge.md, BankSet). */
static const uint8_t *cart_bankset_rom_at(const struct muhtari__cart *c, uint16_t addr, int maria)
{
    uint32_t half = c->info.rom_size / 2u;
    uint32_t offset;
    if (half <= 52u * CART_KIB) {
        uint32_t base = 0x10000u - half;
        if (addr < base)
            return NULL;
        offset = (uint32_t)addr - base;
    } else {
        if (addr < 0x4000u)
            return NULL;
        uint32_t banks = half / CART_BANK;
        uint32_t bank = addr >= 0xC000u ? banks - 1u : addr >= 0x8000u ? c->bank : banks - 2u;
        offset = bank * CART_BANK + (addr & (CART_BANK - 1u));
    }
    return &c->rom[(maria ? half : 0u) + offset];
}

/* A linear board maps its ROM so that it ends at $FFFF; below the ROM's
   start nothing drives the bus. A SuperGame board maps three 16 KiB
   windows from $4000 (A7800-CARTRIDGES-AND-PERIPHERALS, section 2.1).
   RAM, where a board has it, answers at $4000-$7FFF instead of ROM. No
   function-pointer table: the board is a switch, so the core keeps no
   mutable global. */
static uint8_t cart_peek_view(const muhtari *m, uint16_t addr, int maria)
{
    const struct muhtari__cart *c = &m->cart;
    if (c->rom == NULL) {
        return m->bus.open_bus;
    }
    const uint8_t *expansion = cart_expansion_at(c, addr, maria);
    if (expansion != NULL) {
        return *expansion;
    }
    /* HSC pass-through decode (docs/hardware/cartridge.md, HSC). The bus
       has already applied MEN/BIOS gates; CPU audio decode precedes this. */
    if (cart_hsc_decoded(m)) {
        if (addr >= 0x1000u && addr < 0x1800u) {
            return m->battery.data[addr - 0x1000u];
        }
        if (addr >= 0x3000u && addr < 0x4000u) {
            return m->battery.rom[addr - 0x3000u];
        }
    }
    const uint8_t *ram = cart_ram_at(c, addr, maria);
    if (ram != NULL) {
        return *ram;
    }
    switch (c->info.board) {
    case MUHTARI_BOARD_BANKSET: {
        const uint8_t *rom = cart_bankset_rom_at(c, addr, maria);
        if (rom != NULL)
            return *rom;
        break;
    }
    case MUHTARI_BOARD_FRACTALUS:
        if (addr >= 0x8000u) {
            return c->rom[addr - 0x8000u];
        }
        break;
    case MUHTARI_BOARD_ACTIVISION: {
        /* OM half order (cartridge preparation, Headerless images and Every scheme).
           The switched A000/C000 halves are reversed within their bank. */
        static const uint32_t fixed[6] = {0x18000u, 0x1A000u, 0x1C000u, 0u, 0u, 0x1E000u};
        if (addr >= 0x4000u) {
            uint32_t window = ((uint32_t)addr - 0x4000u) / CART_ACT_HALF;
            uint32_t offset = fixed[window];
            if (window == 3u || window == 4u) {
                offset = (uint32_t)c->bank * CART_ACT_BANK + (window == 3u ? CART_ACT_HALF : 0u);
            }
            return c->rom[offset + (addr & (CART_ACT_HALF - 1u))];
        }
        break;
    }
    case MUHTARI_BOARD_ABSOLUTE:
        /* Fixed upper 32 KiB; the low window selects either 16 KiB bank
           (cartridge preparation section 2.1, Absolute). */
        if (addr >= 0x8000u) {
            return c->rom[addr];
        }
        if (addr >= 0x4000u) {
            return c->rom[(uint32_t)c->bank * CART_BANK + (addr & (CART_BANK - 1u))];
        }
        break;
    case MUHTARI_BOARD_LINEAR: {
        uint32_t base = 0x10000u - c->info.rom_size;
        if (addr >= base) {
            return c->rom[addr - base];
        }
        break;
    }
    case MUHTARI_BOARD_SUPERGAME:
        if (addr >= 0x4000u) {
            return c->rom[cart_sg_bank(c, addr) * CART_BANK + (addr & (CART_BANK - 1u))];
        }
        break;
    default:
        break;
    }
    return m->bus.open_bus;
}

uint8_t muhtari__cart_peek(const muhtari *m, uint16_t addr)
{
    return cart_peek_view(m, addr, 0);
}

uint8_t muhtari__cart_dma_read(muhtari *m, uint16_t addr, uint64_t tick)
{
    if (m->cart.info.board == MUHTARI_BOARD_BANKSET ||
        m->cart.expansion.kind == MUHTARI_EXPANSION_XM) {
        return cart_peek_view(m, addr, muhtari__bus_maria_selected(m, tick));
    }
    return muhtari__cart_read(m, addr, 1);
}

uint8_t muhtari__cart_read(muhtari *m, uint16_t addr, int by_maria)
{
    /* DMA and observation see board memory, never register side effects. */
    if (!by_maria) {
        for (unsigned chip = 0; chip < 2u; chip++) {
            uint16_t at = m->cart.pokey_at[chip];
            uint16_t size = at == 0x4000u ? 0x4000u : at == 0x0800u ? 0x0800u : 16u;
            if (at != 0u && cart_pokey_decoded(&m->cart, chip) && addr >= at &&
                (uint32_t)addr < (uint32_t)at + size) {
                /* Overlapping ROM/RAM remains readable; POKEY is write-only
                   there (cartridge notes section 2.1). */
                int overlap = at == 0x4000u &&
                              (m->cart.ram != NULL ||
                               (m->cart.info.board_options &
                                (MUHTARI_BOARD_ROM_AT_4000 | MUHTARI_BOARD_BANK6_AT_4000)) != 0u ||
                               m->cart.info.board == MUHTARI_BOARD_ABSOLUTE ||
                               m->cart.info.board == MUHTARI_BOARD_ACTIVISION ||
                               (m->cart.info.board == MUHTARI_BOARD_BANKSET &&
                                m->cart.info.rom_size / 2u > 32768u) ||
                               (m->cart.info.board == MUHTARI_BOARD_LINEAR &&
                                m->cart.info.rom_size > 32768u));
                if (!overlap) {
                    return muhtari__pokey_read(m, chip, (uint8_t)(addr & 15u),
                                               m->bus.ticks + (m->bus.cycle_len == 6u ? 5u : 4u));
                }
            }
        }
    }
    return muhtari__cart_peek(m, addr);
}

/* A write to the board's RAM stores; on a SuperGame board a write to
   $8000-$BFFF sets the bank register: the value modulo the bank count, or
   modulo 2^n on the 2^n + 1 board, which shows bank 1 + the register at
   $8000. A banked-RAM board
   takes the RAM half from D5 of the same write
   (A7800-CARTRIDGES-AND-PERIPHERALS section 2.1). ROM ignores a write. */
void muhtari__cart_write(muhtari *m, uint16_t addr, uint8_t v)
{
    struct muhtari__cart *c = &m->cart;
    if (c->rom == NULL) {
        return;
    }
    /* All sixteen control aliases drive the same XBoard latch
       (cartridge preparation section 2.1, XBoard). */
    if (c->expansion.kind == MUHTARI_EXPANSION_XBOARD && addr >= 0x0470u && addr <= 0x047Fu) {
        c->expansion.control = v;
        return;
    }
    if (c->expansion.kind == MUHTARI_EXPANSION_XM) {
        if (addr == 0x0470u) {
            c->expansion.control = v;
            return;
        }
        if (addr == 0x0478u || addr == 0x047Cu) {
            if (addr == 0x0478u)
                c->expansion.cpu_pages = v;
            else
                c->expansion.maria_pages = v;
            return;
        }
    }
    uint8_t *expansion = cart_expansion_at(c, addr, 0);
    if (expansion != NULL) {
        *expansion = v;
        return;
    }
    for (unsigned chip = 0; chip < 2u; chip++) {
        uint16_t at = c->pokey_at[chip];
        uint16_t size = at == 0x4000u ? 0x4000u : at == 0x0800u ? 0x0800u : 16u;
        if (at != 0u && cart_pokey_decoded(c, chip) && addr >= at &&
            (uint32_t)addr < (uint32_t)at + size) {
            muhtari__pokey_write(m, chip, (uint8_t)(addr & 15u), v,
                                 m->bus.ticks + (m->bus.cycle_len == 6u ? 5u : 4u));
            if (c->info.board != MUHTARI_BOARD_BANKSET)
                return;
        }
    }
    if (cart_hsc_decoded(m) && addr >= 0x1000u && addr < 0x1800u) {
        uint8_t *cell = &m->battery.data[addr - 0x1000u];
        if (*cell != v) {
            *cell = v;
            m->battery.generation++;
        }
        return;
    }
    /* BankSet CPU stores at $C000 redirect to the MARIA half, while reads
       there remain ROM (cartridge.md, BankSet). No shared-RAM redirect. */
    if (c->info.board == MUHTARI_BOARD_BANKSET &&
        (c->info.board_options & MUHTARI_BOARD_HALT_BANKED_RAM) != 0u && addr >= 0xC000u) {
        c->ram[CART_BANK + (uint32_t)addr - 0xC000u] = v;
        return;
    }
    uint8_t *ram = cart_ram_at(c, addr, 0);
    if (ram != NULL) {
        *ram = v;
        return;
    }
    switch (c->info.board) {
    case MUHTARI_BOARD_BANKSET:
        if (c->info.rom_size / 2u >= 128u * CART_KIB && addr >= 0x8000u && addr < 0xC000u) {
            c->bank = (uint8_t)(v % (c->info.rom_size / 2u / CART_BANK));
        }
        break;
    case MUHTARI_BOARD_ACTIVISION:
        /* FF88-FF8F aliasing and bank-zero startup are selected compatibility
           profile policies, not new measurements (cartridge notes, Every scheme). */
        if (addr >= CART_ACT_SELECT_FIRST && addr <= CART_ACT_SELECT_LAST) {
            c->bank = (uint8_t)(addr & 7u);
        }
        break;
    case MUHTARI_BOARD_ABSOLUTE:
        /* Compatibility policy: only $8000, D0 before D1, else retain
           (cartridge preparation section 2.1, Absolute). */
        if (addr == 0x8000u) {
            if ((v & 1u) != 0u) {
                c->bank = 0u;
            } else if ((v & 2u) != 0u) {
                c->bank = 1u;
            }
        }
        break;
    case MUHTARI_BOARD_SUPERGAME:
        if (addr >= 0x8000u && addr < 0xC000u) {
            uint32_t banks = c->info.rom_size / CART_BANK;
            if ((c->info.board_options & MUHTARI_BOARD_ROM_AT_4000) != 0u) {
                c->bank = (uint8_t)(v % (banks - 1u));
            } else {
                c->bank = (uint8_t)(v % banks);
            }
            c->ram_bank = (uint8_t)((v >> 5) & 1u);
        }
        break;
    default:
        break;
    }
}
