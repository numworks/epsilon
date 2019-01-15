#ifndef REGS_PWR_H
#define REGS_PWR_H

#include "register.h"

class PWR {
public:
  class CR1 : Register32 {
  public:
    enum class UnderDrive {
      Disable = 0,
      Enable = 3
    };
    REGS_BOOL_FIELD(LPDS, 0);
    REGS_BOOL_FIELD(PPDS, 1);
    REGS_BOOL_FIELD(FPDS, 9);
    REGS_BOOL_FIELD(LPUDS, 10);
    REGS_BOOL_FIELD(MRUDS, 11);
    REGS_FIELD_W(UDEN, UnderDrive, 19, 18);
  };

  constexpr PWR() {};
  REGS_REGISTER_AT(CR1, 0x00);
private:
  constexpr uint32_t Base() const {
    return 0x40007000;
  };
};

constexpr PWR PWR;

#endif
