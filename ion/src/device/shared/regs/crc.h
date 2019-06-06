#ifndef REGS_CRC_H
#define REGS_CRC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class CRC {
public:
#if REGS_CRC_CONFIG_BYTE_ACCESS
  class DR_ByteAccess : public Register8 {
  };
#endif
  class DR_WordAccess : public Register32 {
  };

  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(RESET, 0);
  };

  constexpr CRC() {};
#if REGS_CRC_CONFIG_BYTE_ACCESS
  REGS_REGISTER_AT(DR_ByteAccess, 0x00);
#endif
  REGS_REGISTER_AT(DR_WordAccess, 0x00);
  REGS_REGISTER_AT(CR, 0x08);
private:
  constexpr uint32_t Base() const {
    return 0x40023000;
  };
};

constexpr CRC CRC;

}
}
}

#endif
