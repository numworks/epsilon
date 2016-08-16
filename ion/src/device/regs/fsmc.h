#ifndef REGS_FSMC_H
#define REGS_FSMC_H

#include "register.h"

class FSMC {
public:
  class BCR : Register32 {
  public:
    REGS_BOOL_FIELD(MBKEN, 0);
    REGS_BOOL_FIELD(MUXEN, 1);
  };
  class BTR : Register32 {
  public:
    typedef uint8_t ADDSET;
    REGS_TYPE_FIELD(ADDSET, 3, 0);
    /*
    REGS_TYPE_FIELD(ADDHLD, 7, 4);
    REGS_TYPE_FIELD(DATAST, 15, 8);
    REGS_TYPE_FIELD(BUSTURN, 19, 16);
    REGS_TYPE_FIELD(CLKDIV, 23, 20);
    REGS_TYPE_FIELD(DATLAT, 27, 24);
    REGS_TYPE_FIELD(ACCMOD, 29, 28);
    */
  };

  constexpr FSMC() {}
  constexpr volatile BCR * BCR(int index) {
    return (class BCR *)(Base() + 8*(index-1));
  }
  constexpr volatile BTR * BTR(int index) {
    return (class BTR *)(Base() + 4 + 8*(index-1));
  }
private:
  constexpr uint32_t Base() const {
    return 0xA0000000;
  };
};

constexpr FSMC FSMC;

#endif
