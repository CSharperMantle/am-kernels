#include <am.h>
#include <limits.h>
#include <stdint.h>

#define TEST_SIZE 512

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
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_SS, 1);
  // clang-format off
  SpiMasCtrl_t ctrl = {
    .char_len = 64,
    .tx_neg = 0,
    .rx_neg = 0,
    .go_bsy = 0,
    .lsb = 0,
    .ie = 0,
    .ass = 1
  };
  // clang-format on
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL, ctrl.as_u32);
}

__attribute__((noinline)) static uint32_t flash_read(uint32_t addr) {
  union {
    struct {
      uint32_t addr : 24;
      uint32_t cmd : 8;
    };
    uint32_t as_u32;
  } cmd;
  cmd.cmd = 0x03;
  cmd.addr = addr & 0x00ffffff;
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_TX1, cmd.as_u32);
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_TX0, 0);
  SpiMasCtrl_t ctrl = {.as_u32 = inl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL)};
  ctrl.go_bsy = 1;
  outl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL, ctrl.as_u32);
  do {
    ctrl.as_u32 = inl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_CTRL);
  } while (ctrl.go_bsy);
  return inl(PERIP_SPI_MAS_ADDR + SPI_MAS_REG_RX0);
}

static uint32_t byteswap_u32(uint32_t x) {
  return ((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8)
         | ((x & 0xff000000) >> 24);
}

int main(void) {
  spi_init();
  for (size_t i = 0; i < 128; i++) {
    const uint32_t w = byteswap_u32(flash_read(i));
    const uint8_t b = (w & (0xff << ((i % 4) * 8))) >> ((i % 4) * 8);
    check(b == (i & 0xff));
  }
  return 0;
}