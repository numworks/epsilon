#ifndef REGS_FLASH_H
#define REGS_FLASH_H

#include "register.h"
#include <regs/config/flash.h>

namespace Ion {
namespace Device {
namespace Regs {

class FLASH {
public:
  class ACR : public Register32 {
  public:
    REGS_FIELD(LATENCY, uint8_t, 3, 0);
    REGS_BOOL_FIELD(PRFTEN, 8);
#if REGS_FLASH_CONFIG_ART
    REGS_BOOL_FIELD(ARTEN, 9);
    REGS_BOOL_FIELD(ARTRST, 11);
#else
    REGS_BOOL_FIELD(ICEN, 9);
    REGS_BOOL_FIELD(DCEN, 10);
    REGS_BOOL_FIELD(ICRST, 11);
    REGS_BOOL_FIELD(DCRST, 12);
#endif
  };

  class KEYR : public Register32 {
  };

  class OPTKEYR : public Register32 {
  };

  class CR : public Register32 {
  public:
    enum class PSIZE : uint8_t {
      X8 = 0,
      X16 = 1,
      X32 = 2,
      X64 = 3
    };
    REGS_BOOL_FIELD(PG, 0);
    REGS_BOOL_FIELD(SER, 1);
    REGS_BOOL_FIELD(MER, 2);
    REGS_FIELD(SNB, uint8_t, 6, 3);
    REGS_TYPE_FIELD(PSIZE, 9, 8);
    REGS_BOOL_FIELD(STRT, 16);
    REGS_BOOL_FIELD(EOPIE, 24);
    REGS_BOOL_FIELD(ERRIE, 25);
    REGS_BOOL_FIELD(RDERRIE, 26)
    REGS_BOOL_FIELD(LOCK, 31);
  };

  class SR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(BSY, 16);
    REGS_BOOL_FIELD(ERSERR, 7);
    REGS_BOOL_FIELD(PGPERR, 6);
    REGS_BOOL_FIELD(PGAERR, 5);
    REGS_BOOL_FIELD(WRPERR, 4);
    REGS_BOOL_FIELD(EOP, 0);
  };

  class OPTCR : public Register32 {
  public:
    REGS_BOOL_FIELD(nWRP0, 16);
    REGS_BOOL_FIELD(nWRP1, 17);
    REGS_BOOL_FIELD(nWRP2, 18);
    REGS_BOOL_FIELD(nWRP3, 19);
    REGS_BOOL_FIELD(nWRP4, 20);
    REGS_BOOL_FIELD(nWRP5, 21);
    REGS_BOOL_FIELD(nWRP6, 22);
    REGS_BOOL_FIELD(nWRP7, 23);
    REGS_BOOL_FIELD(LOCK, 0);
  };

  constexpr FLASH() {};
  REGS_REGISTER_AT(ACR, 0x00);
  REGS_REGISTER_AT(KEYR, 0x04);
  REGS_REGISTER_AT(OPTKEYR, 0x08);
  REGS_REGISTER_AT(SR, 0x0C);
  REGS_REGISTER_AT(CR, 0x10);
  REGS_REGISTER_AT(OPTCR, 0x14);
private:
  constexpr uint32_t Base() const {
    return 0x40023C00;
  }
};

constexpr FLASH FLASH;

}
}
}

#endif
