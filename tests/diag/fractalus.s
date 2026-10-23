; SPDX-License-Identifier: MIT
; Owned physical Fractalus RAM diagnostic. Bankswitching guide rev391 and
; docs/hardware/cartridge.md, Fractalus: adjacent page pairs 40/41,42/43,
; ...,4E/4F share eight physical 256-byte blocks. Pairs repeat at 5000,
; 6000 and 7000. Upper mirrors and zero-filled volatile startup are selected
; compatibility/profile policies, not cartridge measurements. Each write
; below names an independent physical location; a second pattern detects
; stale or distinct aliases. Expected.csv specifies record n before execution.

        processor 6502
        ORG $8000
        ds $4000, $80
        .byte $C0
        include "lib.s"

ROM_ID  = 12

        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
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
        READ $4007, 1
        READ $4FFF, 2
        READ $7F55, 3
        lda #$5A
        sta $4107
        READ $4007, 4
        READ $5007, 5
        READ $7107, 6
        lda #$A6
        sta $4207
        READ $4307, 7
        READ $6307, 8
        READ $4007, 9          ; block zero stays distinct from block one
        lda #$3C
        sta $7F55
        READ $4E55, 10
        READ $4F55, 11
        READ $6E55, 12
        READ $4E54, 13         ; low address bits remain connected
        lda #$81
        sta $40FF
        READ $41FF, 14
        READ $70FF, 15
        READ $4207, 16
        lda #$C3
        sta $4FFF
        READ $4EFF, 17
        READ $7FFF, 18
        READ $40FF, 19         ; block seven differs from block zero
        READ $8000, 20
        READ $BFFF, 21
        READ $C000, 22
        lda #$69
        sta $4107
        READ $6007, 23         ; second pattern propagates through aliases
        READ $4207, 24
        jsr mb_finish
        jmp show

        LIB_TAIL
