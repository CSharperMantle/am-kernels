#include <am.h>
#include <limits.h>
#include <stdint.h>

__attribute__((noinline)) static void check(bool cond) {
  if (!cond) {
    halt(1);
  }
}

__attribute__((noinline)) void set_u32(uint32_t *buf, uint32_t val) {
  *buf = val;
}

static volatile uint8_t buf[8] = {0x11, 0xbe, 0xba, 0xfe, 0xca, 0xad, 0xde, 0x22};

int main(void) {
  set_u32((uint32_t *)(buf + 1), 0xdeadbeef);
  check(*(uint32_t *)(buf + 1) == 0xdeadbeef);
  return 0;
}
