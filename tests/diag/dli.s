; SPDX-License-Identifier: MIT
; The DLI, WSYNC, RDY and MSTAT diagnostic. Each test starts the RIOT timer
; in the first instruction after a WSYNC releases and reports INTIM, or a
; count of lines or polls: see docs/hardware/maria.md, The dli ROM, which
; derives every dli row of tests/diag/expected.csv. Test n (from 1) is the
; n-th dli row. Linear 16K at $C000.

        processor 6502
        ORG $C000
        include "lib.s"

ROM_ID  = 3
; Zero-page bytes after lib.s's: the result, low byte first; the stack
; pointer the NMI handler returns with; the DLL and the WSYNC count of a
; DLI test; the clear edge's line count.
RESULT  = $86
SAVE_S  = $88
DLL_LO  = $89
DLL_HI  = $8A
LINES_N = $8B
CLEAR_N = $8C

; WSYNCRUN k: the timer from a WSYNC's t 0, k more WSYNCs, then INTIM.
        MAC WSYNCRUN
        ldx #255
        sta WSYNC
        stx TIM1T
        REPEAT {1}
        sta WSYNC
        REPEND
        lda INTIM
        ENDM

; DLITEST id, dll, lines: runs zone_test on the DLL with the timer on line
; 16 + lines - 20, then records the result.
        MAC DLITEST
        lda #<({2})
        sta DLL_LO
        lda #>({2})
        sta DLL_HI
        lda #{3}
        sta LINES_N
        jsr zone_test
        lda RESULT
        ldx RESULT+1
        ldy #{1}
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
        DLITEST 1, dll_z1, 20   ; zone1
        DLITEST 2, dll_z2, 21   ; zone2
        DLITEST 3, dll_z0, 20   ; zone0
        jsr parked              ; parked
        lda RESULT
        ldx RESULT+1
        ldy #4
        jsr mb_record
        WSYNCRUN 1              ; wsync_1
        ldx #0
        ldy #5
        jsr mb_record
        WSYNCRUN 3              ; wsync_3
        ldx #0
        ldy #6
        jsr mb_record
        jsr rdy_store           ; rdy_store
        ldx #0
        ldy #7
        jsr mb_record
        jsr mstat_lines         ; set_lines, clear_lines
        jsr mstat_polls         ; set_polls, clear_polls
        jsr mb_finish
        jmp show

; dli_frame: in VBLANK, from line 258's t 42, DMA on at the DLL for VBE,
; then LINES_N WSYNCs, which end on line 258 + LINES_N.
dli_frame SUBROUTINE
        jsr wait_visible
        jsr wait_vblank
        lda DLL_LO
        sta DPPL
        lda DLL_HI
        sta DPPH
        lda #CTRL_ON
        sta CTRL
        ldx LINES_N
.line   sta WSYNC
        dex
        bne .line
        rts

; zone_test: one more WSYNC, the timer at its t 0, then `lda $80` and NOPs,
; so each NOP's opcode fetch is an odd cycle; the DLI's NMI handler reads
; INTIM and returns from here. $FFFF if no NMI comes.
zone_test SUBROUTINE
        tsx
        stx SAVE_S
        jsr dli_frame
        ldx #255
        sta WSYNC
        stx TIM1T
        lda $80
        REPEAT 128
        nop
        REPEND
timeout lda #$FF
        sta RESULT
        sta RESULT+1
        lda #CTRL_OFF
        sta CTRL
        rts

; parked: the zone1 DLL; the timer on line 16, a slow read, then WSYNC to
; line 17 and WSYNC again there, so line 17's DLI comes while the CPU is
; held; the NMI follows the NOP after the hold.
parked SUBROUTINE
        tsx
        stx SAVE_S
        lda #<dll_z1
        sta DLL_LO
        lda #>dll_z1
        sta DLL_HI
        lda #20
        sta LINES_N
        jsr dli_frame
        ldx #255
        sta WSYNC
        stx TIM1T
        bit $00
        sta WSYNC
        sta WSYNC
        nop
        REPEAT 16
        nop
        REPEND
        jmp timeout

; The NMI handler: INTIM first, then DMA off, and back to the test's caller.
diag_nmi SUBROUTINE
        lda INTIM
        sta RESULT
        lda #0
        sta RESULT+1
        lda #CTRL_OFF
        sta CTRL
        ldx SAVE_S
        txs
        rts

; rdy_store: zone 0 loaded at VBE and DMA off, as in the dma ROM; the timer
; on line 100; on line 101 two NOPs after the hold, then DMA on, written at
; t 28 or later, and INTIM.
rdy_store SUBROUTINE
        jsr wait_visible
        jsr wait_vblank
        lda #<dll_rdy
        sta DPPL
        lda #>dll_rdy
        sta DPPH
        lda #CTRL_ON
        sta CTRL
        jsr wait_visible
        lda #CTRL_OFF
        sta CTRL
        ldx #83                 ; WSYNC from line 16 to line 99
.line   sta WSYNC
        dex
        bne .line
        ldx #255
        lda #CTRL_ON
        ldy #CTRL_OFF
        sta WSYNC
        stx TIM1T               ; line 100
        sta WSYNC
        nop                     ; line 101
        nop
        sta CTRL
        lda INTIM
        sty CTRL
        rts

; mstat_lines: from line 16, lines until a line's early read sees VBLANK
; set, then lines until one sees it clear.
mstat_lines SUBROUTINE
        jsr wait_visible
        jsr wait_vblank
        jsr wait_visible
        ldx #0
.set    sta WSYNC
        inx
        bit MSTAT
        bpl .set
        ldy #0
.clear  sta WSYNC
        iny
        bit MSTAT
        bmi .clear
        sty CLEAR_N
        txa
        ldx #0
        ldy #8
        jsr mb_record
        lda CLEAR_N
        ldx #0
        ldy #9
        jsr mb_record
        rts

; mstat_polls: WSYNC to t 0 of line 258, then polls until MSTAT reads set;
; WSYNC to t 0 of line 16, then polls until it reads clear.
mstat_polls SUBROUTINE
        jsr wait_vblank
        jsr wait_visible
        ldy #0
        ldx #242                ; from line 16 to line 258
.to_set sta WSYNC
        dex
        bne .to_set
.set    iny
        bit MSTAT
        bpl .set
        IF >.set != >*         ; a page crossing would add a cycle
        ERR
        ENDIF
        tya
        ldx #0
        ldy #10
        jsr mb_record
        jsr wait_visible
        jsr wait_vblank
        ldy #0
        ldx #21                 ; from line 258 to line 16
.to_clr sta WSYNC
        dex
        bne .to_clr
.clear  iny
        bit MSTAT
        bmi .clear
        IF >.clear != >*         ; a page crossing would add a cycle
        ERR
        ENDIF
        tya
        ldx #0
        ldy #11
        jsr mb_record
        rts

; The DLLs: zone byte 0 is DLI (bit 7) and OFFSET; every DL is empty.
        MAC ZONE
        .byte {1}, >dl_null, <dl_null
        ENDM
        MAC REST
        REPEAT 16
        ZONE $0F
        REPEND
        ENDM
dll_z1  ZONE $00
        ZONE $8F
        REST
dll_z2  ZONE $00
        ZONE $00
        ZONE $8F
        REST
dll_z0  ZONE $80
        REST
dll_rdy ZONE $01
        REST
dl_null .byte $00, $00, $00, $00

        LIB_TAIL
