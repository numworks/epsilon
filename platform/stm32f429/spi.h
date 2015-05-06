#ifndef STM32F429_SPI_H
#define STM32F429_SPI_H 1

#include <spi.h>
#include <platform/stm32f429/registers/spi.h>

struct spi_port {
  struct {
    SPI_CR1_t controlRegister1;
    //    SPI_CR2_t controlRegister1;
  } config;
  struct {
    int b;
  } state;
};

#endif
