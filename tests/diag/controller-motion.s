; SPDX-License-Identifier: MIT
; Owned motion diagnostic; connector and cadence derivation in
; docs/hardware/controllers.md, Motion diagnostic. ROM identity 14.
        processor 6502
        ORG $C000
        include "lib.s"

reset SUBROUTINE
        sei
        cld
        ldx #$FF
        txs
        lda #7
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        lda #14
        jsr mb_init
        jsr wait_visible
        jsr wait_vblank
        lda $0282
        ldx #0
        ldy #1
        jsr mb_record
        lda $0280
        ldx #0
        ldy #2
        jsr mb_record
        jsr mb_finish
        jmp show

        LIB_TAIL
