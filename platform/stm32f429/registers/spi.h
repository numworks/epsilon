#ifndef STM32F429_REGISTERS_SPI_H
#define STM32F429_REGISTERS_SPI_H 1

#include <stdint.h>

typedef enum {
  SPI1 = 0,
  SPI2 = 1,
  SPI3 = 2,
  SPI4 = 3,
  SPI5 = 4,
  SPI6 = 5
} SPI_t;

#pragma mark - SPI control registers 1

typedef enum {
  SPI_BR_DIV_2 = 0,
  SPI_BR_DIV_4 = 1,
  SPI_BR_DIV_8 = 2,
  SPI_BR_DIV_16 = 3,
  SPI_BR_DIV_32 = 4,
  SPI_BR_DIV_64 = 5,
  SPI_BR_DIV_128 = 6,
  SPI_BR_DIV_256 = 7
} SPI_BR_t;

typedef enum {
  SPI_DFF_8_BITS = 0,
  SPI_DFF_16_BITS = 1
} SPI_DFF_t;

typedef struct {
  unsigned int CPHA:1;
  unsigned int CPOL:1;
  unsigned int MSTR:1;
  SPI_BR_t BR:3;
  unsigned int SPE:1;
  unsigned int LSBFIRST:1;
  unsigned int SSI:1;
  unsigned int SSM:1;
  unsigned int RXONLY:1;
  SPI_DFF_t DFF:1;
  unsigned int CRCNEXT:1;
  unsigned int CRCEN:1;
  unsigned int BIDIOE:1;
  unsigned int BIDIMODE:1;
} SPI_CR1_t;

SPI_CR1_t * SPI_CR1(SPI_t spi);

#pragma mark - SPI status registers

typedef struct {
  unsigned int RXNE:1;
  unsigned int TXE:1;
  unsigned int CHSIDE:1;
  unsigned int UDR:1;
  unsigned int CRCERR:1;
  unsigned int MODF:1;
  unsigned int OVR:1;
  unsigned int BSY:1;
  unsigned int FRE:1;
  unsigned int :7;
} SPI_SR_t;

volatile SPI_SR_t * SPI_SR(SPI_t spi);

#pragma mark - SPI data registers

typedef uint16_t SPI_DR_t;
volatile SPI_DR_t * SPI_DR(SPI_t spi);

#endif
