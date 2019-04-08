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
    REGS_BOOL_FIELD(LPUDS, 10); // Called LPLVDS in N0100
    REGS_BOOL_FIELD(MRUDS, 11); // Called MRLVDS in N100
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
    enum class Voltage {
      Scale3 = 0x01,
      Scale2 = 0x10,
      Scale1 = 0x11
    };
    REGS_FIELD_W(VOS, Voltage, 15, 14);
    REGS_BOOL_FIELD_W(ODEN, 16);
    REGS_BOOL_FIELD_W(ODSWEN, 17);
    enum class UnderDrive {
      Disable = 0,
      Enable = 3
    };
    REGS_FIELD_W(UDEN, UnderDrive, 19, 18);
#endif
  };

#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  class CSR1 : Register32 {
  public:
    REGS_BOOL_FIELD_R(VOSRDY, 14);
    REGS_BOOL_FIELD_R(ODRDY, 16);
    REGS_BOOL_FIELD_R(ODSWRDY, 17);
  };
#endif

  constexpr PWR() {};
  REGS_REGISTER_AT(CR, 0x00);
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  REGS_REGISTER_AT(CSR1, 0x04);
#endif
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
