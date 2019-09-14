#ifndef REGS_RNG_H
#define REGS_RNG_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class RNG {
public:
  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(RNGEN, 2);
  };

  class SR : Register32 {
  public:
    REGS_BOOL_FIELD(DRDY, 0);
  };

  class DR : public Register32 {
  };

  constexpr RNG() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(SR, 0x04);
  REGS_REGISTER_AT(DR, 0x08);
private:
  constexpr uint32_t Base() const {
    return 0x50060800;
  }
};

constexpr RNG RNG;

}
}
}

#endif
