; SPDX-License-Identifier: MIT
; Owned XG-1 numeric diagnostic, identity15, scenario0.
; Timing/model and host frame schedule: docs/hardware/controllers.md.
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
        MAC CLEAR
        lda #0
        sta $01
        lda #$40
        sta $01
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
        lda #15
        jsr mb_init
        lda SCENARIO
        RECORD 1
        lda SCENARIO
        beq .light
        jmp .finish
.light
        lda #7
        sta BACKGRND
        CLEAR
        jsr next_frame          ; frame3, native luma7 never qualifies
        SENSOR $0C, 2
        SENSOR $0D, 3
        lda $0280
        RECORD 4
        ; The bright-background step is added by the implementation.
        jsr next_frame          ; frame4, remember frame3 light
        SENSOR $0C, 5
        SENSOR $0D, 6
        lda $0280
        RECORD 7
        CLEAR
        jsr next_frame          ; frame5, right aim remains offscreen
        SENSOR $0C, 8
        SENSOR $0D, 9
        lda #$40
        sta $01                 ; repeated enable preserves low memory
        SENSOR $0C, 10
        SENSOR $0D, 11
        jsr next_frame          ; frame6, both endpoint aims now active
        CLEAR
        jsr next_frame          ; frame7, final pixel response still due
        SENSOR $0C, 12
        SENSOR $0D, 13
        jsr next_frame          ; frame8, historical right pulse captured
        SENSOR $0D, 14
        lda $0280
        RECORD 15
.finish
        jsr mb_finish
        jmp show

next_frame SUBROUTINE
        jsr wait_visible
        jmp wait_vblank
        LIB_TAIL
