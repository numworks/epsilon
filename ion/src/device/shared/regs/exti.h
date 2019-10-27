#ifndef REGS_EXTI_H
#define REGS_EXTI_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class EXTI {
public:
  class MaskRegister : Register32 {
  public:
    bool get(int index) { return (bool)getBitRange(index, index); }
    void set(int index, bool state) volatile { setBitRange(index, index, state); }
  };

  class IMR : public MaskRegister { };
  class EMR : public MaskRegister { };
  class RTSR : public MaskRegister { };
  class FTSR : public MaskRegister { };
  class PR : public MaskRegister { };

  constexpr EXTI() {};
  REGS_REGISTER_AT(IMR, 0x00);
  REGS_REGISTER_AT(EMR, 0x04);
  REGS_REGISTER_AT(RTSR, 0x08);
  REGS_REGISTER_AT(FTSR, 0x0C);
  REGS_REGISTER_AT(PR, 0x14);
private:
  constexpr uint32_t Base() const {
    return 0x40013C00;
  }
};

constexpr EXTI EXTI;

}
}
}

#endif
