#ifndef REGS_MPU_H
#define REGS_MPU_H

#include "register.h"

class MPU {
public:
  class TYPER : Register32 {
  public:
    REGS_FIELD_R(IREGION, uint8_t, 23, 16);
    REGS_FIELD_R(DREGION, uint8_t, 15, 8);
    REGS_BOOL_FIELD_R(SEPARATE, 0);
  };

  class CTRL : Register32 {
  public:
    REGS_BOOL_FIELD(PRIVDEFENA, 2);
    REGS_BOOL_FIELD(HFNMIENA, 1);
    REGS_BOOL_FIELD(ENABLE, 0);
  };

  class RNR : Register32 {
  public:
    REGS_FIELD(REGION, uint8_t, 7, 0);
  };

  class RBAR : Register32 {
  public:
    void setADDR(void * address) volatile { assert(((uint32_t)address & 0b11111) == 0); setBitRange(31, 5, (uint32_t)address >> 5); }
    REGS_BOOL_FIELD(VALID, 4);
    REGS_FIELD(REGION, uint8_t, 3, 0);
  };

  class RASR : Register32 {
  public:
    REGS_BOOL_FIELD(XN, 28);
    REGS_FIELD(AP, uint8_t, 26, 24);
    REGS_FIELD(TEX, uint8_t, 21, 19);
    REGS_BOOL_FIELD(S, 18);
    REGS_BOOL_FIELD(C, 17);
    REGS_BOOL_FIELD(B, 16);
    REGS_FIELD(SRD, uint8_t, 15, 8);
    enum class RegionSize : uint8_t {
      Bytes32 = 0b00100,
      Bytes64 = 0b00101,
      Bytes128 = 0b00110,
      KyloBytes1 = 0b01001,
      MegaBytes1 = 0b10011,
      GigaBytes1 = 0b11101,
      GigaBytes4 = 0b11111
    };

    REGS_FIELD(SIZE, RegionSize, 5, 1);
    REGS_BOOL_FIELD(ENABLE, 0);
  };

  constexpr MPU() {};
  REGS_REGISTER_AT(TYPER, 0x0);
  REGS_REGISTER_AT(CTRL, 0x04);
  REGS_REGISTER_AT(RNR, 0x08);
  REGS_REGISTER_AT(RBAR, 0x0C);
  REGS_REGISTER_AT(RASR, 0x10);
private:
  constexpr uint32_t Base() const {
    return 0xE000ED90;
  };
};

constexpr MPU MPU;

#endif
