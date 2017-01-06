#ifndef REGS_CRC_H
#define REGS_CRC_H

#include "register.h"

class CRC {
public:
  class DR : public Register32 {
  };

  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(RESET, 0);
  };

  constexpr CRC() {};
  REGS_REGISTER_AT(DR, 0x00);
  REGS_REGISTER_AT(CR, 0x08);
private:
  constexpr uint32_t Base() const {
    return 0x40023000;
  };
};

constexpr CRC CRC;

#endif
