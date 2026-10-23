; SPDX-License-Identifier: MIT
; Owned Absolute diagnostic. Hardware windows: bankswitching guide rev391
; and docs/hardware/cartridge.md, Absolute. Bank tags are 20/40 hex; fixed
; 8000/A000/C000 tags are 80/A0/C0 hex. All code and shared helpers are in
; fixed upper ROM. Record n is the n-th absolute row in expected.csv.
; Exact 8000 decoding, D0 precedence for unusual values and bank-zero
; startup are selected compatibility/profile policies, not measurements.

        processor 6502
        ORG $0000
        ds $4000, $20
        ds $4000, $40
        ds $2000, $80
        ds $2000, $A0
        .byte $C0
        include "lib.s"

ROM_ID  = 11

        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM

        MAC SELECT
        lda #{1}
        sta $8000
        READ $4000, {2}
        ENDM

reset SUBROUTINE
        sei
        cld
        lda #$07
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        ldx #$FF
        txs
        lda #ROM_ID
        jsr mb_init
        READ $4000, 1
        READ $7FFF, 2
        READ $8000, 3
        READ $A000, 4
        READ $C000, 5
        SELECT 2, 6
        READ $7FFF, 7
        SELECT 0, 8             ; retain selected bank one
        SELECT $FC, 9           ; high bits alone retain
        SELECT $FE, 10          ; high bits do not change D1 selection
        SELECT $FD, 11          ; high bits do not change D0 selection
        SELECT 2, 12
        SELECT 3, 13            ; D0 takes precedence over D1
        SELECT $FF, 14
        SELECT 2, 15
        lda #1
        sta $8001
        READ $4000, 16          ; above selector retains bank one
        lda #1
        sta $7FFF
        READ $4000, 17          ; below selector retains bank one
        lda #1
        sta $FFFF
        READ $4000, 18          ; A15 alone is not a selector
        SELECT 1, 19
        SELECT 0, 20            ; retain selected bank zero
        READ $BFFF, 21          ; fixed ROM remains unchanged by writes
        jsr mb_finish
        jmp show

        LIB_TAIL
