#ifndef REGS_ITM_H
#define REGS_ITM_H

#include "register.h"

// See ARM Cortex M4 TRM
//
namespace Ion {
namespace Device {
namespace Regs {

class ITM {
public:
  class STIM : public Register8 {
  };

  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0314h/Chdbicac.html
  class TER : Register32 {
  public:
    bool get(int index) volatile { return (bool)getBitRange(index, index); }
  };

  constexpr ITM() {};
  volatile STIM * STIM(int i) const {
    return (class STIM *)(Base() + 4*i);
  };
  REGS_REGISTER_AT(TER, 0xE00);
private:
  constexpr uint32_t Base() const {
    return 0xE0000000;
  }
};

constexpr ITM ITM;

}
}
}

#endif
