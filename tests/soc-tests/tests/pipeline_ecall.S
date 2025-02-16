#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

.section .text, "ax"
.globl main
.type main, @function
main:
    la t0, 1f
    csrw mtvec, t0
    nop
    nop
    nop
    la t0, 3f
    csrw mtvec, t0
    ecall
1:
    li a0, 0x2
    ebreak
2:
    li a0, 0x1
    ebreak
    j 2b
3:
    li a0, 0
    ebreak
    j 3b
