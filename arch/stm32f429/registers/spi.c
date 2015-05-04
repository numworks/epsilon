#include <registers/spi.h>

#define SPI1_BASE 0x40013000
#define SPI2_BASE 0x40003800
#define SPI3_BASE 0x40003C00
#define SPI4_BASE 0x40013400
#define SPI5_BASE 0x40015000
#define SPI6_BASE 0x40015400

#define SPI_CR1_OFFSET 0x00
#define SPI_SR_OFFSET  0x08

char * SPI_REGISTER_ADDRESS(SPI_t spi, int registerOffset) {
  char * spiBaseAddress[6] = {
    (char *)SPI1_BASE, (char *)SPI2_BASE, (char *)SPI3_BASE,
    (char *)SPI4_BASE, (char *)SPI5_BASE, (char *)SPI6_BASE
  };
  return spiBaseAddress[spi] + registerOffset;
}

SPI_CR1_t * SPI_CR1(SPI_t spi) {
  return (SPI_CR1_t *)SPI_REGISTER_ADDRESS(spi, SPI_CR1_OFFSET);
}

SPI_SR_t * SPI_SR(SPI_t spi) {
  return (SPI_SR_t *)SPI_REGISTER_ADDRESS(spi, SPI_SR_OFFSET);
}
