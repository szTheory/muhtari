; SPDX-License-Identifier: MIT
; The SuperGame bank-6 board diagnostic: 8 banks of 16 KiB, each starting
; with its own number. It reads the bank at $8000 at power-on, the fixed
; banks at $4000 and $C000, and the bank $8000 shows after each write of a
; value to $8000. See docs/hardware/cartridge.md, The board ROMs, which
; derives every bank6 row of tests/diag/expected.csv. Test n (from 1) is the
; n-th bank6 row. The code runs from the last bank, fixed at $C000.

        processor 6502
BANKS   = 8

; Banks 0 to BANKS - 2: their number, then zeros.
BANK    SET 0
        REPEAT BANKS - 1
        ORG BANK * $4000
        RORG $8000
        .byte BANK
        ds $3FFF, 0
BANK    SET BANK + 1
        REPEND

        ORG (BANKS - 1) * $4000
        RORG $C000
        .byte BANKS - 1         ; the last bank's number
        include "lib.s"

ROM_ID  = 5

; READ address, id: one absolute load, recorded as test id.
        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM

; SELECT value, id: the value written to $8000, then $8000 read back.
        MAC SELECT
        lda #{1}
        sta $8000
        READ $8000, {2}
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
        READ $8000, 1           ; power_on
        READ $4000, 2           ; at_4000
        READ $C000, 3           ; at_c000
        SELECT 0, 4             ; sel_0
        SELECT 1, 5
        SELECT 2, 6
        SELECT 3, 7
        SELECT 4, 8
        SELECT 5, 9
        SELECT 6, 10
        SELECT 7, 11            ; sel_7
        SELECT 11, 12           ; sel_11
        jsr mb_finish
        jmp show

        LIB_TAIL
