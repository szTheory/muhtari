; SPDX-License-Identifier: MIT
; Owned analog numeric diagnostic, identity16, scenario0 paddles /1 keypad.
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

; Each fresh recovery starts with both row1 pins low then released. The
; release STA finishes one tick after data. LDX+140 DEX/BNE iterations
; cost2804 ticks. Three BIT zero-page TIA reads add42. LDA absolute data
; arrives17 ticks later: 1+2804+42+17=2864. BIT RAM saves2 for early;
; BIT absolute TIA adds4 for late. Alignment prevents branch page carry.
        MAC RECOVERY
        ALIGN 256
        SUBROUTINE
        lda #0
        sta $0280
        lda #$11
        sta $0281
        lda #0
        sta $0281
        ldx #140
.delay  dex
        bne .delay
        IF {3} = 0
        bit $80
        ELSE
        IF {3} = 2
        bit $0100
        ELSE
        bit $00
        ENDIF
        ENDIF
        bit $00
        bit $00
        lda.w {1}
        and #$80
        RECORD {2}
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
        cmp #1
        bne .unknown
        jmp keypad
.unknown
        jmp finish
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
        ldx #5
        jsr wait_lines          ; >1878, well below95750
        SENSOR $08, 4
        SENSOR $09, 5
        SENSOR $0A, 6
        SENSOR $0B, 7
        ldx #210
        jsr wait_lines          ; both midpoint comparators crossed
        SENSOR $08, 8
        SENSOR $09, 9
        SENSOR $0A, 10
        SENSOR $0B, 11
        ldx #210
        jsr wait_lines          ; >189625; full physical maximum
        jsr all_pots
        RECORD 12
        jsr next_frame          ; frame5, opposite independent buttons
        lda $0280
        RECORD 13
        jsr next_frame          ; frame6, swapped endpoint positions
        lda #$80
        sta $01
        lda #0
        sta $01
        ldx #5
        jsr wait_lines
        SENSOR $08, 14
        SENSOR $09, 15
        SENSOR $0A, 16
        SENSOR $0B, 17
        ldx #210
        jsr wait_lines
        ldx #210
        jsr wait_lines
        jsr all_pots
        RECORD 18
        jsr next_frame          ; frame8, all buttons released
        lda $0280
        RECORD 19
finish SUBROUTINE
        jsr mb_finish
        jmp show

keypad SUBROUTINE
        jsr next_frame          ; frame3: keys1,2,3,4 on both ports
        ldx #8
        jsr wait_lines
        jsr all_pots
        RECORD 2
        jsr triggers
        RECORD 3
        lda #0
        sta $0280
        lda #$11
        sta $0281
        jsr all_pots
        RECORD 4
        jsr triggers
        RECORD 5
        RECOVERY $08, 6, 0
        RECOVERY $08, 7, 1
        RECOVERY $08, 8, 2
        RECOVERY $0A, 9, 0
        RECOVERY $0A, 10, 1
        RECOVERY $0A, 11, 2
        lda #$11
        sta $0281
        lda #$44              ; scan a different row before charge recovers
        sta $0281
        jsr all_pots
        RECORD 12
        ldx #8
        jsr wait_lines
        jsr all_pots
        RECORD 13
        lda #$22              ; ghost through keys4,1,2, with row1 released
        sta $0281
        jsr all_pots
        RECORD 14
        lda #$FF
        sta $0280
        sta $0281
        ldx #8
        jsr wait_lines
        jsr all_pots
        RECORD 15
        jsr triggers
        RECORD 16
        lda #0
        sta $0280
        jsr all_pots
        RECORD 17
        jsr triggers
        RECORD 18
        lda #$11
        sta $0280
        lda #$33
        sta $0281
        jsr all_pots
        RECORD 19
        lda $0280
        RECORD 20
        lda #0
        sta $0281
        jsr next_frame        ; frame4: no keys
        lda #$80
        sta $01
        jsr all_pots
        RECORD 21
        jsr triggers
        RECORD 22
        jsr next_frame        ; frame5: all twelve keys; dump still held
        lda #$C0
        sta $01
        jsr triggers
        RECORD 23
        lda #$40
        sta $01
        jsr triggers
        RECORD 24
        lda #0
        sta $01
        jsr triggers
        RECORD 25
        ldx #8
        jsr wait_lines
        jsr all_pots
        RECORD 26
        jsr next_frame        ; frame6: keys3,6, the trigger column
        lda #0
        sta $0280
        lda #$22
        sta $0281
        jsr triggers
        RECORD 27
        jmp finish

triggers SUBROUTINE
        lda $0D
        asl
        lda #0
        rol
        sta $92
        lda $0C
        asl
        rol $92
        lda $92
        rts

next_frame SUBROUTINE
        jsr wait_visible
        jmp wait_vblank

wait_lines SUBROUTINE
.line   sta WSYNC
        dex
        bne .line
        rts

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
