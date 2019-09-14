#ifndef REGS_NVIC_H
#define REGS_NVIC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

// http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf
class NVIC {
public:
  class MaskRegister : Register32 {
  public:
    bool get(int index) { return (bool)getBitRange(index, index); }
    void set(int index, bool state) volatile { setBitRange(index, index, state); }
  };

  class NVIC_ISER0 : public MaskRegister { };
  class NVIC_ISER1 : public MaskRegister { };
  class NVIC_ISER2 : public MaskRegister { };
  class NVIC_ICER0 : public MaskRegister { };
  class NVIC_ICER1 : public MaskRegister { };
  class NVIC_ICER2 : public MaskRegister { };

  constexpr NVIC() {};
  REGS_REGISTER_AT(NVIC_ISER0, 0x00);
  REGS_REGISTER_AT(NVIC_ISER1, 0x04);
  REGS_REGISTER_AT(NVIC_ISER2, 0x08);
  REGS_REGISTER_AT(NVIC_ICER0, 0x80);
  REGS_REGISTER_AT(NVIC_ICER1, 0x84);
  REGS_REGISTER_AT(NVIC_ICER2, 0x88);
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
