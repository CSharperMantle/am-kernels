#include <am.h>
#include <limits.h>
#include <stdint.h>

#define PERIP_UART16550_ADDR 0x10000000

#define UART16550_REG_TXR 0

int main(void) {
  asm volatile("li a0, 0\r\n"
               "li a1, %0\r\n"
               "li t1, 0x41\r\n" // 0x41 = 'A'
               "la a2, l_again\r\n"
               "li t2, 0x00008067\r\n" // 0x00008067 = ret
               "l_again:\r\n"
               "sb t1, (a1)\r\n"
               "sw t2, (a2)\r\n"
               "fence.i\r\n"
               "j l_again\r\n"
               :
               : "i"(PERIP_UART16550_ADDR + UART16550_REG_TXR)
               : "a0", "a1", "a2", "t1", "t2", "memory");
  return 0;
}