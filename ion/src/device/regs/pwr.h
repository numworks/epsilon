#ifndef REGS_PWR_H
#define REGS_PWR_H

#include "register.h"

class PWR {
public:
  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(LPDS, 0);
    REGS_BOOL_FIELD(PPDS, 1);
    REGS_BOOL_FIELD(FPDS, 9);
  };

  constexpr PWR() {};
  REGS_REGISTER_AT(CR, 0x00);
private:
  constexpr uint32_t Base() const {
    return 0x40007000;
  };
};

constexpr PWR PWR;

#endif
