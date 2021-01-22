#ifndef REGS_NVIC_H
#define REGS_NVIC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

// http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf
class NVIC {
public:
  class MaskRegister : public Register32 {
  public:
    using Register32::Register32;
    bool getBit(int index) { return (bool)getBitRange(index, index); }
    void setBit(int index, bool state) volatile { setBitRange(index, index, state); }
  };

  class NVIC_ISER0 : public MaskRegister {
  public:
    using MaskRegister::MaskRegister;
  };
  class NVIC_ISER1 : public MaskRegister { };
  class NVIC_ISER2 : public MaskRegister { };
  class NVIC_ICER0 : public MaskRegister {
  public:
    using MaskRegister::MaskRegister;
  };
  class NVIC_ICER1 : public MaskRegister { };
  class NVIC_ICER2 : public MaskRegister { };

  class NVIC_ICPR0 : public MaskRegister { };
  class NVIC_ICPR1 : public MaskRegister { };
  class NVIC_ICPR2 : public MaskRegister { };

  class NVIC_IPR  {
  public:
    /* STM32 implements only 16 programable priority levels - when Cortex M(4/7)
     * would offer a maximal range of 256. */
    uint8_t getPriority(int interruptIndex) volatile {
      int indexMod4 = interruptIndex % 4;
      return m_values[interruptIndex / 4].getBitRange( 4 * indexMod4 + 7, 4 * indexMod4 + 4);
    }

    void setPriority(int interruptIndex, uint8_t priority) volatile {
      int indexMod4 = interruptIndex % 4;
      m_values[interruptIndex / 4].setBitRange(4 * indexMod4 + 7, 4 * indexMod4 + 3, priority);
    }
  private:
    Register32 m_values[60];
  };

  constexpr NVIC() {};
  REGS_REGISTER_AT(NVIC_ISER0, 0x00);
  REGS_REGISTER_AT(NVIC_ISER1, 0x04);
  REGS_REGISTER_AT(NVIC_ISER2, 0x08);
  REGS_REGISTER_AT(NVIC_ICER0, 0x80);
  REGS_REGISTER_AT(NVIC_ICER1, 0x84);
  REGS_REGISTER_AT(NVIC_ICER2, 0x88);
  REGS_REGISTER_AT(NVIC_ICPR0, 0x180);
  REGS_REGISTER_AT(NVIC_ICPR1, 0x184);
  REGS_REGISTER_AT(NVIC_ICPR2, 0x188);
  REGS_REGISTER_AT(NVIC_IPR, 0x300);
private:
  constexpr uint32_t Base() const {
    return 0xE000E100;
  }
};

constexpr NVIC NVIC;

}
}
}

#endif
