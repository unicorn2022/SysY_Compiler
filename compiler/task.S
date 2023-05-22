
	.text
	.globl half
half:
	addi sp, sp, -16
	sw   ra, 12(sp)
entry_half:
	mv   t0, a0
	sw   t0, 0(sp)

	lw   t0, 0(sp)
	sw   t0, 4(sp)

	lw   t0, 4(sp)
	li   t1, 2
	div  t0, t0, t1
	sw   t0, 8(sp)

	lw   a0, 8(sp)
	lw   ra, 12(sp)
	addi sp, sp, 16
	ret


	.text
	.globl f
f:
	addi sp, sp, -8
	sw   ra, 4(sp)
entry_f:
	addi sp, sp, -32
	sw   a0, 0(sp)
	sw   a1, 4(sp)
	sw   a2, 8(sp)
	sw   a3, 12(sp)
	sw   a4, 16(sp)
	sw   a5, 20(sp)
	sw   a6, 24(sp)
	sw   a7, 28(sp)
	mv   a0, a0
	call half
	addi sp, sp, 32
	sw   a0, 0(sp)
	lw   a0, -32(sp)
	lw   a1, -28(sp)
	lw   a2, -24(sp)
	lw   a3, -20(sp)
	lw   a4, -16(sp)
	lw   a5, -12(sp)
	lw   a6, -8(sp)
	lw   a7, -4(sp)

	lw   a0, 0(sp)
	lw   ra, 4(sp)
	addi sp, sp, 8
	ret


	.text
	.globl main
main:
	addi sp, sp, -8
	sw   ra, 4(sp)
entry_main:
	addi sp, sp, -32
	sw   a0, 0(sp)
	sw   a1, 4(sp)
	sw   a2, 8(sp)
	sw   a3, 12(sp)
	sw   a4, 16(sp)
	sw   a5, 20(sp)
	sw   a6, 24(sp)
	sw   a7, 28(sp)
	li   a0, 1
	call f
	addi sp, sp, 32
	sw   a0, 0(sp)
	lw   a0, -32(sp)
	lw   a1, -28(sp)
	lw   a2, -24(sp)
	lw   a3, -20(sp)
	lw   a4, -16(sp)
	lw   a5, -12(sp)
	lw   a6, -8(sp)
	lw   a7, -4(sp)

	lw   a0, 0(sp)
	lw   ra, 4(sp)
	addi sp, sp, 8
	ret

