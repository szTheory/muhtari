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

; The tests: CTRL for line 100, then the DLL's address. The order is
; expected.csv's.
        MAC TESTROW
        .byte {1}, <({2}), >({2})
        ENDM
test_tab
        TESTROW CTRL_ON, dll_null               ; null
        TESTROW CTRL_ON, dll_null_last          ; null_last
        TESTROW CTRL_OFF, dll_null              ; off
        TESTROW CTRL_ON, dll_h4_w1x8            ; h4_w1x8
        TESTROW CTRL_ON, dll_h5_w1x8            ; h5_w1x8
        TESTROW CTRL_ON, dll_h4_w8              ; h4_w8
        TESTROW CTRL_ON, dll_h5_w32             ; h5_w32
        TESTROW CTRL_ON, dll_h5_w32_last        ; h5_w32_last
        TESTROW CTRL_ON, dll_dir_w8             ; dir_w8
        TESTROW CTRL_ON, dll_ind_w8             ; ind1_w8
        TESTROW CTRL_ON_CW2, dll_ind_w8         ; ind2_w8
        TESTROW CTRL_ON, dll_term_00            ; term_00
        TESTROW CTRL_ON, dll_term_20            ; term_20
        TESTROW CTRL_ON, dll_term_40            ; term_40
        TESTROW CTRL_ON, dll_term_80            ; term_80
        TESTROW CTRL_ON, dll_term_a0            ; term_a0
        TESTROW CTRL_ON, dll_term_c0            ; term_c0
        TESTROW CTRL_ON, dll_hole8_h4           ; hole8_h4
        TESTROW CTRL_ON, dll_hole8_h5           ; hole8_h5
        TESTROW CTRL_ON, dll_hole16_h4          ; hole16_h4
        TESTROW CTRL_ON, dll_hole16_h5          ; hole16_h5
        TESTROW CTRL_ON, dll_hole8_ctl          ; hole8_ctl
        TESTROW CTRL_ON, dll_over_11            ; over_11
        TESTROW CTRL_ON, dll_over_12            ; over_12
        TESTROW CTRL_ON, dll_over_13            ; over_13
        TESTROW CTRL_ON, dll_over_14            ; over_14
        TESTROW CTRL_ON, dll_over_16            ; over_16
TESTS   = (* - test_tab) / 3

; A DLL: zone 0 with entry byte 0 = {1} (DLI, H16, H8, OFFSET) and DL {2},
; then 16-line zones of the empty DL. OFFSET 1 makes line 100 an ordinary
; line, OFFSET 0 the zone's last.
        MAC DLL
        .byte {1}, >({2}), <({2})
        .byte $0F, >dl_null, <dl_null
        .byte $0F, >dl_null, <dl_null
        .byte $0F, >dl_null, <dl_null
        ENDM
ORD     = $01
LAST    = $00
H8      = $20
H16     = $40
dll_null        DLL ORD, dl_null
dll_null_last   DLL LAST, dl_null
dll_h4_w1x8     DLL ORD, dl_h4_w1x8
dll_h5_w1x8     DLL ORD, dl_h5_w1x8
dll_h4_w8       DLL ORD, dl_h4_w8
dll_h5_w32      DLL ORD, dl_h5_w32
dll_h5_w32_last DLL LAST, dl_h5_w32
dll_dir_w8      DLL ORD, dl_dir_w8
dll_ind_w8      DLL ORD, dl_ind_w8
dll_term_00     DLL ORD, dl_term_00
dll_term_20     DLL ORD, dl_term_20
dll_term_40     DLL ORD, dl_term_40
dll_term_80     DLL ORD, dl_term_80
dll_term_a0     DLL ORD, dl_term_a0
dll_term_c0     DLL ORD, dl_term_c0
dll_hole8_h4    DLL ORD|H8, dl_hole8_h4
dll_hole8_h5    DLL ORD|H8, dl_hole8_h5
dll_hole16_h4   DLL ORD|H16, dl_hole16_h4
dll_hole16_h5   DLL ORD|H16, dl_hole16_h5
dll_hole8_ctl   DLL ORD|H8, dl_hole16_h4
dll_over_11     DLL ORD, dl_over + 4*(16-11)
dll_over_12     DLL ORD, dl_over + 4*(16-12)
dll_over_13     DLL ORD, dl_over + 4*(16-13)
dll_over_14     DLL ORD, dl_over + 4*(16-14)
dll_over_16     DLL ORD, dl_over

; Headers (docs/hardware/maria.md, Display lists). H4 is a 4-byte header of
; width {2} at {1}; H5 a 5-byte one, byte 1 {3}: $40 direct, $60 indirect.
; Palette 0, hpos 0. TERM is a terminator, read whole.
        MAC H4
        .byte <({1}), 32-({2}), >({1}), 0
        ENDM
        MAC H5
        .byte <({1}), {3}, >({1}), (32-({2}))&$1F, 0
        ENDM
        MAC TERM
        .byte $00, $00, $00, $00
        ENDM
GFX     = $E000                 ; no hole: A12 and A11 clear
HOLE8   = $C800                 ; A11: a hole under H8
HOLE16  = $D000                 ; A12: a hole under H16

dl_null TERM
dl_h4_w1x8
        REPEAT 8
        H4 GFX, 1
        REPEND
        TERM
dl_h5_w1x8
        REPEAT 8
        H5 GFX, 1, $40
        REPEND
        TERM
dl_h4_w8
        H4 GFX, 8
        TERM
dl_h5_w32
        H5 GFX, 32, $40
        TERM
dl_dir_w8
        H5 GFX, 8, $40
        TERM
dl_ind_w8
        H5 GFX, 8, $60
        TERM
; Byte 1 of the first header takes each value; $40 and $C0 read as a 5-byte
; header of width 8.
dl_term_00
        .byte $00, $00, >GFX, $18, $00
        TERM
dl_term_20
        .byte $00, $20, >GFX, $18, $00
        TERM
dl_term_40
        .byte $00, $40, >GFX, $18, $00
        TERM
dl_term_80
        .byte $00, $80, >GFX, $18, $00
        TERM
dl_term_a0
        .byte $00, $A0, >GFX, $18, $00
        TERM
dl_term_c0
        .byte $00, $C0, >GFX, $18, $00
        TERM
dl_hole8_h4
        REPEAT 8
        H4 HOLE8, 8
        REPEND
        TERM
dl_hole8_h5
        REPEAT 8
        H5 HOLE8, 8, $40
        REPEND
        TERM
dl_hole16_h4
        REPEAT 8
        H4 HOLE16, 8
        REPEND
        TERM
dl_hole16_h5
        REPEAT 8
        H5 HOLE16, 8, $40
        REPEND
        TERM
; Sixteen objects of width 8; over_n starts 16 - n headers in.
dl_over
        REPEAT 16
        H4 GFX, 8
        REPEND
        TERM

        LIB_TAIL
