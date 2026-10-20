; SPDX-License-Identifier: MIT
; The DMA cost diagnostic. Each test points zone 0 at a display list and
; measures the CPU cycles one line's DMA takes from it, as an INTIM value:
; see docs/hardware/maria.md, Diagnostic expected values, which derives every
; value in tests/diag/expected.csv. Test n (from 1) is the n-th row of the
; table below and the n-th dma row of expected.csv. Linear 16K at $C000.

        processor 6502
        ORG $C000
        include "lib.s"

ROM_ID  = 1
; The zero-page test index, after lib.s's bytes.
TEST_I  = $85
; WSYNC writes from line 16 to line 99; line 100 is measured.
LINES   = 83

reset SUBROUTINE
        sei
        cld
        lda #$07                ; INPTCTRL: lock, MARIA on, BIOS off
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        lda #$E0                ; indirect tests' characters
        sta CHARBASE
        ldx #$FF
        txs
        lda #ROM_ID
        jsr mb_init
        lda #0
        sta TEST_I
.next   jsr wait_vblank
        lda TEST_I
        cmp #TESTS
        beq .done
        asl
        adc TEST_I              ; 3 bytes per test; carry is clear
        tay
        lda test_tab+1,y
        sta DPPL
        lda test_tab+2,y
        sta DPPH
        lda #CTRL_ON            ; DMA on for VBE at line 16
        sta CTRL
        jsr wait_visible
        lda #CTRL_OFF           ; and off before line 17's t 28
        sta CTRL
        lda test_tab,y
        jsr measure
        ldx #0
        ldy TEST_I
        iny
        jsr mb_record
        inc TEST_I
        jmp .next
.done   jsr mb_finish
        jmp show

; measure: line 100 runs with CTRL = A, every other line with DMA off.
; Returns INTIM read 10 cycles after a write of 255 to TIM1T at line 100's
; t 0, less the cycles line 100's burst froze.
measure SUBROUTINE
        ldx #LINES
.line   sta WSYNC
        dex
        bne .line
        nop                     ; line 99: past its t 28
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        sta CTRL
        ldx #255
        ldy #CTRL_OFF
        sta WSYNC
        stx TIM1T               ; line 100, t 0-3: p = 0
        nop
        nop
        nop
        lda INTIM               ; p = 10
        sty CTRL
        rts

; The tests: CTRL for line 100, then the DLL's address.
        MAC TESTROW
        .byte {1}, <{2}, >{2}
        ENDM
test_tab
        TESTROW CTRL_ON, dll_null          ; null
TESTS   = (* - test_tab) / 3

; A DLL: zone 0 with entry byte 0 = {1} (DLI, H16, H8, OFFSET) and DL {2},
; then 16-line zones of the empty DL.
        MAC DLL
        .byte {1}, >{2}, <{2}
        .byte $0F, >dl_null, <dl_null
        .byte $0F, >dl_null, <dl_null
        .byte $0F, >dl_null, <dl_null
        ENDM
dll_null
        DLL $01, dl_null

; The DLs.
dl_null
        .byte $00, $00, $00, $00

        LIB_TAIL
