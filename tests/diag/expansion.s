; SPDX-License-Identifier: MIT
; Owned RAM/POKEY/HSC stimulus; docs/hardware/cartridge.md, XBoard and XM.
; Upper ROM keeps code outside every overlay. $F700..$F706 select profile,
; CPU page, right CPU page, MARIA pages, sound gate, ignored-store offset,
; and DLL window. These are inputs, never precomputed observations.
        processor 6502
        ORG $4000
        ds $8000, $31
        include "lib.s"
ROM_ID = 21
PAGE = $85
PTR = $86
DL = $88
BASE = $8A
SUM_LO = $8B
SUM_HI = $8C
PHASE = $8D
        MAC READ
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
        READ $4000, 1
        READ $7FFF, 2
        ; The host restored physical HSC before this initially hidden store.
        lda $F700
        cmp #1
        beq .initialhsc
        lda #$99
        sta $1100
.initialhsc
        lda #0
        sta PAGE
        sta SUM_LO
        sta SUM_HI
.init   lda $F700
        cmp #1
        beq .xb
        lda PAGE
        sta $0478
        lda #$20
        sta $0470
        jmp .fill
.xb     lda PAGE
        ora #8
        sta $0470
.fill   lda PAGE
        clc
        adc #$40
        sta $4000
        lda PAGE
        clc
        adc #$80
        sta $5FFF
        lda $F700
        cmp #1
        bne .dll
        lda PAGE
        clc
        adc #$60
        sta $6000
        lda PAGE
        clc
        adc #$A0
        sta $7FFF
.dll    ; Every page owns a DLL; VBE must read the CPU page, later zones
        ; the electrically HALTed MARIA page. OFFSET0 keeps endpoints exact.
        lda #0
        sta PTR
        lda #$41
        sta PTR+1
        lda PAGE
        asl
        asl
        asl
        asl
        asl
        sta DL
        lda PAGE
        lsr
        lsr
        lsr
        clc
        adc #$18
        sta DL+1
        ldx #0
.zone   ldy #0
        lda #0
        sta (PTR),y
        iny
        lda DL+1
        sta (PTR),y
        iny
        lda DL
        sta (PTR),y
        clc
        lda PTR
        adc #3
        sta PTR
        bcc .next
        inc PTR+1
.next   inx
        bne .zone
        ; Four one-byte objects, endpoints of both windows. The DL selected
        ; by a page also moves each object horizontally by two clocks/page.
        lda DL
        sta PTR
        lda DL+1
        sta PTR+1
        ldy #0
        ldx #0
.object lda object_bytes,x
        sta (PTR),y
        iny
        inx
        cpx #16
        bne .object
        lda #0
        sta (PTR),y
        iny
        sta (PTR),y
        lda PAGE
        asl
        ldy #3
.position
        sta (PTR),y
        clc
        adc #8
        iny
        iny
        iny
        iny
        cpy #19
        bne .position
        inc PAGE
        lda $F700
        cmp #1
        beq .xbcount
        lda PAGE
        cmp #16
        bne .again
        jmp .select
.xbcount
        lda PAGE
        cmp #8
        beq .select
.again  jmp .init
.select jsr select
        READ $4000, 3
        READ $5FFF, 4
        READ $6000, 5
        READ $7FFF, 6
        lda #0
        sta $0470
        READ $4000, 7
        READ $7FFF, 8
        jsr select
        READ $4000, 9
        READ $1100, 10
        READ $17FF, 11
        READ $3010, 12
        lda #$A5
        sta $1100
        READ $1100, 13
        lda #$3C
        sta $1101
        READ $1101, 14
        lda $F700
        cmp #1
        beq .retained
        lda BASE
        and #$F7
        sta $0470
        lda #$99
        sta $1101
        lda BASE
        sta $0470
.retained
        READ $1101, 15
        lda $F700
        cmp #1
        bne .xmabsent
        ; All sixteen XBoard aliases select eight banks twice, measured sum.
        ldx #0
.alias  txa
        and #7
        ora #8
        sta $0470,x
        clc
        lda SUM_LO
        adc $4000
        sta SUM_LO
        bcc .aliasnext
        inc SUM_HI
.aliasnext
        inx
        cpx #16
        bne .alias
        lda SUM_LO
        ldx SUM_HI
        jmp .aliasrecord
.xmabsent
        ; These absent XM controls/unsupported bits must not change RAM.
        lda #$FF
        sta $0471
        sta $0472
        lda BASE
        ora #$87
        sta $0470
        lda $4000
        ldx #0
.aliasrecord
        ldy #16
        jsr mb_record
        jsr select
        READ $4000, 17
        READ $3010, 18
        ; Configure installed primary tone/timer and optional declared $440.
        lda #3
        sta $045F
        sta $044F
        lda #7
        sta $0450
        lda #$AF
        sta $0451
        lda #17
        sta $0440
        lda #$A9
        sta $0441
        lda #1
        sta $045E
        lda #0
        sta $0459
        sta $0449
        lda BASE
        and $F704
        sta BASE
        sta $0470
        ; Exactly equal instruction timing in gate/control/ignored-write runs.
        ; $460 is absent. A disabled SKCTL write must not reset the primary.
        ldx $F705
        lda #0
        sta $045F,x
        ldx #$FF
.wait   dex
        bne .wait
        lda BASE
        ora #$10
        sta $0470
        READ $045E, 19
        lda BASE
        sta $0470
        lda #0
        sta PHASE
        lda $F706
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
        jsr mb_finish
.forever
        jsr wait_vblank
        inc PHASE
        lda $F700
        cmp #1
        beq .visible
        lda PHASE
        and #1
        beq .visible
        lda BASE
        and #$F7
        sta $0470
        lda #$99
        sta $1101
        jmp .waitvisible
.visible
        lda BASE
        sta $0470
        lda #$A5
        sta $1100
.waitvisible
        jsr wait_visible
        jmp .forever
select SUBROUTINE
        lda $F700
        cmp #1
        beq .xb
        lda $F702
        asl
        asl
        asl
        asl
        ora $F701
        sta $0478
        lda $F703
        sta $047C
        lda #$78
        bne .set
.xb     lda $F701
        ora #$18
.set    sta BASE
        sta $0470
        rts
object_bytes
        .byte 0,31,$40,0, $FF,31,$5F,8
        .byte 0,31,$60,16, $FF,31,$7F,24
        ds $F6F0-*, 0
        .byte "EXPAND01"
        ds $F700-*, 0
        .byte 2,0,15,$78,$FF,16,$41
        LIB_TAIL
