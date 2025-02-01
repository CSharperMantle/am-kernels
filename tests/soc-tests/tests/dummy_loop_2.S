#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

.section .bl.f, "ax"
.globl _start
.type _start, @function
_start:
    li a1, -50
    li t2, 5
    la t0, vals
    la t1, vals_end
    j 2f
1:
    addi t0, t0, 4
    beq t0, t1, 3f
2:
    lw a2, 0(t0)
    beq a2, t2, 1b
    add a0, a0, a2
    j 1b
3:
    add a0, a0, a1
    ebreak

.align 4
vals:
    .word 1
    .word 2
    .word 3
    .word 4
    .word 5
    .word 6
    .word 7
    .word 8
    .word 9
    .word 10
    .word 0
vals_end:
