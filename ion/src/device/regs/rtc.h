#ifndef REGS_RTC_H
#define REGS_RTC_H

#include "register.h"

class RTC {
public:
  class TR : Register32 {
  public:
    REGS_BOOL_FIELD(PM, 22);
    REGS_FIELD(HT, uint8_t, 21, 20);
    REGS_FIELD(HU, uint8_t, 19, 16);
    REGS_FIELD(MNT, uint8_t, 14, 12);
    REGS_FIELD(MNU, uint8_t, 11, 8);
    REGS_FIELD(ST, uint8_t, 6, 4);
    REGS_FIELD(SU, uint8_t, 3, 0);
  };

  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(COE, 23);
    REGS_FIELD(OSEL, uint8_t, 22, 21);
    REGS_BOOL_FIELD(POL, 20);
    REGS_BOOL_FIELD(COSEL, 19);
    REGS_BOOL_FIELD(BKP, 18);
    REGS_BOOL_FIELD_W(SUB1H, 17);
    REGS_BOOL_FIELD_W(ADD1H, 16);
    REGS_BOOL_FIELD(TSIE, 15);
    REGS_BOOL_FIELD(WUTIE, 14);
    REGS_BOOL_FIELD(ALRBIE, 13);
    REGS_BOOL_FIELD(ALRAIE, 12);
    REGS_BOOL_FIELD(TSE, 11);
    REGS_BOOL_FIELD(WUTE, 10);
    REGS_BOOL_FIELD(ALRBE, 9);
    REGS_BOOL_FIELD(ALRAE, 8);
    REGS_BOOL_FIELD(DCE, 7);
    REGS_BOOL_FIELD(FMT, 6);
    REGS_BOOL_FIELD(BYPSHAD, 5);
    REGS_BOOL_FIELD(REFCKON, 4);
    REGS_BOOL_FIELD(TSEDGE, 3);
    REGS_FIELD(WUCKSEL, uint8_t, 2, 0);
  };

  class WPR : Register32 {
  public:
    REGS_FIELD_W(KEY, uint8_t, 7, 0);
  };
    
  constexpr RTC() {}
  REGS_REGISTER_AT(TR, 0x00);
  REGS_REGISTER_AT(CR, 0x08);
  REGS_REGISTER_AT(WPR, 0x24);
private:
  constexpr uint32_t Base() const {
    return 0x40002800;
  };
};

constexpr RTC RTC;

#endif
