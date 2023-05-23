	.data
	.global var
var:
	.word 14

	.data
	.global x
x:
	.word 13


	.text
	.globl main
main:
	addi sp, sp, -20
	sw   ra, 16(sp)
entry:
	la   t0, var
	lw   t0, 0(t0)
	sw   t0, 0(sp)

	la   t0, x
	lw   t0, 0(t0)
	sw   t0, 4(sp)

	lw   t0, 0(sp)
	lw   t1, 4(sp)
	add  t0, t0, t1
	sw   t0, 8(sp)

	addi sp, sp, -32
	sw   a0, 0(sp)
	sw   a1, 4(sp)
	sw   a2, 8(sp)
	sw   a3, 12(sp)
	sw   a4, 16(sp)
	sw   a5, 20(sp)
	sw   a6, 24(sp)
	sw   a7, 28(sp)
	call getch
	addi sp, sp, 32
	sw   a0, 12(sp)
	lw   a0, -32(sp)
	lw   a1, -28(sp)
	lw   a2, -24(sp)
	lw   a3, -20(sp)
	lw   a4, -16(sp)
	lw   a5, -12(sp)
	lw   a6, -8(sp)
	lw   a7, -4(sp)

	addi sp, sp, -32
	sw   a0, 0(sp)
	sw   a1, 4(sp)
	sw   a2, 8(sp)
	sw   a3, 12(sp)
	sw   a4, 16(sp)
	sw   a5, 20(sp)
	sw   a6, 24(sp)
	sw   a7, 28(sp)
	lw   a0, 44(sp)
	call putch
	addi sp, sp, 32
	lw   a0, -32(sp)
	lw   a1, -28(sp)
	lw   a2, -24(sp)
	lw   a3, -20(sp)
	lw   a4, -16(sp)
	lw   a5, -12(sp)
	lw   a6, -8(sp)
	lw   a7, -4(sp)

	addi sp, sp, -32
	sw   a0, 0(sp)
	sw   a1, 4(sp)
	sw   a2, 8(sp)
	sw   a3, 12(sp)
	sw   a4, 16(sp)
	sw   a5, 20(sp)
	sw   a6, 24(sp)
	sw   a7, 28(sp)
	lw   a0, 44(sp)
	call putch
	addi sp, sp, 32
	lw   a0, -32(sp)
	lw   a1, -28(sp)
	lw   a2, -24(sp)
	lw   a3, -20(sp)
	lw   a4, -16(sp)
	lw   a5, -12(sp)
	lw   a6, -8(sp)
	lw   a7, -4(sp)

	lw   a0, 12(sp)
	lw   ra, 16(sp)
	addi sp, sp, 20
	ret

