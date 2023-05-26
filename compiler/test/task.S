	.data
	.global a
a:
	.word 10


	.text
	.globl inc
inc:
	addi sp, sp, -16
	sw   ra, 12(sp)
entry_inc:
	la   t1, a
	lw   t0, 0(t1)
	sw   t0, 0(sp)

	lw   t0, 0(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 4(sp)

	lw   t0, 4(sp)
	la   t1, a
	sw   t0, 0(t1)

	la   t1, a
	lw   t0, 0(t1)
	sw   t0, 8(sp)

	lw   a0, 8(sp)
	lw   ra, 12(sp)
	addi sp, sp, 16
	ret


	.text
	.globl print_a
print_a:
	addi sp, sp, -8
	sw   ra, 4(sp)
entry_print_a:
	la   t1, a
	lw   t0, 0(t1)
	sw   t0, 0(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 4(sp)
	call putint
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	lw   ra, 4(sp)
	addi sp, sp, 8
	ret


	.text
	.globl main
main:
	addi sp, sp, -56
	sw   ra, 52(sp)
entry_main:
	li   t0, 0
	sw   t0, 0(sp)

	j    while_entry

while_entry:
	lw   t0, 0(sp)
	sw   t0, 4(sp)

	lw   t0, 4(sp)
	li   t1, 10
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 8(sp)

	lw   t0, 8(sp)
	bnez t0, while_body
	j    while_end

while_body:
	lw   t0, 0(sp)
	sw   t0, 12(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 16(sp)
	call putint
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	call inc
	addi sp, sp, 4
	sw   a0, 20(sp)
	lw   a0, -4(sp)

	lw   t0, 20(sp)
	sw   t0, 16(sp)

	lw   t0, 16(sp)
	sw   t0, 24(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 28(sp)
	call putint
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	li   t0, 1
	sw   t0, 28(sp)

	lw   t0, 28(sp)
	sw   t0, 32(sp)

	lw   t0, 32(sp)
	li   t1, 2
	add  t0, t0, t1
	sw   t0, 36(sp)

	lw   t0, 36(sp)
	sw   t0, 28(sp)

	lw   t0, 28(sp)
	sw   t0, 40(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 44(sp)
	call putint
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	call print_a
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 10
	call putch
	addi sp, sp, 4
	lw   a0, -4(sp)

	lw   t0, 0(sp)
	sw   t0, 44(sp)

	lw   t0, 44(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 48(sp)

	lw   t0, 48(sp)
	sw   t0, 0(sp)

	j    while_entry

while_end:
	li   a0, 0
	lw   ra, 52(sp)
	addi sp, sp, 56
	ret

