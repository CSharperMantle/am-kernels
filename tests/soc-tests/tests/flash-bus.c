#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#define TEST_SIZE 512

__attribute__((noinline)) void check(bool cond) {
  if (!cond) {
    halt(1);
  }
}

static const uint8_t *const FLASH_ADDR = (const uint8_t *)0x30000000;

int main(void) {
  for (size_t i = 0; i < TEST_SIZE; i++) {
    check(FLASH_ADDR[i] == (i & 0xff));
  }
  return 0;
}
