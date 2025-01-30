#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

.section .bl.f, "ax"
.globl _start
.type _start, @function
_start:
    li a0, 0
    ebreak
