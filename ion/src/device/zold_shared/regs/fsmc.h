#ifndef REGS_FSMC_H
#define REGS_FSMC_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class FSMC {
public:
  class BCR : Register32 {
  public:
    enum class MTYP : uint8_t {
      SRAM = 0,
      PSRAM = 1,
      NOR = 2
    };
    enum class MWID : uint8_t {
      EIGHT_BITS = 0,
      SIXTEEN_BITS = 1
    };
    REGS_BOOL_FIELD(MBKEN, 0);
    REGS_BOOL_FIELD(MUXEN, 1);
    REGS_TYPE_FIELD(MTYP, 3, 2);
    REGS_TYPE_FIELD(MWID, 5, 4);
    REGS_BOOL_FIELD(WREN, 12);
    REGS_BOOL_FIELD(EXTMOD, 14);
  };

  class BTR : Register32 {
  public:
    enum class ACCMOD : uint8_t {
      A = 0,
      B = 1,
      C = 2,
      D = 3
    };
    REGS_FIELD(ADDSET, uint8_t, 3, 0);
    REGS_FIELD(ADDHLD, uint8_t, 7, 4);
    REGS_FIELD(DATAST, uint8_t, 15, 8);
    REGS_FIELD(BUSTURN, uint8_t, 19, 16);
    REGS_FIELD(CLKDIV, uint8_t, 23, 20);
    REGS_FIELD(DATLAT, uint8_t, 27, 24);
    REGS_TYPE_FIELD(ACCMOD, 29, 28);
  };

  class BWTR : Register32 {
  public:
    enum class ACCMOD : uint8_t {
      A = 0,
      B = 1,
      C = 2,
      D = 3
    };
    REGS_FIELD(ADDSET, uint8_t, 3, 0);
    REGS_FIELD(ADDHLD, uint8_t, 7, 4);
    REGS_FIELD(DATAST, uint8_t, 15, 8);
    REGS_FIELD(BUSTURN, uint8_t, 19, 16);
    REGS_FIELD(CLKDIV, uint8_t, 23, 20);
    REGS_FIELD(DATLAT, uint8_t, 27, 24);
    REGS_TYPE_FIELD(ACCMOD, 29, 28);
  };

  constexpr FSMC() {}
  volatile BCR * BCR(int index) const {
    return (class BCR *)(Base() + 8*(index-1));
  }
  volatile BTR * BTR(int index) const {
    return (class BTR *)(Base() + 4 + 8*(index-1));
  }
  volatile BWTR * BWTR(int index) const {
    return (class BWTR *)(Base() + 0x104 + 8*(index-1));
  }
private:
  constexpr uint32_t Base() const {
    return 0xA0000000;
  };
};

constexpr FSMC FSMC;

}
}
}

#endif
