#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

.section .bl.f, "ax"
.globl _start
.type _start, @function
_start:
    li t0, 1 << 0
    li t1, 1 << 1
    li t2, 1 << 2
    li a0, 0b00000111
    csrrs zero, mscratch, t0
    csrrs zero, mscratch, t1
    csrrs zero, mscratch, t2
    csrr a1, mscratch
    sub a0, a0, a1
    ebreak
