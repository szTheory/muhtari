; SPDX-License-Identifier: MIT
; The 16K linear board diagnostic: the byte at $C000, where a 16 KiB ROM
; ending at $FFFF starts, and three reads of $4000-$BFFF, where nothing
; drives the bus. See docs/hardware/cartridge.md, The board ROMs, which
; derives every linear16 row of tests/diag/expected.csv. Test n (from 1) is
; the n-th linear16 row. Linear 16K at $C000.

        processor 6502
        ORG $C000
        .byte $16               ; the ROM's first byte
        include "lib.s"

ROM_ID  = 4

; READ address, id: one absolute load, recorded as test id.
        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM

reset SUBROUTINE
        sei
        cld
        lda #$07                ; INPTCTRL: lock, MARIA on, BIOS off
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        ldx #$FF
        txs
        lda #ROM_ID
        jsr mb_init
        READ $C000, 1           ; c000
        READ $4000, 2           ; at_4000
        READ $8000, 3           ; at_8000
        READ $BFFF, 4           ; at_bfff
        jsr mb_finish
        jmp show

        LIB_TAIL
