; SPDX-License-Identifier: MIT
; Owned analog numeric diagnostic, identity16, scenario0 paddles.
; Independent RC vectors and host schedule: docs/hardware/controllers.md.
        processor 6502
        ORG $C000
        include "lib.s"
SCENARIO = $90

        MAC RECORD
        ldx #0
        ldy #{1}
        jsr mb_record
        ENDM
        MAC SENSOR
        lda {1}
        and #$80
        RECORD {2}
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
        lda $0282
        eor #$FF
        and #$0B
        sta SCENARIO
        lda #16
        jsr mb_init
        lda SCENARIO
        RECORD 1
        lda SCENARIO
        beq .paddles
        jmp .finish
.paddles
        jsr next_frame          ; frame3
        lda #$80
        sta $01
        jsr all_pots
        RECORD 2
        lda $0280
        RECORD 3
        lda #0
        sta $01
        ; The first executed test requires an elapsed finite charge wait.
        SENSOR $08, 4
        SENSOR $09, 5
        SENSOR $0A, 6
        SENSOR $0B, 7
.finish
        jsr mb_finish
        jmp show

next_frame SUBROUTINE
        jsr wait_visible
        jmp wait_vblank

; Four D7 values as bits0..3, never the undriven open-bus bits.
all_pots SUBROUTINE
        lda #0
        sta $91
        ldx #3
.pot    lda $08,x
        asl
        rol $91
        dex
        bpl .pot
        lda $91
        rts
        LIB_TAIL
