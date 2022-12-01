public _rmemcpy
public _bigint_add
public _bigint_sub
public _bigint_mul
public _bigint_iszero
public _bigint_setzero
public _bigint_isequal



; rmemcpy(void *dest, void *src, size_t len)
_rmemcpy:
; optimized by calc84maniac
    ld  iy, -3
    add iy, sp
    ld  bc, (iy + 12)
    sbc hl, hl
    add hl, bc
    ret nc
    ld  de, (iy + 9)
    add hl, de
    ld  de, (iy + 6)
.loop:
    ldi
    ret po
    dec hl
    dec hl
    jr  .loop


; bigint_iszero(uint8_t *op);
_bigint_iszero:
	pop hl,de
	push de,hl
	ld a, 0
	ld b, 34
.loop:
	or (hl)
	djnz .loop
	or a
	ret z
	ld a, 1
	ret


;bigint_setzero(uint8_t *op);
_bigint_setzero:
	pop de,hl
	push hl,de
	ld a, 0
	ld (de), a
	push de
	pop hl
	inc de
	ld bc, 31
	ldir


; bigint_isequal(uint8_t *op1, uint8_t *op2);
_bigint_isequal:
	call ti._frameset0
	ld hl, (ix + 3)
	ld de, (ix + 6)
	ld b, 32
.loop:
	ld a, (de)
	inc de
	xor (hl)
	djnz .loop
	add a, -1
	sbc a, a
	inc a
	ret


; bigint_add(uint8_t *op1, uint8_t *op2);
; hard limit to 34 bytes
; output in op1
; because its a binary field, add is xor
_bigint_add:
	ti._frameset0
	ld hl, (ix + 3)		; op2
	ld de, (ix + 6)		; op1
	ld b, 32
.loop:
	ld a,(de)
	;adc a,(hl)
	xor (hl)
	ld (de),a
	inc hl
	inc de
	djnz .loop
	ld sp, ix
	pop ix
	ret


; bigint_sub(uint8_t *op1, uint8_t *op2);
; on a binary field addition and subtraction are the same
_bigint_sub := _bigint_add
	

_src_zeros := FF0000h
; bigint_mul(uint8_t *op1, uint8_t *op2)
_bigint_mul:
	ld hl, 32
	ti._frameset
	ld de, ix - 32		; stack mem?
	ld hl, (ix + 6)		; op1 (save a copy)
	ld bc, 32
	ldir
	
	ld hl, (ix + 3)		; op2
	ld b, 32
.loop_op2
	ld a, (hl)
.loop:
	rl a
	jr nc, .set0
	lea de, ix - 32
	jr .skip_set0
.set0:
	ld de, _src_zeros
.skip_set0:
	push af,hl,bc
		ld hl, (ix + 6)
		ld b, 32
.loop_mul_and_add:
		rl (hl)				; shift byte of op1 left
		ld a, (hl)			; load byte into a
		ex de, hl			; get copy of op1 into hl
		xor (hl)			; add = xor. op1 + tmp
		ld (de), a			; result back in op1
		ex de, hl			; op1 back in hl
		djnz .loop_mul_and_add
	pop bc,hl,af
	inc hl
	djnz .loop_op2
	ld sp, ix
	pop ix
	ret
	

_bigint_mul2:
; hl = ptr to bigint
	push bc
	ld b, 32
	or a
.loop:
	rl (hl)
	inc hl
	djnz .loop
	pop bc
	ret
