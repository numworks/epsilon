#ifndef REGS_CM4_H
#define REGS_CM4_H

#include "register.h"

class CM4 {
public:
  // Coprocessor Access Control Register
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BEHBJHIG.html
  class CPACR : public Register32 {
  public:
    enum class Access {
      Denied = 0,
      PrivilegedOnly = 1,
      Full = 3
    };
    void setAccess(int index, Access a) volatile { setBitRange(2*index+1, 2*index, (uint32_t)a); }
  };


  // Application Interrupt and Reset Control Register
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihehdge.html
  class AIRCR : public Register32 {
  public:
    void requestReset() volatile {
      set(0x5FA<<16 |(1<<2));
    }
  };

  constexpr CM4() {};
  REGS_REGISTER_AT(AIRCR, 0x0C);
  REGS_REGISTER_AT(CPACR, 0x88);
private:
  constexpr uint32_t Base() const {
    return 0xE000ED00;
  }
};

constexpr CM4 CM4;

#endif
