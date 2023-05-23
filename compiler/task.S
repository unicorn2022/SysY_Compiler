
	.text
	.globl main
main:
	addi sp, sp, -4
	sw   ra, 0(sp)
entry:
	li   a0, 0
	lw   ra, 0(sp)
	addi sp, sp, 4
	ret

