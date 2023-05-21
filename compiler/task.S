
	.text
	.globl main
main:
	addi sp, sp, -16
	sw   ra, 12(sp)
entry:
	li   t0, 0
	li   t1, 2
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 0(sp)

	li   t0, 0
	li   t1, 4
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 4(sp)

	lw   t0, 0(sp)
	lw   t1, 4(sp)
	and  t0, t0, t1
	sw   t0, 8(sp)

	lw   a0, 8(sp)
	lw   ra, 12(sp)
	addi sp, sp, 16
	ret

