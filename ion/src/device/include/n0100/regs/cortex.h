#ifndef N0100_REGS_CORTEX_H
#define N0100_REGS_CORTEX_H

#include <shared/regs/register.h>

namespace Ion {
namespace Device {
namespace Regs {

class CORTEX {
public:
  // Application Interrupt and Reset Control Register
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihehdge.html
  class AIRCR : public Register32 {
  public:
    void requestReset() volatile {
      set(0x5FA<<16 |(1<<2));
    }
  };

  constexpr CORTEX() {};
  REGS_REGISTER_AT(AIRCR, 0xD0C);
private:
  constexpr uint32_t Base() const {
    return 0xE000E000;
  }
};

constexpr CORTEX CORTEX;

}
}
}

#endif

