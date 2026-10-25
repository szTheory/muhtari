/* muhtari_run_frame: every check first, then the frame. With no content
   loaded the frame is the test card and silence; with content, the CPU runs
   from one VBLANK start to the next (docs/hardware/bus.md, The run loop). */
#include <assert.h>
#include <string.h>

#include "internal.h"

/* Runs the CPU until the tick reaches the frame's deadline. Every step makes
   at least one bus cycle of 4 ticks or more, a jammed CPU's read of $FFFF
   included, so the loop ends for any program. It stops at the first
   instruction boundary at or after the deadline; the deadlines are absolute
   ticks, so the overshoot carries into the next frame and frames never
   drift. It stops early when the BIOS rejects the cartridge. */
static void run_to_deadline(muhtari *m)
{
    while (m->bus.ticks < m->frame_deadline && m->boot.rejected == 0u) {
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
       zero samples. Validation executes nothing; BIOS refusal can instead
       follow partial execution and stands until load or power cycle. */
    if (m->boot.rejected != 0u) {
        io->audio_count = 0u;
        return MUHTARI_STATUS_ERR_BIOS_REJECTED_CART;
    }
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

    /* The pending input applies at a VBLANK start: the start of every
       frame but the first after a load. That first frame starts at the
       fast boot's hand-off, which latches nothing, or at the BIOS boot's
       power-on, whose inputs are what is held as the power comes on, so
       they apply from it (docs/runner.md, Movies). An initial extended
       packet also applies before fast boot's first opcode (embedding.md). */
    if ((m->frames == 0u && m->ports.input_mode != 0u && m->ports.initialized == 0u) ||
        !m->loaded || elapsed == MUHTARI__TICKS_PER_FRAME_NTSC ||
        elapsed == MUHTARI__VBLANK_START) {
        muhtari__ports_latch(m);
    }
    if (m->loaded) {
        /* This logical horizon is fixed before execution. The CPU still
           completes its instruction and possible interrupt entry; their
           later audio stays in the bounded synthesis FIFO (audio.md). */
        const uint64_t audio_end = m->frame_deadline;
        /* MARIA writes each pixel into the caller's row as the beam plays
           it (docs/hardware/maria.md, Rendering). A full frame scans every
           row; the first frame after a load starts mid-frame, and the rows
           the beam has not scanned since the load are written $00. */
        if (io->video != NULL &&
            m->frame_deadline - m->frame_start != MUHTARI__TICKS_PER_FRAME_NTSC) {
            for (uint32_t y = 0; y < MUHTARI__HEIGHT_NTSC; y++) {
                memset(io->video + (size_t)y * io->video_pitch, 0, MUHTARI__WIDTH);
            }
        }
        m->maria.rows = io->video;
        m->maria.pitch = io->video_pitch;
        muhtari__observe_begin(m, audio_end);
        run_to_deadline(m);
        /* Programming follows actual execution, including completed
           instruction overshoot and BIOS refusal (controllers.md). */
        muhtari__savekey_catch_up(m, m->bus.ticks);
        m->maria.rows = NULL;
        m->maria.pitch = 0u;
        muhtari__observe_end(m);
        if (m->boot.rejected != 0u) {
            /* The frame stops where the BIOS took its 2600 path. */
            io->audio_count = 0u;
            return MUHTARI_STATUS_ERR_BIOS_REJECTED_CART;
        }
        muhtari__tia_catch_up(m, m->bus.ticks);
        muhtari__observe_end(m);
        uint32_t rendered = muhtari__synth_render(m, audio_end, m->audio_work);
        assert(rendered == n);
        (void)rendered;
        m->frame_start = m->frame_deadline;
        m->frame_deadline += MUHTARI__TICKS_PER_FRAME_NTSC;
    } else if (io->video != NULL) {
        for (uint32_t y = 0; y < MUHTARI__HEIGHT_NTSC; y++) {
            uint8_t *row = io->video + (size_t)y * io->video_pitch;
            for (uint32_t x = 0; x < MUHTARI__WIDTH; x++) {
                row[x] = muhtari__testcard_pixel(x, y, MUHTARI__HEIGHT_NTSC);
            }
        }
    }
    if (io->audio != NULL) {
        if (m->loaded) {
            memcpy(io->audio, m->audio_work, (size_t)n * sizeof io->audio[0]);
        } else {
            memset(io->audio, 0, (size_t)n * sizeof io->audio[0]);
        }
    }
    /* A lag frame is one in which the CPU read no input register. */
    m->frames++;
    if (m->ports.input_read == 0u) {
        m->lag_frames++;
    }
    m->ports.input_read = 0u;
    muhtari__ports_count_hazard(m);
    m->audio_rem = rem;
    io->audio_count = n;
    return MUHTARI_STATUS_OK;
}
