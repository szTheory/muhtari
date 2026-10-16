/* muhtari_run_frame: every check first, then the frame. With no content
   loaded the frame is the test card and silence. */
#include <string.h>

#include "internal.h"

muhtari_status muhtari_run_frame(muhtari *m, muhtari_frame *io)
{
    if (m == NULL || io == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(io, MUHTARI__FRAME_SIZE_FIRST, (uint32_t)sizeof *io);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    /* From here on audio_count is written on every return; an error reports
       zero samples and changes no state. */
    if (io->video != NULL &&
        (io->video_pitch < MUHTARI__PITCH_MIN || io->video_pitch > MUHTARI__PITCH_MAX)) {
        io->audio_count = 0u;
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    /* Samples this frame: 88 per 13,125 ticks with the remainder carried, so
       800 or 801 per 119,402-tick frame and no drift (A7800-AUDIO, timing
       table). The sum stays below 2^32. */
    uint32_t acc = m->audio_rem + MUHTARI__TICKS_PER_FRAME_NTSC * MUHTARI__SAMPLES_PER_PERIOD;
    uint32_t n = acc / MUHTARI__TICKS_PER_PERIOD;
    uint32_t rem = acc % MUHTARI__TICKS_PER_PERIOD;
    if (io->audio != NULL && io->audio_capacity < n) {
        io->audio_count = 0u;
        return MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL;
    }

    if (io->video != NULL) {
        for (uint32_t y = 0; y < MUHTARI__HEIGHT_NTSC; y++) {
            uint8_t *row = io->video + (size_t)y * io->video_pitch;
            for (uint32_t x = 0; x < MUHTARI__WIDTH; x++) {
                row[x] = muhtari__testcard_pixel(x, y, MUHTARI__HEIGHT_NTSC);
            }
        }
    }
    if (io->audio != NULL) {
        memset(io->audio, 0, (size_t)n * sizeof io->audio[0]);
    }
    m->audio_rem = rem;
    io->audio_count = n;
    return MUHTARI_STATUS_OK;
}
