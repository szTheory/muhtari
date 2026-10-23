; SPDX-License-Identifier: MIT
; Owned Activision OM-order diagnostic. Hardware map: bankswitching guide
; rev391 and docs/hardware/cartridge.md, Activision. Each 8 KiB half's tag
; is its OM image half number. The switched A000/C000 pair reverses each
; 16 KiB bank's halves, so bank n reads 2*n+1 and 2*n. Fixed windows read
; tags 12,13,14,15. Expectations are specified before execution in
; expected.csv; record n is the n-th activision row.
; FF88-FF8F decoding and bank-zero startup are selected compatibility /
; deterministic profile policies, not cartridge measurements. All code,
; helpers, font and vectors execute in fixed E000-FFFF, never C000-DFFF.

        processor 6502
HALF    SET 0
        REPEAT 15
        ORG HALF * $2000
        RORG $4000
        ds $2000, HALF
HALF    SET HALF + 1
        REPEND

        ORG $1E000
        RORG $E000
        .byte 15
        include "lib.s"

ROM_ID  = 10

        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM

; Selector address, first record: read both independently tagged halves.
        MAC SELECT
        lda #$A5               ; selection ignores written data
        sta {1}
        READ $A000, {2}
        READ $C000, {2}+1
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
        READ $A000, 1          ; bank-zero upper half = 1
        READ $C000, 2          ; bank-zero lower half = 0
        READ $4000, 3          ; fixed OM offsets 18000,1A000,1C000,1E000
        READ $6000, 4
        READ $8000, 5
        READ $E000, 6
        SELECT $FF80, 7
        SELECT $FF81, 9
        SELECT $FF82, 11
        SELECT $FF83, 13
        SELECT $FF84, 15
        SELECT $FF85, 17
        SELECT $FF86, 19
        SELECT $FF87, 21
        lda #0
        sta $FF83
        READ $BFFF, 23         ; bank 3 upper endpoint = 7
        READ $DFFF, 24         ; bank 3 lower endpoint = 6
        sta $FF8D              ; extended hotspot selects bank 5, data ignored
        READ $C000, 25
        sta $FF7F              ; neighbors cannot change bank 5
        READ $C000, 26
        sta $FF90
        READ $C000, 27
        jsr mb_finish
        jmp show

        LIB_TAIL
