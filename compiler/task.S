	.data
	.global x
x:
	.zero 8


	.text
	.globl main
main:
	addi sp, sp, -52
	sw   ra, 48(sp)
entry:
	addi t0, sp, 0
	li   t1, 0
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 20(sp)

	li   t0, 1
	lw   t1, 20(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 1
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 24(sp)

	li   t0, 2
	lw   t1, 24(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 2
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 28(sp)

	li   t0, 3
	lw   t1, 28(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 3
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 32(sp)

	li   t0, 0
	lw   t1, 32(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 4
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 36(sp)

	li   t0, 0
	lw   t1, 36(sp)
	sw   t0, 0(t1)

	addi t0, sp, 0
	li   t1, 2
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 40(sp)

	lw   t1, 40(sp)
	lw   t0, 0(t1)
	sw   t0, 44(sp)

	lw   a0, 44(sp)
	lw   ra, 48(sp)
	addi sp, sp, 52
	ret

