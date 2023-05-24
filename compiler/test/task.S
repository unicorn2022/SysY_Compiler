
	.text
	.globl main
main:
	addi sp, sp, -76
	sw   ra, 72(sp)
entry_main:
	li   t0, 0
	sw   t0, 0(sp)

	li   t0, 1
	sw   t0, 4(sp)

	li   t0, 2
	sw   t0, 8(sp)

	lw   t0, 0(sp)
	sw   t0, 12(sp)

	lw   t0, 12(sp)
	li   t1, 0
	or   t0, t0, t1
	sw   t0, 16(sp)

	li   t0, 0
	lw   t1, 16(sp)
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 20(sp)

	lw   t0, 20(sp)
	bnez t0, then
	j    else

then:
	li   t0, 3
	sw   t0, 4(sp)

	j    end

else:
	li   t0, 3
	sw   t0, 8(sp)

	li   t0, 1
	sw   t0, 24(sp)

	lw   t0, 24(sp)
	sw   t0, 28(sp)

	lw   t0, 28(sp)
	li   t1, 0
	or   t0, t0, t1
	sw   t0, 32(sp)

	li   t0, 0
	lw   t1, 32(sp)
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 36(sp)

	lw   t0, 36(sp)
	bnez t0, then_1
	j    else_1

end:
	lw   t0, 0(sp)
	sw   t0, 40(sp)

	lw   t0, 40(sp)
	li   t1, 0
	add  t0, t0, t1
	sw   t0, 44(sp)

	lw   t0, 4(sp)
	sw   t0, 48(sp)

	lw   t0, 44(sp)
	lw   t1, 48(sp)
	add  t0, t0, t1
	sw   t0, 52(sp)

	lw   t0, 8(sp)
	sw   t0, 56(sp)

	lw   t0, 52(sp)
	lw   t1, 56(sp)
	add  t0, t0, t1
	sw   t0, 60(sp)

	lw   a0, 60(sp)
	lw   ra, 72(sp)
	addi sp, sp, 76
	ret

then_1:
	li   t0, 4
	sw   t0, 4(sp)

	j    end_1

else_1:
	li   t0, 4
	sw   t0, 8(sp)

	j    end_1

end_1:
	lw   t0, 24(sp)
	sw   t0, 64(sp)

	lw   t0, 64(sp)
	li   t1, 0
	sub  t0, t0, t1
	seqz t0, t0
	andi t0, t0, 0xff
	sw   t0, 68(sp)

	lw   t0, 68(sp)
	bnez t0, then_2
	j    end_2

then_2:
	li   a0, 1
	lw   ra, 72(sp)
	addi sp, sp, 76
	ret

end_2:
	j    end

