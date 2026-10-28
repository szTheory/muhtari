; SPDX-License-Identifier: MIT
; Owned BankSet stimulus. Window/redirect facts: docs/hardware/cartridge.md,
; BankSet. Every record comes from CPU reads, with a runtime mailbox CRC.
; The driver relocates these two sets and preserves the upper CPU code.
; $F6F0 identifies the template; $F700..3 select low start/end high bytes,
; bank count and RAM mode (0 none, 1 shared, 2 HALT). They are stimuli,
; never numeric answers. The canonical 32 KiB/set image has no RAM.
        processor 6502
        ORG 0
        RORG $8000
        ds $3FFF, $40
        .byte $80
        .byte $D0
        include "lib.s"
ROM_ID = 20
PTR = $85
SUM_LO = $87
SUM_HI = $88
BANK = $89
        MAC READ
        lda {1}
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM
        MAC INDIRECT
        ldy #{1}
        lda (PTR),y
        ldx #0
        ldy #{2}
        jsr mb_record
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
        lda #ROM_ID
        jsr mb_init
        lda #0
        sta PTR
        lda $F700
        sta PTR+1
        INDIRECT 0, 1
        lda $F701
        sta PTR+1
        INDIRECT $FF, 2
        READ $C000, 3
        READ $8000, 4
        lda #0
        sta BANK
        sta SUM_LO
        sta SUM_HI
.banks  lda BANK
        sta $8000
        clc
        lda SUM_LO
        adc $8000
        sta SUM_LO
        bcc .next
        inc SUM_HI
.next   inc BANK
        bne .banks
        lda SUM_LO
        ldx SUM_HI
        ldy #5
        jsr mb_record
        lda #0
        sta SUM_LO
        sta SUM_HI
.ends   lda BANK
        sta $BFFF
        clc
        lda SUM_LO
        adc $BFFF
        sta SUM_LO
        bcc .nextend
        inc SUM_HI
.nextend
        inc BANK
        bne .ends
        lda SUM_LO
        ldx SUM_HI
        ldy #6
        jsr mb_record
        READ $8000, 7
        READ $BFFF, 8
        lda #0
        sta $8000
        lda #$40
        sta $4000
        lda #$80
        sta $7FFF
        lda #$C0
        sta $C000
        lda #$C8
        sta $FFFF
        lda $F700
        sta PTR+1
        INDIRECT 0, 9
        lda $F701
        sta PTR+1
        INDIRECT $FF, 10
        READ $C000, 11
        lda $F700
        sta PTR+1
        INDIRECT 1, 12
        INDIRECT $FF, 13
        READ $8000, 14
        READ $BFFF, 15
        READ $BFFE, 16
        lda $F701
        sta PTR+1
        INDIRECT $FE, 17
        jsr mb_finish
        ; One byte from each endpoint, separate 160A objects. ROM graphics
        ; differ between sets; RAM graphics distinguish shared from HALT.
        ; Copy the MARIA display list to ordinary RAM, then choose addresses.
        lda #0
        sta $1800
        lda #31
        sta $1801
        lda #$E0
        sta $1802
        lda #0
        sta $1803
        lda #1
        sta $1804
        lda #31
        sta $1805
        lda #$E0
        sta $1806
        lda #8
        sta $1807
        lda #0
        sta $1808
        sta $1809
        lda $F703
        beq .display
        lda #$40
        sta $1802
        lda #$FF
        sta $1804
        lda #$7F
        sta $1806
.display
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
        .byte $40, $80
        ds $F6F0-*, 0
        .byte "BANKSET1"
        ds $F700-*, 0
        .byte $80, $BF, 0, 0
        LIB_TAIL
        ORG $8000
        RORG $8000
        ds $3FFF, $A0
        .byte $B0
        .byte $E0
        ds $D000-*, 0
        REPEAT 32
        .byte 15, $18, 0
        REPEND
        ds $E000-*, 0
        .byte $C0, $C8
        ds $FFF8-*, 0
        .byte $FF, $C7
        .word lib_nmi, reset, lib_nmi
