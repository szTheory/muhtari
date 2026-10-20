/* muhtari_run_frame: every check first, then the frame. With no content
   loaded the frame is the test card and silence; with content, the CPU runs
   from one VBLANK start to the next (docs/hardware/bus.md, The run loop). */
#include <string.h>

#include "internal.h"

/* Runs the CPU until the tick reaches the frame's deadline. Every step makes
   at least one bus cycle of 4 ticks or more, a jammed CPU's read of $FFFF
   included, so the loop ends for any program. It stops at the first
   instruction boundary at or after the deadline; the deadlines are absolute
   ticks, so the overshoot carries into the next frame and frames never
   drift. */
static void run_to_deadline(muhtari *m)
{
    while (m->bus.ticks < m->frame_deadline) {
        muhtari__cpu_step(m);
    }
}

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
       table). A frame with content spans the two VBLANK-start deadlines, so
       the hand-off's first frame of 8,190 ticks gives 54. The sum stays
       below 2^32. */
    uint32_t elapsed = MUHTARI__TICKS_PER_FRAME_NTSC;
    if (m->loaded) {
        elapsed = (uint32_t)(m->frame_deadline - m->frame_start);
    }
    uint32_t acc = m->audio_rem + elapsed * MUHTARI__SAMPLES_PER_PERIOD;
    uint32_t n = acc / MUHTARI__TICKS_PER_PERIOD;
    uint32_t rem = acc % MUHTARI__TICKS_PER_PERIOD;
    if (io->audio != NULL && io->audio_capacity < n) {
        io->audio_count = 0u;
        return MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL;
    }

    if (m->loaded) {
        run_to_deadline(m);
        m->frame_start = m->frame_deadline;
        m->frame_deadline += MUHTARI__TICKS_PER_FRAME_NTSC;
    }
    if (io->video != NULL) {
        for (uint32_t y = 0; y < MUHTARI__HEIGHT_NTSC; y++) {
            uint8_t *row = io->video + (size_t)y * io->video_pitch;
            if (m->loaded) {
                /* MARIA renders no row yet, and every row it has not
                   rendered is written $00. */
                memset(row, 0, MUHTARI__WIDTH);
                continue;
            }
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
