#include <am.h>
#include <limits.h>
#include <stdint.h>

static const uintptr_t FLASH_ADDR = 0x30000000u;

int main(void) {
  ((void (*)(void))(FLASH_ADDR + sizeof(uint32_t)))();
  halt(1);
  return 0;
}
