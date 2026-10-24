/* Instance lifetime, loading and power cycling, the version and info
   queries, and the size-tag helper. */
#include <string.h>

#include "internal.h"
#include "muhtari_version.h"
#include "sha1.h"

/* The interface numbers this build reports. Each starts at 1, so zero means
   absent; the behaviour revision changes when a frame, audio or state hash
   changes for the same inputs. */
#define MUHTARI__ABI 1u
#define MUHTARI__BEHAVIOUR_REVISION 4u

/* The largest size tag accepted, far above any struct's planned growth. A tag
   promises that many readable bytes; the cap bounds how far a wrong tag can
   make the check read, it does not make a wrong tag safe. */
#define MUHTARI__SIZE_TAG_MAX 4096u

muhtari_status muhtari__check_size(const void *s, uint32_t first, uint32_t ours)
{
    const uint8_t *bytes = (const uint8_t *)s;
    uint32_t size;
    memcpy(&size, s, sizeof size); /* the tag is untrusted: read it alone first */
    if (size < first || size > MUHTARI__SIZE_TAG_MAX) {
        return MUHTARI_STATUS_ERR_STRUCT_SIZE;
    }
    /* A newer caller's struct is accepted only if the fields this build does
       not know are all zero. */
    for (uint32_t i = ours; i < size; i++) {
        if (bytes[i] != 0u) {
            return MUHTARI_STATUS_ERR_STRUCT_SIZE;
        }
    }
    return MUHTARI_STATUS_OK;
}

void muhtari__write_out(void *out, const void *full, uint32_t ours)
{
    uint32_t size;
    memcpy(&size, out, sizeof size);
    memcpy(out, full, size < ours ? size : ours);
}

