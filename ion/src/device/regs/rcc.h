#ifndef REGS_RCC_H
#define REGS_RCC_H

#include "register.h"

class RCC {
public:
  class AHB1ENR : public Register32 {
  public:
    AHB1ENR(uint32_t v) : Register32(v) {}
    REGS_BOOL_FIELD(GPIOAEN, 0);
    REGS_BOOL_FIELD(GPIOBEN, 1);
    REGS_BOOL_FIELD(GPIOCEN, 2);
    REGS_BOOL_FIELD(GPIODEN, 3);
    REGS_BOOL_FIELD(GPIOEEN, 4);
    REGS_BOOL_FIELD(GPIOFEN, 5);
    REGS_BOOL_FIELD(GPIOGEN, 6);
    REGS_BOOL_FIELD(GPIOHEN, 7);
    REGS_BOOL_FIELD(CRCEN, 12);
    REGS_BOOL_FIELD(DMA1EN, 21);
    REGS_BOOL_FIELD(DMA2EN, 22);
  };
  class AHB3ENR : Register32 {
  public:
    REGS_BOOL_FIELD(FSMCEN, 0);
    REGS_BOOL_FIELD(QSPIEN, 0);
  };
  class APB2ENR : Register32 {
  public:
    REGS_BOOL_FIELD(TIM1EN, 0);
  };

  constexpr RCC() {};
  REGS_REGISTER_AT(AHB1ENR, 0x30);
  REGS_REGISTER_AT(AHB3ENR, 0x38);
  REGS_REGISTER_AT(APB2ENR, 0x44);
private:
  constexpr uint32_t Base() const {
    return 0x40023800;
  }
};

constexpr RCC RCC;

#endif
