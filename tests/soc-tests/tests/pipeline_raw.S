#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

#define MAGIC_VAL 0x12345678

.section .bl.f, "ax"
.globl _start
.type _start, @function
_start:
    li t1, MAGIC_VAL
    la t0, val
    lw a0, 0(t0)
    sub a0, a0, t1
    ebreak

.align 4
val:
    .word MAGIC_VAL
