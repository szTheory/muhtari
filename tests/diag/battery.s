; SPDX-License-Identifier: MIT
; Owned persistent-counter diagnostic. HSC windows and RIOT open-drain pins:
; docs/hardware/cartridge.md and controllers.md. The serial protocol follows
; Microchip DS20001203Y sections 4-8; no firmware routine is called.
        processor 6502
        ORG $C000
        include "lib.s"

ORA     = $0280
DDRA    = $0281
WORD_LO = $90
WORD_HI = $91
SERIAL  = $92
ACK     = $93
HOST    = $94
BUSY    = $95
POLL    = $96
ROM_ID  = 13

        MAC OBSERVE
        lda {1}
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
        OBSERVE $3900, 1
        OBSERVE $3904, 2
        lda $1100
        sta WORD_LO
        lda $1101
        sta WORD_HI
        jsr increment
        lda WORD_LO
        sta $1100
        lda WORD_HI
        sta $1101
        ldx $1101
        lda $1100
        ldy #3
        jsr mb_record
        lda #0
        sta ORA
        sta DDRA
        jsr read_word
        jsr increment
        jsr address
        lda WORD_LO
        jsr send
        lda WORD_HI
        jsr send
        jsr stop
        lda #0
        sta BUSY
        lda #$FF
        sta POLL
.poll   jsr start
        lda #$A0
        jsr send
        lda ACK
        beq .ready
        jsr stop
        dec POLL
        bne .poll
        jmp .reported
.ready  jsr stop
        lda #1
        sta BUSY
.reported
        jsr read_word
        lda WORD_LO
        ldx WORD_HI
        ldy #4
        jsr mb_record
        OBSERVE BUSY, 5
        OBSERVE $3010, 6
        jsr mb_finish
        jmp show

increment SUBROUTINE
        lda WORD_LO
        and WORD_HI
        cmp #$FF
        bne .add
        lda #0
        sta WORD_LO
        sta WORD_HI
.add    inc WORD_LO
        bne .done
        inc WORD_HI
.done   rts

; Only host DDR changes generate clock/data edges. ORA remains zero;
; a released pin pulls high and a DDR output drives low.
start SUBROUTINE
        lda #0
        sta DDRA
        lda #4
        sta DDRA
        lda #12
        sta DDRA
        rts
stop SUBROUTINE
        lda #12
        sta DDRA
        lda #4
        sta DDRA
        lda #0
        sta DDRA
        rts
send SUBROUTINE
        sta SERIAL
        ldx #8
.bit    asl SERIAL
        lda #12
        bcc .low
        lda #8
.low    sta DDRA
        and #4
        sta DDRA
        ora #8
        sta DDRA
        dex
        bne .bit
        lda #8
        sta DDRA
        lda #0
        sta DDRA
        lda ORA
        and #4
        sta ACK
        lda #8
        sta DDRA
        rts
address SUBROUTINE
        jsr start
        lda #$A0
        jsr send
        lda #$60
        jsr send
        lda #0
        jmp send
read_word SUBROUTINE
        jsr address
        jsr start
        lda #$A1
        jsr send
        jsr receive
        sta WORD_LO
        lda #12
        sta DDRA
        lda #4
        sta DDRA
        lda #12
        sta DDRA
        jsr receive
        sta WORD_HI
        lda #8
        sta DDRA
        lda #0
        sta DDRA
        lda #8
        sta DDRA
        jmp stop
receive SUBROUTINE
        lda #0
        sta SERIAL
        ldx #8
.bit    lda #8
        sta DDRA
        lda #0
        sta DDRA
        lda ORA
        and #4
        cmp #4
        rol SERIAL
        lda #8
        sta DDRA
        dex
        bne .bit
        lda SERIAL
        rts

        LIB_TAIL
