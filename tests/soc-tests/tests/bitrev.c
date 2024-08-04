#include <am.h>
#include <limits.h>
#include <stdint.h>

#define PERIP_SPI_MAS_ADDR 0x10001000l

#define SPI_MAS_REG_RX0  0x00l
#define SPI_MAS_REG_RX1  0x04l
#define SPI_MAS_REG_RX2  0x08l
#define SPI_MAS_REG_RX3  0x0cl
#define SPI_MAS_REG_TX0  0x00l
#define SPI_MAS_REG_TX1  0x04l
#define SPI_MAS_REG_TX2  0x08l
#define SPI_MAS_REG_TX3  0x0cl
#define SPI_MAS_REG_CTRL 0x10l
#define SPI_MAS_REG_DIV  0x14l
#define SPI_MAS_REG_SS   0x18l

typedef union SpiMasCtrl {
  struct {
    uint32_t char_len : 7;
    uint32_t resv0 : 1;
    uint32_t go_bsy : 1;
    uint32_t rx_neg : 1;
    uint32_t tx_neg : 1;
    uint32_t lsb : 1;
    uint32_t ie : 1;
    uint32_t ass : 1;
    uint32_t resv1 : 18;
  };
  uint32_t as_u32;
} SpiMasCtrl_t;

#define inb(addr) (*(volatile uint8_t *)(addr))
#define inw(addr) (*(volatile uint16_t *)(addr))
#define inl(addr) (*(volatile uint32_t *)(addr))
#define outb(addr, data)                                                                           \
  do {                                                                                             \
    *(volatile uint8_t *)(addr) = (data);                                                          \
  } while (0)
#define outw(addr, data)                                                                           \
  do {                                                                                             \
    *(volatile uint16_t *)(addr) = (data);                                                         \
  } while (0)
#define outl(addr, data)                                                                           \
  do {                                                                                             \
    *(volatile uint32_t *)(addr) = (data);                                                         \
  } while (0)

__attribute__((noinline)) static void check(bool cond) {
  if (!cond) {
    halt(1);
  }
}

__attribute__((noinline)) static void spi_init(void) {
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_DIV, 0);
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_SS, 1 << 7);
  // clang-format off
  SpiMasCtrl_t ctrl = {
    .char_len = 16,
    .tx_neg = 0,
    .rx_neg = 1,
    .go_bsy = 0,
    .lsb = 0,
    .ie = 1,
    .ass = 1
  };
  // clang-format on
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL, ctrl.as_u32);
}

__attribute__((noinline)) static uint8_t spi_bitrev(uint8_t x) {
  outw(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_TX0, ((uint16_t)x) << 8);
  SpiMasCtrl_t ctrl = {.as_u32 = inl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL)};
  ctrl.go_bsy = 1;
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL, ctrl.as_u32);
  do {
    ctrl.as_u32 = inl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL);
  } while (ctrl.go_bsy);
  return inw(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_RX0) & 0xff;
}

__attribute__((noinline)) static uint8_t bitrev(uint8_t x) {
  x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
  x = ((x & 0x33) << 2) | ((x & 0xCC) >> 2);
  x = ((x & 0x0F) << 4) | ((x & 0xF0) >> 4);
  return x;
}

int main(void) {
  spi_init();
  for (uint16_t i = 0; i < UINT8_MAX; i++) {
    check(bitrev(i & 0xff) == spi_bitrev(i & 0xff));
    putch('0' + (i % 10));
  }
  putch('\n');
  return 0;
}
