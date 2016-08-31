#ifndef REGS_SYSCFG_H
#define REGS_SYSCFG_H

#include "register.h"

#include "gpio.h"

class SYSCFG {
public:
  class EXTICR1 : Register32 {
  public:
    void setEXTI(int index, GPIO gpio) volatile { setBitRange(4*index+3, 4*index, (uint32_t)gpio); }
  };

  constexpr SYSCFG() {};
  REGS_REGISTER_AT(EXTICR1, 0x08);
private:
  constexpr uint32_t Base() const {
    return 0x40013800;
  }
};

constexpr SYSCFG SYSCFG;

#endif
