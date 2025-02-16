#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

.section .text, "ax"
.globl main
.type main, @function
main:
    li t1, 1
    li a0, 0x2
    la t0, 2f
    csrw mtvec, t0
    ecall
    ebreak
    addi a0, a0, -0x2
    ebreak
2:
    csrr t0, mepc
    addi t0, t0, 4*2
    csrw mepc, t0
    mret
