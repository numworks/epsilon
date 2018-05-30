#ifndef REGS_FIMD_H
#define REGS_FIMD_H

#include "register.h"

class FIMD {
public:
  class VIDCON0 : public Register32 {
  public:
    using Register32::Register32;

    enum class ENVID {
      DisableNow = 0,
      DisableAtEndOfFrame = 2,
      Enable = 3
    };

    REGS_TYPE_FIELD(ENVID, 1, 0);
  };

  class VIDCON1 : public Register32 {
  public:
    using Register32::Register32;

    enum class VSTATUS {
      Vsync = 0,
      BackPorch = 1,
      Active = 2,
      FrontPorch = 3
    };

    REGS_TYPE_FIELD(VSTATUS, 14, 13);
  };

  class VIDTCON0 : public Register32 {
  public:
    using Register32::Register32;
  };

  class VIDTCON1 : public Register32 {
  public:
    using Register32::Register32;
  };

  class VIDTCON2 : public Register32 {
  public:
    using Register32::Register32;
  };

  class WINCON0 : public Register32 {
  public:
    enum class BPPMODE_F {
      PALLETIZED_1BPP = 0,
      PALLETIZED_2BPP = 1,
      PALLETIZED_4BPP = 2,
      PALLETIZED_8BPP = 3,
      RGB_565 = 5,
      RGB_1555 = 7,
      RGB_666 = 8,
      RGB_888 = 11
    };

    REGS_BOOL_FIELD(ENWIN_F, 0);
    REGS_TYPE_FIELD(BPPMODE_F, 5, 2);
  };

  class WINCON1 : public Register32 {
  public:
    enum class BPPMODE_F {
      PALLETIZED_1BPP = 0,
      PALLETIZED_2BPP = 1,
      PALLETIZED_4BPP = 2,
      PALLETIZED_8BPP = 3,
      RGB_565 = 5,
      RGB_1555 = 7,
      RGB_666 = 8,
      RGB_888 = 11
    };

    REGS_BOOL_FIELD(ENWIN_F, 0);
    REGS_TYPE_FIELD(BPPMODE_F, 5, 2);
  };

  class VIDOSD0A : public Register32 {
  public:
    REGS_FIELD(TOPLEFTY, uint16_t, 10, 0);
    REGS_FIELD(TOPLEFTX, uint16_t, 21, 11);
  };

  class VIDOSD1A : public Register32 {
  public:
    REGS_FIELD(TOPLEFTY, uint16_t, 10, 0);
    REGS_FIELD(TOPLEFTX, uint16_t, 21, 11);
  };

  class VIDOSD0B : public Register32 {
  public:
    REGS_FIELD(BOTTOMRIGHTY, uint16_t, 10, 0);
    REGS_FIELD(BOTTOMRIGHTX, uint16_t, 21, 11);
  };

  class VIDOSD1B : public Register32 {
  public:
    REGS_FIELD(BOTTOMRIGHTY, uint16_t, 10, 0);
    REGS_FIELD(BOTTOMRIGHTX, uint16_t, 21, 11);
  };

  class VIDW00ADD0B0 : public Register32 {
  public:
    using Register32::Register32;
  };

  class VIDW00ADD0B1 : public Register32 {
  public:
    using Register32::Register32;
  };

  class VIDW01ADD0 : public Register32 {
  public:
    using Register32::Register32;
  };

  class VIDW01ADD1 : public Register32 {
  public:
    using Register32::Register32;
  };

  REGS_REGISTER_AT(VIDCON0, 0x00);
  REGS_REGISTER_AT(VIDCON1, 0x04);
  REGS_REGISTER_AT(VIDTCON0, 0x08);
  REGS_REGISTER_AT(VIDTCON1, 0x0C);
  REGS_REGISTER_AT(VIDTCON2, 0x10);
  REGS_REGISTER_AT(WINCON0, 0x14);
  REGS_REGISTER_AT(WINCON1, 0x18);
  REGS_REGISTER_AT(VIDOSD0A, 0x28);
  REGS_REGISTER_AT(VIDOSD0B, 0x2C);
  REGS_REGISTER_AT(VIDOSD1A, 0x34);
  REGS_REGISTER_AT(VIDOSD1B, 0x38);
  REGS_REGISTER_AT(VIDW00ADD0B0, 0x64);
  REGS_REGISTER_AT(VIDW01ADD0, 0x6C);
  REGS_REGISTER_AT(VIDW00ADD0B1, 0x7C);
  REGS_REGISTER_AT(VIDW01ADD1, 0x84);

  constexpr FIMD() {}
private:
  constexpr uint32_t Base() const {
    return 0x4C800000;
  };
};

constexpr FIMD FIMD;

#endif
