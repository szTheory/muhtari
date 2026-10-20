/* Instance lifetime, the version and info queries, and the size-tag helper. */
#include <string.h>

#include "internal.h"
#include "muhtari_version.h"

/* The interface numbers this build reports. Each starts at 1, so zero means
   absent; the behaviour revision changes when a frame, audio or state hash
   changes for the same inputs. */
#define MUHTARI__ABI 1u
#define MUHTARI__BEHAVIOUR_REVISION 1u

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
    /* The beam starts at line 0, t 0 of the current origin; the first cycle
       recomputes the deadline from MARIA's next line event. */
    muhtari__maria_start(m);
    m->bus.next_deadline = 0u;
    /* Only the zero fill exists so far; every RAM_FILL value gives it. */
    memset(m->ram, 0, sizeof m->ram);
    muhtari__riot_reset(m);
}

void muhtari_destroy(muhtari *m)
{
    if (m == NULL) {
        return;
    }
    muhtari_allocator a = m->allocator;
    if (m->cart.rom != NULL) {
        a.free(a.user, m->cart.rom, m->cart.info.rom_size);
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
