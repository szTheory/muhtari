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

        ; Distinguish both ends of each flat EXRAM half with CPU stores.
        lda #$A0
        sta $4000
        lda #$A1
        sta $5FFF
        lda #$A2
        sta $6000
        sta $6FFF
        lda #$A3
        sta $7000
        sta $7FFF
        lda $4000
        ldx #0
        ldy #6
        jsr mb_record
        lda $5FFF
        ldx #0
        ldy #7
        jsr mb_record
        lda $6000
        ldx #0
        ldy #8
        jsr mb_record
        lda $6FFF
        ldx #0
        ldy #9
        jsr mb_record
        lda $7000
        ldx #0
        ldy #10
        jsr mb_record
        lda $7FFF
        ldx #0
        ldy #11
        jsr mb_record

        ; Enable all independent mode bits, then select distinct video/data
        ; pages through register aliases. Lower EXRAM remains fixed.
        lda #7
        sta $D003
        lda #6
        sta $D004
        lda #7
        sta $D005
        lda #$A6
        sta $6000
        sta $6FFF
        lda #$A7
        sta $7000
        sta $7FFF
        lda $6000
        ldx #0
        ldy #12
        jsr mb_record
        lda $6FFF
        ldx #0
        ldy #13
        jsr mb_record
        lda $7000
        ldx #0
        ldy #14
        jsr mb_record
        lda $7FFF
        ldx #0
        ldy #15
        jsr mb_record
        lda $4000
        ldx #0
        ldy #16
        jsr mb_record
        lda $5FFF
        ldx #0
        ldy #17
        jsr mb_record

        ; Exercise the high bank selector and its low-three-bit alias. The
        ; fixed upper bank remains executable while the lower bank changes.
        lda #30
        sta $FFF8
        lda $8000
        ldx #0
        ldy #18
        jsr mb_record
        lda $BFFF
        ldx #0
        ldy #19
        jsr mb_record
        lda #31
        sta $FFF8
        lda $8000
        ldx #0
        ldy #20
        jsr mb_record
        jsr mb_finish
        lda #0
        sta $D003 ; restore flat EXRAM for the real MARIA EXRAM pixel workload
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
