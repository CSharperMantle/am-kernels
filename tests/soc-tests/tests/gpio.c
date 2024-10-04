#include <am.h>
#include <stdint.h>

static volatile void *const GPIO_ADDR = (void *)0x10002000;

int main(void) {
  uint32_t archid;
  asm volatile("csrr %0, marchid" : "=r"(archid));

  for (size_t i = 0; i < 4; i++) {
    uint8_t b = 0;
    b |= (archid % 10) & 0xf;
    archid /= 10;
    b |= ((archid % 10) & 0xf) << 4;
    archid /= 10;
    *(volatile uint8_t *)(GPIO_ADDR + 8 + i) = b;
  }

  uint16_t v = 0x0001u;

  while (1) {
    *(volatile uint16_t *)GPIO_ADDR = v;
    v = v << 1;
    for (volatile uint32_t i = 0; i < 1000; i++) {
      ;
    }
    if (v == 0) {
      break;
    }
  }

  while (1) {
    *(volatile uint16_t *)GPIO_ADDR = *(volatile uint16_t *)(GPIO_ADDR + 4);
    for (volatile uint32_t i = 0; i < 10; i++) {
      ;
    }
    if (*(volatile uint16_t *)(GPIO_ADDR + 4) & 0x8000) {
      break;
    }
  }

  return 0;
}
