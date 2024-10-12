#ifndef YSYXSOC
#error This test only works on ysyxSoC.
#endif

#define AREA_BASE 0xc0000000
#define AREA_SIZE 0x100


/* void _start(void)
 * Reg allocation:
 *     a2: loop counter
 *     a3: base
 *     a4: round counter
 *     a5: round size
 *     t0: constant loader/temp
 *     t1: final pointer
 *     t2: temp
 */
.globl _start
.type _start, @function
_start:
    li a3, AREA_BASE

    mv a4, zero
l_check_next_round:
    li a5, 1
    sll a5, a5, a4

    mv a2, zero
l_check_loop:
    la t0, masks
    slli t2, a4, 2
    add t0, t0, t2
    lw t0, (t0)
    and t0, t0, a2
    add t1, a3, a2

    li t2, 1
    bltu a4, t2, l_check_byte
    beq a4, t2, l_check_half

    sw t0, 0(t1)
    lw t2, 0(t1)
    j l_check_cmp

l_check_byte:
    sb t0, 0(t1)
    lb t2, 0(t1)
    j l_check_cmp

l_check_half:
    sh t0, 0(t1)
    lh t2, 0(t1)

l_check_cmp:
    xor t0, t0, t2
    not a0, t0
    jal assert

    add a2, a2, a5
    li t0, AREA_SIZE
    bltu a2, t0, l_check_loop

    addi a4, a4, 1
    li t0, 3
    bltu a4, t0, l_check_next_round

    mv a0, zero
    jal trap


// void assert(int@a0 cond)
.local assert
.type assert, @function
assert:
    beqz a0, l_check_fail
    ret
l_check_fail:
    addi a0, zero, 1
    jal trap


// _Noreturn void trap(int@a0 code)
.local trap
.type trap, @function
trap:
    ebreak
l_halt_inf_loop:
    nop
    j l_halt_inf_loop


.local masks
masks:
    .word 0x000000ff
    .word 0x0000ffff
    .word 0x00ffffff
    .word 0xffffffff
