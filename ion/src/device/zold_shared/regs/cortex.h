#ifndef REGS_CORTEX_H
#define REGS_CORTEX_H

#include "register.h"
#include <regs/config/cortex.h>

namespace Ion {
namespace Device {
namespace Regs {

class CORTEX {
public:
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

  // Vector table offset register
  // http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf
  class VTOR : Register32 {
  public:
    void setVTOR(void *address) volatile {
      assert(((uint32_t)address & 0xC00001FF) == 0);
      setBitRange(29, 9, (uint32_t)address >> 9); }
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

  class CCR : public Register32 {
  public:
    REGS_BOOL_FIELD(IC, 17);
    REGS_BOOL_FIELD(DC, 16);
  };

  class SHCRS : public Register32 {
  public:
    REGS_BOOL_FIELD(USGFAULTENA, 18);
    REGS_BOOL_FIELD(BUSFAULTENA, 17);
    REGS_BOOL_FIELD(MEMFAULTENA, 16);
  };

#if REGS_CORTEX_CONFIG_CACHE
  class CCSIDR : public Register32 {
  public:
    using Register32::Register32;
    REGS_FIELD(ASSOCIATIVITY, uint16_t, 12, 3);
    REGS_FIELD(NUMSETS, uint16_t, 27, 13);
  };

  class CSSELR : public Register32 {
  public:
    REGS_BOOL_FIELD(IND, 0);
  };
#endif

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

#if REGS_CORTEX_CONFIG_CACHE
  class ICIALLU : public Register32 {
  public:
    using Register32::Register32;
  };

  class DCSW : public Register32 {
  public:
    DCSW() : Register32(0) {}
    REGS_FIELD(SET, uint16_t, 13, 5);
    REGS_FIELD(WAY, uint8_t, 31, 30);
  };

  class DCISW : public DCSW {
  };

  class DCCSW : public DCSW {
  };

  class DCCISW : public DCSW {
  };
#endif

  constexpr CORTEX() {};
  REGS_REGISTER_AT(SYST_CSR, 0x10);
  REGS_REGISTER_AT(SYST_RVR, 0x14);
  REGS_REGISTER_AT(SYST_CVR, 0x18);
  REGS_REGISTER_AT(VTOR, 0xD08);
  REGS_REGISTER_AT(AIRCR, 0xD0C);
  REGS_REGISTER_AT(SCR, 0xD10);
  REGS_REGISTER_AT(CCR, 0xD14);
  REGS_REGISTER_AT(SHCRS, 0xD24);
#if REGS_CORTEX_CONFIG_CACHE
  REGS_REGISTER_AT(CCSIDR, 0xD80);
  REGS_REGISTER_AT(CSSELR, 0xD84);
#endif
  REGS_REGISTER_AT(CPACR, 0xD88);
#if REGS_CORTEX_CONFIG_CACHE
  REGS_REGISTER_AT(ICIALLU, 0xF50);
  REGS_REGISTER_AT(DCISW, 0xF60);
  REGS_REGISTER_AT(DCCSW, 0xF6C);
  REGS_REGISTER_AT(DCCISW, 0xF74);
#endif
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
