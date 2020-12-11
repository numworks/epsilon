#ifndef REGS_ADC_H
#define REGS_ADC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class ADC {
public:
  class SR : Register32 {
  public:
    REGS_BOOL_FIELD(EOC, 1);
  };

  class CR1 : Register32 {
  public:
  };

  class CR2 : Register32 {
  public:
    REGS_BOOL_FIELD(ADON, 0);
    REGS_BOOL_FIELD(SWSTART, 30);
  };

  class SMPR : Register64 {
    /* The SMPR register doesn't exist per-se in the documentation. It is the
     * consolidation of SMPR1 and SMPR2 which are two 32-bit registers. */
  public:
    enum class SamplingTime {
      Cycles3 = 0,
      Cycles15 = 1,
      Cycles28 = 2,
      Cycles56 = 3,
      Cycles84 = 4,
      Cycles112 = 5,
      Cycles144 = 6,
      Cycles480 = 7
    };
    SamplingTime getSamplingTime(int channel) {
      return (SamplingTime)getBitRange(3*channel+2, 3*channel);
    }
    void setSamplingTime(int channel, SamplingTime t) volatile {
      setBitRange(3*channel+2, 3*channel, (uint64_t)t);
    }
  };

  class SQR1 : Register32 {
  public:
    REGS_FIELD(L, uint8_t, 23, 20);
  };

  class SQR3 : Register32 {
  public:
    REGS_FIELD(SQ1, uint8_t, 4, 0);
  };

  class DR : public Register16 {
  };

  constexpr ADC() {};
  REGS_REGISTER_AT(SR, 0x0);
  REGS_REGISTER_AT(CR2, 0x08);
  REGS_REGISTER_AT(SMPR, 0x0C);
  REGS_REGISTER_AT(SQR1, 0x2C);
  REGS_REGISTER_AT(SQR3, 0x34);
  REGS_REGISTER_AT(DR, 0x4C);
private:
  constexpr uint32_t Base() const {
    return 0x40012000;
  };
};

constexpr ADC ADC;

}
}
}

#endif