muhtari_status muhtari_get_versions(muhtari_versions *out)
{
    if (out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st =
        muhtari__check_size(out, MUHTARI__VERSIONS_SIZE_FIRST, (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_versions v;
    memset(&v, 0, sizeof v);
    v.size = out->size;
    v.library_major = MUHTARI_VERSION_MAJOR;
    v.library_minor = MUHTARI_VERSION_MINOR;
    v.library_patch = MUHTARI_VERSION_PATCH;
    v.api = MUHTARI_API_VERSION;
    v.abi = MUHTARI__ABI;
    v.behaviour_revision = MUHTARI__BEHAVIOUR_REVISION;
    muhtari__write_out(out, &v, (uint32_t)sizeof v);
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_create(const muhtari_config *cfg, muhtari **out)
{
    if (cfg == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(cfg, MUHTARI__CONFIG_SIZE_FIRST, (uint32_t)sizeof *cfg);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    /* Both hooks are required: the core has no allocator of its own. */
    muhtari_allocator a = cfg->allocator;
    if (a.alloc == NULL || a.free == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari *m = (muhtari *)a.alloc(a.user, sizeof *m);
    if (m == NULL) {
        return MUHTARI_STATUS_ERR_NO_MEMORY;
    }
    memset(m, 0, sizeof *m);
    m->allocator = a;
    /* The default profile: the ANE and LXA constant three real SALLYs show
       (docs/hardware/sally.md, Unstable opcodes), port B's output register at
       its reset value, zero-filled RAM, and the TIA line at its origin. */
    m->profile.ane_lxa_constant = MUHTARI__ANE_LXA_SALLY;
    m->profile.riot_orb_power_on = 0xFFu;
    m->profile.ram_fill = MUHTARI__RAM_FILL_ZERO;
    m->profile.tia_phase = 0u;
    muhtari__power_on(m);
    *out = m;
    return MUHTARI_STATUS_OK;
}

void muhtari__power_on(muhtari *m)
{
    memset(&m->cpu, 0, sizeof m->cpu);
    memset(&m->bus, 0, sizeof m->bus);
    muhtari__tia_reset(m);
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__pokey_reset(m, 0u);
    muhtari__pokey_reset(m, 1u);
    m->audio_rem = 0u;
    /* The beam starts at line 0, t 0 of the current origin; the first cycle
       recomputes the deadline from MARIA's next line event. */
    muhtari__maria_start(m);
    m->bus.next_deadline = 0u;
    /* Only the zero fill exists so far; every RAM_FILL value gives it. */
    memset(m->ram, 0, sizeof m->ram);
    muhtari__riot_reset(m);
    muhtari__cart_reset(m);
}

void muhtari_destroy(muhtari *m)
{
    if (m == NULL) {
        return;
    }
    muhtari_allocator a = m->allocator;
    (void)muhtari__synth_observe(m, NULL, NULL);
    if (m->cart.rom != NULL) {
        a.free(a.user, m->cart.rom, (size_t)m->cart.info.rom_size + m->cart.info.ram_size);
    }
    a.free(a.user, m, sizeof *m);
}

muhtari_status muhtari_get_info(const muhtari *m, muhtari_info *out)
{
    if (m == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(out, MUHTARI__INFO_SIZE_FIRST, (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = out->size;
    info.width = MUHTARI__WIDTH;
    info.height = MUHTARI__HEIGHT_NTSC;
    /* A frame is 119,402 ticks of twice the colour subcarrier
       (A7800-MARIA section 6.2). */
    info.fps_num = 39375000u;
    info.fps_den = 656711u;
    /* NTSC pixel aspect 6:7 (A7800-MARIA section 6.2). */
    info.par_num = 6u;
    info.par_den = 7u;
    info.sample_rate_num = 48000u;
    info.sample_rate_den = 1u;
    info.max_audio_samples = MUHTARI__MAX_AUDIO_SAMPLES;
    muhtari__write_out(out, &info, (uint32_t)sizeof info);
    return MUHTARI_STATUS_OK;
}

/* A BIOS is 4 KiB or 16 KiB mapped to end at $FFFF, and its reset vector,
   the last bytes but two, must point inside it (A7800-SYSTEM section 7). */
static int bios_valid(const uint8_t *bios, size_t n)
{
    if (n != 4096u && n != MUHTARI__BIOS_SIZE_MAX) {
        return 0;
    }
    uint32_t reset = (uint32_t)bios[n - 4u] | ((uint32_t)bios[n - 3u] << 8);
    return reset >= 0x10000u - (uint32_t)n;
}

/* Puts the loaded content, or the test card, back at power-on. */
static void power_cycle(muhtari *m)
{
    muhtari__ports_reset_counters(m);
    if (m->cart.rom != NULL && m->bios_size != 0u) {
        muhtari__boot_bios(m);
    } else if (m->cart.rom != NULL) {
        muhtari__boot_fast(m);
    } else {
        muhtari__power_on(m);
        m->audio_rem = 0u;
    }
}

/* Only complete appended fields exist for a short caller. Partial fields
   default to zero, even when their alignment padding is present. */
static void media_copy(muhtari_media *dst, const muhtari_media *src)
{
    memset(dst, 0, sizeof *dst);
    memcpy(dst, src, MUHTARI__MEDIA_SIZE_FIRST);
#define MEDIA_FIELD(field) \
    do { \
        if (src->size >= offsetof(muhtari_media, field) + sizeof dst->field) { \
            memcpy(&dst->field, &src->field, sizeof dst->field); \
        } \
    } while (0)
    MEDIA_FIELD(hsc_mode);
    MEDIA_FIELD(hsc_rom);
    MEDIA_FIELD(hsc_rom_size);
    for (unsigned p = 0; p < 2u; p++) {
        if (src->size >= offsetof(muhtari_media, port) + (p + 1u) * sizeof(uint32_t)) {
            memcpy(&dst->port[p], &src->port[p], sizeof dst->port[p]);
        }
    }
#undef MEDIA_FIELD
}

static int battery_serial(uint32_t device)
{
    return device == MUHTARI_PORT_SAVEKEY || device == MUHTARI_PORT_ATARIVOX;
}

/* Resolve the whole storage topology before installation, including serial
   placement. Its pin protocol is owned by the controller ports. */
struct battery_candidate {
    size_t bytes;
    uint32_t hsc_attached, eeprom_port, eeprom_device, firmware_flags;
    uint8_t hsc_sha1[20];
};

static muhtari_status battery_prepare(struct battery_candidate *b, uint8_t rom[4096],
                                      const muhtari_media *media, const muhtari_cart_info *info)
{
    if (media->hsc_mode > MUHTARI_HSC_OFF ||
        ((media->hsc_rom == NULL) != (media->hsc_rom_size == 0u)) ||
        (media->hsc_rom != NULL &&
         (media->hsc_rom_size != 4096u || media->hsc_mode == MUHTARI_HSC_OFF))) {
        return MUHTARI_STATUS_ERR_INVALID_HSC;
    }
    memset(b, 0, sizeof *b);
    b->hsc_attached = (uint32_t)(media->hsc_mode == MUHTARI_HSC_ON ||
        media->hsc_rom != NULL || (media->hsc_mode == MUHTARI_HSC_AUTO &&
                                  (info->devices & MUHTARI_DEVICE_HSC) != 0u));
    for (unsigned p = 0; p < 2u; p++) {
        uint32_t device = media->port[p];
        if (device != 0u && device != MUHTARI_PORT_PROLINE &&
            device != MUHTARI_PORT_JOYSTICK && device != MUHTARI_PORT_UNPLUGGED &&
            !battery_serial(device)) {
            return device <= MUHTARI_PORT_MEGA7800 ? MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE
                                                  : MUHTARI_STATUS_ERR_ARGUMENT;
        }
        if (device == 0u) {
            device = p == 0u ? info->port1 : info->port2;
        }
        if (battery_serial(device)) {
            if (b->eeprom_port != 0u) {
                return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
            }
            b->eeprom_port = p + 1u;
            b->eeprom_device = device;
        }
    }
    if (b->eeprom_port == 0u && (info->devices & MUHTARI_DEVICE_SAVEKEY) != 0u &&
        media->port[1] == 0u) {
        b->eeprom_port = 2u;
        b->eeprom_device = MUHTARI_PORT_SAVEKEY;
    }
    b->bytes = (b->hsc_attached != 0u ? 2048u : 0u) +
               (b->eeprom_port != 0u ? 32768u : 0u);
    if (b->hsc_attached != 0u) {
        if (media->hsc_rom == NULL) {
            /* Signature-only compatibility stub: no firmware routines
               (cartridge preparation section 4.1). */
            memset(rom, 0xFF, 4096u);
            rom[0x900] = 0xC6u;
            rom[0x904] = 0xFEu;
            b->firmware_flags = MUHTARI_HSC_FIRMWARE_STUB;
        } else {
            /* Canonical identity: cartridge preparation CART.06/18/40;
               unknown explicit firmware is accepted with a warning. */
            static const uint8_t known[20] = {
                0xa3, 0xaf, 0x67, 0x69, 0x91, 0x39, 0x1a, 0x6d, 0xd7, 0x16,
                0xc7, 0x90, 0x22, 0xd4, 0x94, 0x72, 0x06, 0xb7, 0x81, 0x64
            };
            memmove(rom, media->hsc_rom, 4096u);
            struct muhtari__sha1 hash;
            muhtari__sha1_init(&hash);
            muhtari__sha1_update(&hash, rom, 4096u);
            muhtari__sha1_final(&hash, b->hsc_sha1);
            b->firmware_flags = memcmp(b->hsc_sha1, known, sizeof known) == 0
                ? MUHTARI_HSC_FIRMWARE_KNOWN : MUHTARI_HSC_FIRMWARE_WARN_UNKNOWN;
            if (rom[0x900] != 0xC6u || rom[0x904] != 0xFEu) {
                b->firmware_flags |= MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE;
            }
        }
    }
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_load(muhtari *m, const muhtari_media *media)
{
    if (m == NULL || media == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st =
        muhtari__check_size(media, MUHTARI__MEDIA_SIZE_FIRST, (uint32_t)sizeof *media);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_media candidate;
    media_copy(&candidate, media);
    media = &candidate;
    if ((media->bios == NULL && media->bios_size != 0u) ||
        (media->region != 0u && media->region != MUHTARI_REGION_NTSC &&
         media->region != MUHTARI_REGION_PAL)) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    /* Everything is validated before anything is applied, so a refused load
       leaves the previous content running. probe refuses a NULL image with
       a size, and an empty one, before any allocation. */
    muhtari_cart_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    st = muhtari_probe(media->image, media->image_size, &info);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    /* The region: the caller's, else the cartridge's, which the probe
       resolved from the header, the hash list, then NTSC. PAL is refused. */
    uint32_t region = media->region != 0u ? media->region : info.region;
    if (region != MUHTARI_REGION_NTSC) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_REGION;
    }
    if (media->bios != NULL && !bios_valid((const uint8_t *)media->bios, media->bios_size)) {
        return MUHTARI_STATUS_ERR_INVALID_BIOS;
    }
    /* Unpublished fixed staging avoids large stack frames/compiler stack
       probe dependencies, and preserves aliased firmware until installation. */
    struct battery_candidate battery;
    st = battery_prepare(&battery, m->hsc_load_rom, media, &info);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    /* The ROM and its RAM are the only allocation; the BIOS goes into the
       instance's fixed array. With a BIOS the machine boots through it,
       without one through the fast-boot hand-off. */
    st = muhtari__cart_install(m, media->image, &info);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    memset(&m->battery, 0, sizeof m->battery);
    memset(m->battery.data, 0xFF, sizeof m->battery.data);
    m->battery.bytes = battery.bytes;
    m->battery.hsc_attached = battery.hsc_attached;
    m->battery.eeprom_port = battery.eeprom_port;
    m->battery.eeprom_device = battery.eeprom_device;
    m->battery.firmware_flags = battery.firmware_flags;
    memcpy(m->battery.hsc_sha1, battery.hsc_sha1, sizeof battery.hsc_sha1);
    if (battery.hsc_attached != 0u) {
        memcpy(m->battery.rom, m->hsc_load_rom, sizeof m->battery.rom);
    }
    for (unsigned p = 0; p < 2u; p++) {
        m->ports.device[p] = media->port[p];
        m->ports.pending_device[p] = media->port[p];
    }
    m->bios_size = 0u;
    if (media->bios != NULL) {
        memcpy(m->bios, media->bios, media->bios_size);
        m->bios_size = (uint32_t)media->bios_size;
    }
    power_cycle(m);
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_power_cycle(muhtari *m)
{
    if (m == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    power_cycle(m);
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_get_battery(const muhtari *m, muhtari_battery_info *out)
{
    if (m == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(out, MUHTARI__BATTERY_SIZE_FIRST,
                                           (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_battery_info info;
    memset(&info, 0, sizeof info);
    info.size = out->size;
    /* The API deliberately exposes host-restorable RAM from a const
       observer; it never mutates, scans or advances the instance. */
    info.data = m->battery.bytes != 0u ? (uint8_t *)(uintptr_t)m->battery.data : NULL;
    info.bytes = m->battery.bytes;
    info.layout_version = 1u;
    info.generation = m->battery.generation;
    info.hsc_attached = m->battery.hsc_attached;
    info.eeprom_port = m->battery.eeprom_port;
    info.eeprom_device = m->battery.eeprom_device;
    info.firmware_flags = m->battery.firmware_flags;
    memcpy(info.hsc_sha1, m->battery.hsc_sha1, sizeof info.hsc_sha1);
    muhtari__write_out(out, &info, (uint32_t)sizeof info);
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_get_hsc_rom(const muhtari *m, muhtari_const_span *out)
{
    if (m == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(out, MUHTARI__CONST_SPAN_SIZE_FIRST,
                                           (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_const_span span;
    memset(&span, 0, sizeof span);
    span.size = out->size;
    if (m->battery.hsc_attached != 0u) {
        span.data = m->battery.rom;
        span.bytes = sizeof m->battery.rom;
    }
    muhtari__write_out(out, &span, (uint32_t)sizeof span);
    return MUHTARI_STATUS_OK;
}
