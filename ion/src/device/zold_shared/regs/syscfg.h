#ifndef REGS_SYSCFG_H
#define REGS_SYSCFG_H

#include "register.h"
#include <regs/config/syscfg.h>
#include "gpio.h"

namespace Ion {
namespace Device {
namespace Regs {

class SYSCFG {
public:
#if REGS_SYSCFG_CONFIG_F412
  class MEMRMP : Register32 {
  public:
    enum class MemMode : uint8_t {
      MainFlashmemory = 0,
      SystemFlashmemory = 1,
      EmbeddedSRAM = 3
    };
    REGS_FIELD(MEM_MODE, MemMode, 1, 0);
  };
#endif

  class EXTICR  {
  public:
    uint8_t getEXTI(int index) volatile {
      int indexMod4 = index % 4;
      return m_values[index / 4].getBitRange(4 * indexMod4 + 3, 4 * indexMod4);
    }

    void setEXTI(int index, GPIO gpio) volatile {
      int indexMod4 = index % 4;
      m_values[index / 4].setBitRange(4 * indexMod4 + 3, 4 * indexMod4, static_cast<uint8_t>(gpio));
    }
  private:
    Register32 m_values[4];
  };

  class CMPCR : Register32 {
  public:
    REGS_BOOL_FIELD(CMP_PD, 0);
    REGS_BOOL_FIELD(READY, 8);
  };
  constexpr SYSCFG() {};
#if REGS_SYSCFG_CONFIG_F412
  REGS_REGISTER_AT(MEMRMP, 0x00);
#endif
  REGS_REGISTER_AT(EXTICR, 0x08);
  REGS_REGISTER_AT(CMPCR, 0x20);
private:
  constexpr uint32_t Base() const {
    return 0x40013800;
  }
};

constexpr SYSCFG SYSCFG;

}
}
}

#endif
