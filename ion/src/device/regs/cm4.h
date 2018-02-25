#ifndef REGS_CM4_H
#define REGS_CM4_H

#include "register.h"

class CM4 {
public:
  // Vector table offset register
  // http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf
  class VTOR : Register32 {
  public:
    void setVTOR(void *address) volatile { setBitRange(29, 9, (uint32_t)address); }
  };

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

  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihhjgdh.html
  class SCR : public Register32 {
  public:
    REGS_BOOL_FIELD(SLEEPDEEP, 2);
  };

  class SYST_CSR : public Register32 {
  public:
    enum class CLKSOURCE : uint8_t {
      AHB_DIV8 = 0,
      AHB = 1
    };
    REGS_BOOL_FIELD(COUNTFLAG, 16);
    REGS_TYPE_FIELD(CLKSOURCE, 2, 2);
    REGS_BOOL_FIELD(TICKINT, 1);
    REGS_BOOL_FIELD(ENABLE, 0);
  };

  class SYST_RVR : public Register32 {
  public:
    REGS_FIELD(RELOAD, uint32_t, 23, 0);
  };

  class SYST_CVR : public Register32 {
  public:
    REGS_FIELD(CURRENT, uint32_t, 23, 0);
  };

  constexpr CM4() {};
  REGS_REGISTER_AT(SYST_CSR, 0x10);
  REGS_REGISTER_AT(SYST_RVR, 0x14);
  REGS_REGISTER_AT(SYST_CVR, 0x18);
  REGS_REGISTER_AT(VTOR, 0xD08);
  REGS_REGISTER_AT(AIRCR, 0xD0C);
  REGS_REGISTER_AT(SCR, 0xD10);
  REGS_REGISTER_AT(CPACR, 0xD88);
private:
  constexpr uint32_t Base() const {
    return 0xE000E000;
  }
};

constexpr CM4 CM4;

#endif
