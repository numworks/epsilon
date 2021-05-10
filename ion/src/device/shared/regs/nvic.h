#ifndef REGS_NVIC_H
#define REGS_NVIC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

// http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf
class NVIC {
public:
  class MaskRegister  {
  public:
    bool getBit(int index) volatile {
      assert(index < static_cast<int>(sizeof(m_values)/sizeof(Register32) * 32));
      int indexMod32 = index % 32;
      return static_cast<bool>(m_values[index / 32].getBitRange(indexMod32, indexMod32));
    }
    void setBit(int index, bool value) volatile {
      assert(index < static_cast<int>(sizeof(m_values)/sizeof(Register32) * 32));
      int indexMod32 = index % 32;
      m_values[index / 32].setBitRange(indexMod32, indexMod32, value);
    }
  private:
    Register32 m_values[8];
  };

  class NVIC_ISER : public MaskRegister {
  public:
    using MaskRegister::MaskRegister;
  };
  class NVIC_ICER : public MaskRegister {
  public:
    using MaskRegister::MaskRegister;
  };

  class NVIC_ICPR : public MaskRegister {
  public:
    using MaskRegister::MaskRegister;
  };

  class NVIC_IABR : public MaskRegister { };

  class NVIC_IPR  {
  public:
    /* STM32 implements only 16 programable priority levels - when Cortex M(4/7)
     * would offer a maximal range of 256. Only the 4 most significant bits are
     * meaningful. */
    enum class InterruptionPriority : uint8_t {
      High = 0b00000000,
      MediumHigh = 0b00010000,
      Medium = 0b00110000,
      MediumLow = 0b01110000,
      Low = 0b11110000
    };
    uint8_t getPriority(int interruptIndex) volatile {
      int indexMod4 = interruptIndex % 4;
      return m_values[interruptIndex / 4].getBitRange( 4 * indexMod4 + 7, 4 * indexMod4);
    }

    void setPriority(int interruptIndex, InterruptionPriority priority) volatile {
      int indexMod4 = interruptIndex % 4;
      m_values[interruptIndex / 4].setBitRange(4 * indexMod4 + 7, 4 * indexMod4, static_cast<uint8_t>(priority));
    }
  private:
    Register32 m_values[60];
  };

  constexpr NVIC() {};
  REGS_REGISTER_AT(NVIC_ISER, 0x00);
  REGS_REGISTER_AT(NVIC_ICER, 0x80);
  REGS_REGISTER_AT(NVIC_ICPR, 0x180);
  REGS_REGISTER_AT(NVIC_IABR, 0x200);
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
