#ifndef REGS_SYSCFG_H
#define REGS_SYSCFG_H

#include "register.h"

#include "gpio.h"

class SYSCFG {
public:
  class MEMRMP : Register32 {
  public:
    enum class MemMode {
      MainFlashmemory = 0,
      SystemFlashmemory = 1,
      EmbeddedSRAM = 3
    };
    REGS_FIELD(MEM_MODE, MemMode, 1, 0);
  };
  class EXTICR1 : Register32 {
  public:
    void setEXTI(int index, GPIO gpio) volatile { setBitRange(4*index+3, 4*index, (uint32_t)gpio); }
  };
  class EXTICR2 : Register32 {
  public:
    void setEXTI(int index, GPIO gpio) volatile { setBitRange(4*(index-4)+3, 4*(index-4), (uint32_t)gpio); }
  };
  class EXTICR3 : Register32 {
  public:
    void setEXTI(int index, GPIO gpio) volatile { setBitRange(4*(index-8)+3, 4*(index-8), (uint32_t)gpio); }
  };
  constexpr SYSCFG() {};
  REGS_REGISTER_AT(MEMRMP, 0x00);
  REGS_REGISTER_AT(EXTICR1, 0x08);
  REGS_REGISTER_AT(EXTICR2, 0x0C);
  REGS_REGISTER_AT(EXTICR3, 0x10);
private:
  constexpr uint32_t Base() const {
    return 0x40013800;
  }
};

constexpr SYSCFG SYSCFG;

#endif
