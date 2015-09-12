/* The LCD panel is connected via the SPI2 port.
 * This file configures the port. */

#include "../registers/registers.h"
#include "spi.h"

void display_spi_init() {
  // Enable the SPI2 clock (SPI2 lives on the APB1 bus)
  RCC_APB1ENR |= SPI2EN;

  // Configure the SPI port
  SPI_CR1(SPI2) = (SPI_BIDIMODE | SPI_BIDIOE | SPI_MSTR | SPI_DFF_8_BITS | SPI_BR(SPI_BR_DIV_2) | SPI_SSM | SPI_SSI | SPI_SPE);
}

void spi_2_write(char * data, size_t size) {
  while (SPI_SR(SPI2) & SPI_BSY) {
  }
  for (size_t i=0; i<size; i++) {
    SPI_DR(SPI2) = data[i];
    while (!(SPI_SR(SPI2) & SPI_TXE)) {
    }
  }
  while (SPI_SR(SPI2) & SPI_BSY) {
  }
}
