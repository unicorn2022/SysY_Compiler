	.data
	.global buf
buf:
	.zero 800


	.text
	.globl merge_sort
merge_sort:
	li   t0, 400
	add  sp, sp, t0
	sw   ra, 396(sp)
entry_merge_sort:
	mv   t0, a0
	sw   t0, 0(sp)

	mv   t0, a1
	sw   t0, 4(sp)

	lw   t0, 0(sp)
	sw   t0, 8(sp)

	lw   t0, 8(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 12(sp)

	lw   t0, 4(sp)
	sw   t0, 16(sp)

	lw   t0, 12(sp)
	lw   t1, 16(sp)
	slt  t0, t0, t1
	xori t0, t0, 1
	andi t0, t0, 0xff
	sw   t0, 20(sp)

	lw   t0, 20(sp)
	bnez t0, then
	j    if_end

then:
	lw   ra, 396(sp)
	li   t0, 400
	add  sp, sp, t0
	ret

if_end:
	lw   t0, 0(sp)
	sw   t0, 28(sp)

	lw   t0, 4(sp)
	sw   t0, 32(sp)

	lw   t0, 28(sp)
	lw   t1, 32(sp)
	add  t0, t0, t1
	sw   t0, 36(sp)

	lw   t0, 36(sp)
	li   t1, 2
	div  t0, t0, t1
	sw   t0, 40(sp)

	lw   t0, 40(sp)
	sw   t0, 24(sp)

	lw   t0, 0(sp)
	sw   t0, 44(sp)

	lw   t0, 24(sp)
	sw   t0, 48(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 48(sp)
	lw   a1, 52(sp)
	call merge_sort
	addi sp, sp, 4
	lw   a0, -4(sp)

	lw   t0, 24(sp)
	sw   t0, 52(sp)

	lw   t0, 4(sp)
	sw   t0, 56(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 56(sp)
	lw   a1, 60(sp)
	call merge_sort
	addi sp, sp, 4
	lw   a0, -4(sp)

	lw   t0, 0(sp)
	sw   t0, 64(sp)

	lw   t0, 64(sp)
	sw   t0, 60(sp)

	lw   t0, 24(sp)
	sw   t0, 72(sp)

	lw   t0, 72(sp)
	sw   t0, 68(sp)

	lw   t0, 0(sp)
	sw   t0, 80(sp)

	lw   t0, 80(sp)
	sw   t0, 76(sp)

	j    while_entry

while_entry:
	lw   t0, 60(sp)
	sw   t0, 84(sp)

	lw   t0, 24(sp)
	sw   t0, 88(sp)

	lw   t0, 84(sp)
	lw   t1, 88(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 92(sp)

	lw   t0, 68(sp)
	sw   t0, 96(sp)

	lw   t0, 4(sp)
	sw   t0, 100(sp)

	lw   t0, 96(sp)
	lw   t1, 100(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 104(sp)

	li   t0, 0
	lw   t1, 92(sp)
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 108(sp)

	li   t0, 0
	lw   t1, 104(sp)
	sub  t0, t0, t1
	snez t0, t0
	andi t0, t0, 0xff
	sw   t0, 112(sp)

	lw   t0, 108(sp)
	lw   t1, 112(sp)
	and  t0, t0, t1
	sw   t0, 116(sp)

	lw   t0, 116(sp)
	bnez t0, while_body
	j    while_end

while_body:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 120(sp)

	lw   t0, 60(sp)
	sw   t0, 124(sp)

	lw   t0, 120(sp)
	lw   t1, 124(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 128(sp)

	lw   t1, 128(sp)
	lw   t0, 0(t1)
	sw   t0, 132(sp)

	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 136(sp)

	lw   t0, 68(sp)
	sw   t0, 140(sp)

	lw   t0, 136(sp)
	lw   t1, 140(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 144(sp)

	lw   t1, 144(sp)
	lw   t0, 0(t1)
	sw   t0, 148(sp)

	lw   t0, 132(sp)
	lw   t1, 148(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 152(sp)

	lw   t0, 152(sp)
	bnez t0, then_1
	j    else_1

while_end:
	j    while_entry_1

then_1:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 156(sp)

	lw   t0, 60(sp)
	sw   t0, 160(sp)

	lw   t0, 156(sp)
	lw   t1, 160(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 164(sp)

	lw   t1, 164(sp)
	lw   t0, 0(t1)
	sw   t0, 168(sp)

	la   t0, buf
	li   t1, 1
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 172(sp)

	lw   t0, 76(sp)
	sw   t0, 176(sp)

	lw   t0, 172(sp)
	lw   t1, 176(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 180(sp)

	lw   t0, 168(sp)
	lw   t1, 180(sp)
	sw   t0, 0(t1)

	lw   t0, 60(sp)
	sw   t0, 184(sp)

	lw   t0, 184(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 188(sp)

	lw   t0, 188(sp)
	sw   t0, 60(sp)

	j    if_end_1

else_1:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 192(sp)

	lw   t0, 68(sp)
	sw   t0, 196(sp)

	lw   t0, 192(sp)
	lw   t1, 196(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 200(sp)

	lw   t1, 200(sp)
	lw   t0, 0(t1)
	sw   t0, 204(sp)

	la   t0, buf
	li   t1, 1
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 208(sp)

	lw   t0, 76(sp)
	sw   t0, 212(sp)

	lw   t0, 208(sp)
	lw   t1, 212(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 216(sp)

	lw   t0, 204(sp)
	lw   t1, 216(sp)
	sw   t0, 0(t1)

	lw   t0, 68(sp)
	sw   t0, 220(sp)

	lw   t0, 220(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 224(sp)

	lw   t0, 224(sp)
	sw   t0, 68(sp)

	j    if_end_1

while_entry_1:
	lw   t0, 60(sp)
	sw   t0, 228(sp)

	lw   t0, 24(sp)
	sw   t0, 232(sp)

	lw   t0, 228(sp)
	lw   t1, 232(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 236(sp)

	lw   t0, 236(sp)
	bnez t0, while_body_1
	j    while_end_1

if_end_1:
	lw   t0, 76(sp)
	sw   t0, 240(sp)

	lw   t0, 240(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 244(sp)

	lw   t0, 244(sp)
	sw   t0, 76(sp)

	j    while_entry

while_body_1:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 248(sp)

	lw   t0, 60(sp)
	sw   t0, 252(sp)

	lw   t0, 248(sp)
	lw   t1, 252(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 256(sp)

	lw   t1, 256(sp)
	lw   t0, 0(t1)
	sw   t0, 260(sp)

	la   t0, buf
	li   t1, 1
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 264(sp)

	lw   t0, 76(sp)
	sw   t0, 268(sp)

	lw   t0, 264(sp)
	lw   t1, 268(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 272(sp)

	lw   t0, 260(sp)
	lw   t1, 272(sp)
	sw   t0, 0(t1)

	lw   t0, 60(sp)
	sw   t0, 276(sp)

	lw   t0, 276(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 280(sp)

	lw   t0, 280(sp)
	sw   t0, 60(sp)

	lw   t0, 76(sp)
	sw   t0, 284(sp)

	lw   t0, 284(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 288(sp)

	lw   t0, 288(sp)
	sw   t0, 76(sp)

	j    while_entry_1

while_end_1:
	j    while_entry_2

while_entry_2:
	lw   t0, 68(sp)
	sw   t0, 292(sp)

	lw   t0, 4(sp)
	sw   t0, 296(sp)

	lw   t0, 292(sp)
	lw   t1, 296(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 300(sp)

	lw   t0, 300(sp)
	bnez t0, while_body_2
	j    while_end_2

while_body_2:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 304(sp)

	lw   t0, 68(sp)
	sw   t0, 308(sp)

	lw   t0, 304(sp)
	lw   t1, 308(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 312(sp)

	lw   t1, 312(sp)
	lw   t0, 0(t1)
	sw   t0, 316(sp)

	la   t0, buf
	li   t1, 1
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 320(sp)

	lw   t0, 76(sp)
	sw   t0, 324(sp)

	lw   t0, 320(sp)
	lw   t1, 324(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 328(sp)

	lw   t0, 316(sp)
	lw   t1, 328(sp)
	sw   t0, 0(t1)

	lw   t0, 68(sp)
	sw   t0, 332(sp)

	lw   t0, 332(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 336(sp)

	lw   t0, 336(sp)
	sw   t0, 68(sp)

	lw   t0, 76(sp)
	sw   t0, 340(sp)

	lw   t0, 340(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 344(sp)

	lw   t0, 344(sp)
	sw   t0, 76(sp)

	j    while_entry_2

while_end_2:
	j    while_entry_3

while_entry_3:
	lw   t0, 0(sp)
	sw   t0, 348(sp)

	lw   t0, 4(sp)
	sw   t0, 352(sp)

	lw   t0, 348(sp)
	lw   t1, 352(sp)
	slt  t0, t0, t1
	andi t0, t0, 0xff
	sw   t0, 356(sp)

	lw   t0, 356(sp)
	bnez t0, while_body_3
	j    while_end_3

while_body_3:
	la   t0, buf
	li   t1, 1
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 360(sp)

	lw   t0, 0(sp)
	sw   t0, 364(sp)

	lw   t0, 360(sp)
	lw   t1, 364(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 368(sp)

	lw   t1, 368(sp)
	lw   t0, 0(t1)
	sw   t0, 372(sp)

	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 376(sp)

	lw   t0, 0(sp)
	sw   t0, 380(sp)

	lw   t0, 376(sp)
	lw   t1, 380(sp)
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 384(sp)

	lw   t0, 372(sp)
	lw   t1, 384(sp)
	sw   t0, 0(t1)

	lw   t0, 0(sp)
	sw   t0, 388(sp)

	lw   t0, 388(sp)
	li   t1, 1
	add  t0, t0, t1
	sw   t0, 392(sp)

	lw   t0, 392(sp)
	sw   t0, 0(sp)

	j    while_entry_3

while_end_3:
	lw   ra, 396(sp)
	li   t0, 400
	add  sp, sp, t0
	ret


	.text
	.globl main
main:
	li   t0, 36
	add  sp, sp, t0
	sw   ra, 32(sp)
entry_main:
	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 4(sp)

	lw   t0, 4(sp)
	li   t1, 0
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 8(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 12(sp)
	call getarray
	addi sp, sp, 4
	sw   a0, 12(sp)
	lw   a0, -4(sp)

	lw   t0, 12(sp)
	sw   t0, 0(sp)

	lw   t0, 0(sp)
	sw   t0, 16(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	li   a0, 0
	lw   a1, 20(sp)
	call merge_sort
	addi sp, sp, 4
	lw   a0, -4(sp)

	lw   t0, 0(sp)
	sw   t0, 20(sp)

	la   t0, buf
	li   t1, 0
	li   t2, 400
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 24(sp)

	lw   t0, 24(sp)
	li   t1, 0
	li   t2, 4
	mul  t1, t1, t2
	add  t0, t0, t1
	sw   t0, 28(sp)

	addi sp, sp, -4
	sw   a0, 0(sp)
	lw   a0, 24(sp)
	lw   a1, 32(sp)
	call putarray
	addi sp, sp, 4
	lw   a0, -4(sp)

	li   a0, 0
	lw   ra, 32(sp)
	li   t0, 36
	add  sp, sp, t0
	ret

