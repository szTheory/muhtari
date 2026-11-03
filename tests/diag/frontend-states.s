; SPDX-License-Identifier: MIT
; Owned live-progress and HSC diagnostic. The marker and counter at $2300
; are outside lib.s's $1800 display map and $2200 mailbox. The HSC signature
; bytes come from the built-in signature-only cartridge stub; no firmware is
; required. Hardware: docs/hardware/maria.md and docs/hardware/cartridge.md.
        processor 6502
        ORG $C000
        include "lib.s"

PROGRESS_MAP = SHOW_MAP + 2*7 + 3
MARKER       = $2300
COUNT_LO     = $2304
COUNT_HI     = $2305
HSC_LO       = $1100
HSC_HI       = $1101

reset SUBROUTINE
        sei
        cld
        ldx #$FF
        txs
        lda #$07                ; INPTCTRL: lock, MARIA on, BIOS off
        sta INPTCTRL
        lda #CTRL_OFF
        sta CTRL
        lda #'M
        sta MARKER
        lda #'U
        sta MARKER+1
        lda #'H
        sta MARKER+2
        lda #'T
        sta MARKER+3
        lda #0
        sta COUNT_LO
        sta COUNT_HI
        sta HSC_LO
        sta HSC_HI
        ldx #SHOW_ROWS*7-1
        lda #SHOW_BLANK
.blank  sta SHOW_MAP,x
        dex
        cpx #$FF
        bne .blank
        jsr update_progress
        jsr wait_vblank
        lda #>show_dll
        sta DPPH
        lda #<show_dll
        sta DPPL
        lda #>FONT
        sta CHARBASE
        lda #0
        sta BACKGRND
        lda #$0F
        sta P0C1
        sta P0C2
        sta P0C3
        lda #CTRL_ON
        sta CTRL
        jsr wait_visible       ; keep startup value 0000 through its first picture
.frame  jsr wait_vblank
        inc COUNT_LO
        bne .committed
        inc COUNT_HI
.committed
        lda COUNT_LO
        sta HSC_LO
        lda COUNT_HI
        sta HSC_HI
        jsr update_progress
        jsr wait_visible
        jmp .frame

; Four hex digits occupy the value field of display row 2, matching lib.s's
; 3x5 double-width glyphs and its independent image coordinates.
update_progress SUBROUTINE
        lda COUNT_HI
        lsr
        lsr
        lsr
        lsr
        sta PROGRESS_MAP
        lda COUNT_HI
        and #$0F
        sta PROGRESS_MAP+1
        lda COUNT_LO
        lsr
        lsr
        lsr
        lsr
        sta PROGRESS_MAP+2
        lda COUNT_LO
        and #$0F
        sta PROGRESS_MAP+3
        rts

        LIB_TAIL
