; SPDX-License-Identifier: MIT
; Owned motion diagnostic; connector and cadence derivation in
; docs/hardware/controllers.md, Motion diagnostic. ROM identity 14.
; Each scenario is a fresh load with fixed devices. Scenario 0 uses driving
; on both ports; later CX22/ST/Amiga/SNES/Mega slices share this fixture.
; Host frames 1/2 neutral, selector held (Reset/Select/Pause weights 1/2/4);
; from frame 3 release selector. Frame 3: dx +4/-4, left fire. Frame 4:
; dx -1/+1, right fire. Frame 5: no submission. Frame 6: zero/released.
        processor 6502
        ORG $C000
        include "lib.s"

SCENARIO = $90
SAMPLES  = $91

        MAC RECORD
        ldx #0
        ldy #{1}
        jsr mb_record
        ENDM

        MAC READ
        lda {1}
        RECORD {2}
        ENDM

        MAC FIRE
        lda {1}
        and #$80
        RECORD {2}
        ENDM

; Each pair samples 429..432 ticks after VBLANK plus whole 454-tick
; lines, before/after the corresponding edge. No mailbox call between
; samples: its variable work cannot move the selected data ticks.
        MAC SAMPLE
        sta WSYNC
        lda $0280
        sta SAMPLES+{1}
        ENDM

reset SUBROUTINE
        sei
        cld
        ldx #$FF
        txs
        lda #7
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        jsr wait_visible
        jsr wait_vblank
        ; First full-frame VBLANK start; selector still held in frame 2.
        lda $0282
        eor #$FF
        and #$0B
        sta SCENARIO
        and #8
        lsr
        sta TMP_LO
        lda SCENARIO
        and #3
        ora TMP_LO
        sta SCENARIO
        ; Clearing the mailbox takes longer than the short hand-off
        ; frame. Decode first so the held frame-2 selector is observed.
        lda #14
        jsr mb_init
        lda SCENARIO
        RECORD 1
        ; Unsupported selectors complete with their true ID and no
        ; invented records; a scenario judge must reject the mismatch.
        lda SCENARIO
        beq .driving
        cmp #4
        bcc .validmouse
        jmp .finish
.validmouse
        jmp mouse
.driving
        READ $0280, 2
        FIRE $0C, 3
        FIRE $0D, 4

        jsr next_frame          ; frame 3, both directions and left fire
        SAMPLE 0
        SAMPLE 1
        SAMPLE 2
        SAMPLE 3
        SAMPLE 4
        READ SAMPLES, 5
        READ SAMPLES+1, 6
        READ SAMPLES+2, 7
        READ SAMPLES+3, 8
        READ SAMPLES+4, 9
        FIRE $0C, 10
        FIRE $0D, 11

        jsr next_frame          ; frame 4, one reversed edge and right fire
        SAMPLE 0
        SAMPLE 1
        READ SAMPLES, 12
        READ SAMPLES+1, 13
        FIRE $0C, 14
        FIRE $0D, 15
        jsr next_frame          ; frame 5, no submission or replay
        READ $0280, 16
        FIRE $0D, 17
        jsr next_frame          ; frame 6, neutral packet retains phase
        READ $0280, 18
        FIRE $0D, 19
.finish
        jsr mb_finish
        jmp show

; Fixed mouse scenarios1..3 share accesses, never a production pin table.
mouse SUBROUTINE
        READ $0280, 2
        FIRE $0C, 3
        FIRE $0D, 4
        lda #0
        sta $0284              ; falling PA7; clear prior history
        lda $0285
        jsr next_frame          ; frame3, opposite X four-edge cycles
        SAMPLE 0
        SAMPLE 1
        SAMPLE 2
        SAMPLE 3
        SAMPLE 4
        READ SAMPLES, 5
        READ SAMPLES+1, 6
        READ SAMPLES+2, 7
        READ SAMPLES+3, 8
        READ SAMPLES+4, 9
        FIRE $0C, 10
        FIRE $0D, 11
        jsr next_frame          ; frame4, reverse X one unit
        SAMPLE 0
        SAMPLE 1
        READ SAMPLES, 12
        READ SAMPLES+1, 13
        FIRE $0C, 14
        FIRE $0D, 15
        jsr next_frame          ; frame5, no new library submission
        READ $0280, 16
        jsr next_frame          ; frame6, opposite Y complete cycles
        SAMPLE 0
        SAMPLE 1
        SAMPLE 2
        SAMPLE 3
        SAMPLE 4
        READ SAMPLES, 17
        READ SAMPLES+1, 18
        READ SAMPLES+2, 19
        READ SAMPLES+3, 20
        READ SAMPLES+4, 21
        lda $09
        and #$80
        sta SAMPLES+5
        lda $0B
        and #$80
        sta SAMPLES+6
        jsr next_frame          ; frame7, reverse Y one unit
        SAMPLE 0
        SAMPLE 1
        READ SAMPLES, 22
        READ SAMPLES+1, 23
        lda $0285
        and #$40
        RECORD 24
        READ SAMPLES+5, 25
        READ SAMPLES+6, 26
        jsr next_frame          ; frame8, released right switches
        lda $09
        and #$80
        asl
        rol TMP_LO
        lda $0B
        and #$80
        asl
        rol TMP_LO
        lda TMP_LO
        and #3
        RECORD 27
        jsr mb_finish
        jmp show

next_frame SUBROUTINE
        jsr wait_visible
        jmp wait_vblank

        LIB_TAIL
