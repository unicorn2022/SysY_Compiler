
	.text
	.globl f
f:
	addi sp, sp, -20
	sw   ra, 16(sp)
entry:
	mv   t0, a0
	sw   t0, 0(sp)

	lw   t0, 0(sp)
	sw   t0, 4(sp)

	lw   t0, 4(sp)
	li   t1, 1
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 8(sp)

	lw   t0, 8(sp)
	lw   t0, 0(t0)
	sw   t0, 12(sp)

	lw   a0, 12(sp)
	lw   ra, 16(sp)
	addi sp, sp, 20
	ret


	.text
	.globl main
main:
	addi sp, sp, -28
	sw   ra, 24(sp)
entry:
	addi t0, sp, 0
	li   t1, 0
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 8(sp)

	li   t0, 1
	lw   t1, 8(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 1
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 12(sp)

	li   t0, 2
	lw   t1, 12(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 0
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 16(sp)

	lw   a0, 16(sp)
	call f
	sw   a0, 20(sp)

	lw   a0, 0(sp)
	lw   ra, 24(sp)
	addi sp, sp, 28
	ret

