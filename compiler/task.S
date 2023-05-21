  .text
  .globl main
main:
  addi sp, sp, -16
  sw ra, 12(sp)
  call getch
  call putch
  li a0, '\n'
  call putch
  sw a0, 0(sp)
  lw a0, 0(sp)
  lw ra, 12(sp)
  addi sp, sp, 16
  ret
