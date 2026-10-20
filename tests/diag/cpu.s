; SPDX-License-Identifier: MIT
; The slow-access diagnostic. Each test runs 64 absolute loads from one
; address with DMA off, between a write to TIM1T and MARIA's WSYNC, and
; reports INTIM after the WSYNC: see docs/hardware/maria.md, The cpu ROM,
; which derives every cpu row of tests/diag/expected.csv. Test n (from 1) is
; the n-th cpu row. Linear 16K at $C000.

        processor 6502
        ORG $C000
        include "lib.s"

ROM_ID  = 2

; RUN address, id: from the t 0 a WSYNC releases, 255 to TIM1T, 64 loads
; from the address, a WSYNC and INTIM; then record id with INTIM's value.
; Everything from the first WSYNC to the INTIM read is straight-line code.
        MAC RUN
        sta WSYNC
        ldx #255
        stx TIM1T
        REPEAT 64
        lda.w {1}
        REPEND
        sta WSYNC
        lda INTIM
        ldx #0
        ldy #{2}
        jsr mb_record
        ENDM

reset SUBROUTINE
        sei
        cld
        lda #$07                ; INPTCTRL: lock, MARIA on, BIOS off
        sta INPTCTRL
        lda #CTRL_OFF           ; DMA off for every test
        sta CTRL
        ldx #$FF
        txs
        lda #ROM_ID
        jsr mb_init
        RUN $0000, 1            ; tia
        RUN $0480, 2            ; riot_ram
        RUN $0280, 3            ; riot
        RUN $1800, 4            ; ram
        jsr mb_finish
        jmp show

        LIB_TAIL
