#ifndef REGS_SDIO_H
#define REGS_SDIO_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class SDIO {
public:
  class POWER : Register32 {
  public:
    enum class PWRCTRL : uint8_t {
      Off = 0,
      On = 3
    };
    REGS_TYPE_FIELD(PWRCTRL, 1, 0);
  };

  class CLKCR : Register32 {
  public:
    enum class WIDBUS : uint8_t {
      Default = 0,
      FourBits = 1,
      EightBits = 2
    };
    REGS_FIELD(CLKDIV, uint8_t, 7, 0);
    REGS_BOOL_FIELD(CLKEN, 8);
    REGS_BOOL_FIELD(PWRSAV, 9);
    REGS_BOOL_FIELD(BYPASS, 10);
    REGS_TYPE_FIELD(WIDBUS, 12, 11);
    REGS_BOOL_FIELD(NEGEDGE, 13);
    REGS_BOOL_FIELD(HWFC_EN, 14);
  };

  class ARG : public Register32 {
  };

  class CMD : public Register32 {
  public:
    using Register32::Register32;
    enum class WAITRESP : uint8_t {
      None = 0,
      Short = 1,
      Long = 3
    };
    REGS_FIELD(CMDINDEX, uint8_t, 5, 0);
    REGS_TYPE_FIELD(WAITRESP, 7, 6);
    REGS_BOOL_FIELD(WAITINT, 8);
    REGS_BOOL_FIELD(WAITPEND, 9);
    REGS_BOOL_FIELD(CPSMEN, 10);
  };

  class RESP : public Register32 {
  };

  class STA : Register32 {
  public:
    REGS_BOOL_FIELD_R(CCRCFAIL, 0);
    REGS_BOOL_FIELD_R(DCRCFAIL, 1);
    REGS_BOOL_FIELD_R(CTIMEOUT, 2);
    REGS_BOOL_FIELD_R(DTIMEOUT, 3);
    REGS_BOOL_FIELD_R(TXUNDERR, 4);
    REGS_BOOL_FIELD_R(RXOVERR, 5);
    REGS_BOOL_FIELD_R(CMDREND, 6);
    REGS_BOOL_FIELD_R(CMDSENT, 7);
    REGS_BOOL_FIELD_R(DATAEND, 8);
    REGS_BOOL_FIELD_R(DBCKEND, 10);
    REGS_BOOL_FIELD_R(CMDACT, 11);
    REGS_BOOL_FIELD_R(TXACT, 12);
    REGS_BOOL_FIELD_R(RXACT, 13);
    REGS_BOOL_FIELD_R(TXFIFOHE, 14);
    REGS_BOOL_FIELD_R(RXFIFOHF, 15);
    REGS_BOOL_FIELD_R(TXFIFOF, 16);
    REGS_BOOL_FIELD_R(RXFIFOF, 17);
    REGS_BOOL_FIELD_R(TXFIFOE, 18);
    REGS_BOOL_FIELD_R(RXFIFOE, 19);
    REGS_BOOL_FIELD_R(TXDAVL, 20);
    REGS_BOOL_FIELD_R(RXDAVL, 21);
    REGS_BOOL_FIELD_R(SDIOIT, 22);
  };

  class ICR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD_W(CCRCFAILC, 0);
    REGS_BOOL_FIELD_W(DCRCFAILC, 1);
    REGS_BOOL_FIELD_W(CTIMEOUTC, 2);
    REGS_BOOL_FIELD_W(DTIMEOUTC, 3);
    REGS_BOOL_FIELD_W(TXUNDERRC, 4);
    REGS_BOOL_FIELD_W(RXOVERRC, 5);
    REGS_BOOL_FIELD_W(CMDRENDC, 6);
    REGS_BOOL_FIELD_W(CMDSENTC, 7);
    REGS_BOOL_FIELD_W(DATAENDC, 8);
    REGS_BOOL_FIELD_W(DBCKENDC, 10);
    REGS_BOOL_FIELD_W(SDIOITC, 22);
  };

  constexpr SDIO() {};
  REGS_REGISTER_AT(POWER, 0x00);
  REGS_REGISTER_AT(CLKCR, 0x04);
  REGS_REGISTER_AT(ARG, 0x08);
  REGS_REGISTER_AT(CMD, 0x0C);
  volatile RESP * RESP(int i ) const { return (class RESP *)(Base() + 0x10+4*i); };
  REGS_REGISTER_AT(STA, 0x34);
  REGS_REGISTER_AT(ICR, 0x38);
private:
  constexpr uint32_t Base() const {
    return 0x40012C00;
  }
};

constexpr SDIO SDIO;

}
}
}

#endif
