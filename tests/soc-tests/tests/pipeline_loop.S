#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

#define MAGIC_VAL 0x100

.section .bl.f, "ax"
.globl _start
.type _start, @function
_start:
    li t1, 1
    la t0, val
    lw t0, 0(t0)
1:
    beqz t0, 2f
    sub t0, t0, t1
    j 1b
2:
    ebreak

.align 4
val:
    .word MAGIC_VAL
