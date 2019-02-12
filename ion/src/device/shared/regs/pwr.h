#ifndef REGS_PWR_H
#define REGS_PWR_H

#include "register.h"
#include <regs/config/pwr.h>

namespace Ion {
namespace Device {
namespace Regs {

class PWR {
public:
  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(LPDS, 0);
    REGS_BOOL_FIELD(PPDS, 1);
    REGS_BOOL_FIELD(FPDS, 9);
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
    REGS_BOOL_FIELD(LPUDS, 10);
    REGS_BOOL_FIELD(MRUDS, 11);
    enum class UnderDrive {
      Disable = 0,
      Enable = 3
    };
    REGS_FIELD_W(UDEN, UnderDrive, 19, 18);
#endif
  };

  constexpr PWR() {};
  REGS_REGISTER_AT(CR, 0x00);
private:
  constexpr uint32_t Base() const {
    return 0x40007000;
  };
};

constexpr PWR PWR;

}
}
}

#endif
