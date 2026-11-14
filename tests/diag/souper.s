; SPDX-License-Identifier: MIT
; Owned Souper mapper stimulus. The committed template is a full-size image;
; the driver replaces its banks with patterns while preserving the final code.
        processor 6502
        ORG 0
        REPEAT 31
        ds $4000, $40
        REPEND
        ORG $7C000
        RORG $C000
        .byte $E0
        include "lib.s"
ROM_ID = 22
reset SUBROUTINE
        sei
        cld
        ldx #$FF
        txs
        lda #7
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        lda #ROM_ID
        jsr mb_init
        lda $4000
        ldx #0
        ldy #1
        jsr mb_record
        lda $7FFF
        ldx #0
        ldy #2
        jsr mb_record
        lda $8000
        ldx #0
        ldy #3
        jsr mb_record
        lda $BFFF
        ldx #0
        ldy #4
        jsr mb_record
        lda $C000
        ldx #0
        ldy #5
        jsr mb_record
        jsr mb_finish
        ; The mailbox samples the power-on zeros first. Then graphics at $4000
        ; exercise the same EXRAM through MARIA's real DMA fetches.
        lda #$40
        ; One width-1 graphics byte per legal 160A vertical page makes the
        ; literal row oracle independent of which row offset MARIA fetches.
        lda #$FF
        sta $4000
        sta $4100
        sta $4200
        sta $4300
        sta $4400
        sta $4500
        sta $4600
        sta $4700
        sta $4800
        sta $4900
        sta $4A00
        sta $4B00
        sta $4C00
        sta $4D00
        sta $4E00
        sta $4F00
        ; One object reads graphics from the initial flat EXRAM window.
        lda #0
        sta $1800
        lda #$1F
        sta $1801
        lda #$40
        sta $1802
        lda #0
        sta $1803
        lda #0
        sta $1804
        sta $1805
        lda #0
        sta $1806
        lda #0
        sta $1807
        lda #8
        sta $1808
        lda #0
        sta $1809
        lda #$D0
        sta DPPH
        lda #0
        sta DPPL
        lda #$16
        sta P0C1
        lda #$27
        sta P0C2
        lda #$38
        sta P0C3
        lda #0
        sta BACKGRND
        lda #CTRL_ON
        sta CTRL
.forever
        jmp .forever
        ds $D000-*, 0
        REPEAT 32
        .byte 15, $18, 0
        REPEND
        ds $E000-*, 0
        ; The driver replaces the first 32 KiB of EXRAM with a known pattern.
        .byte $55, $AA
        ds $F6F0-*, 0
        .byte "SOUPER01"
        LIB_TAIL
