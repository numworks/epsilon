#ifndef REGS_PWM_H
#define REGS_PWM_H

#include "register.h"

class PWM {
public:
  class TCFG0 : public Register32 {
  public:
    REGS_FIELD(PRESCALER0, uint8_t, 7, 0);
    REGS_FIELD(PRESCALER1, uint8_t, 15, 8);
  };

  class TCFG1 : public Register32 {
  public:
    enum class Mode {
      MUX_1_2 = 0,
      MUX_1_4 = 1,
      MUX_1_8 = 2,
      MUX_1_16 = 3,
      MUX_TCLK = 4
    };

    Mode getMode(int index) { return (Mode)getBitRange(4*index+3, 4*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(4*index+3, 4*index, (uint32_t)mode); }
  };

  class TCON : public Register32 {
  public:
    constexpr static uint32_t ENABLE = 1;
    constexpr static uint32_t MANUAL_UPDATE = 2;
    constexpr static uint32_t INVERTER = 4;
    constexpr static uint32_t AUTORELOAD = 8;

    uint32_t getTimer(int i) volatile                 { return getBitRange(index(i)+4, index(i)); }
    void setTimer(int i, uint32_t flags) volatile     { setBitRange(index(i)+4, index(i), flags); }

  private:
    constexpr static uint32_t index(int i) {
      return i == 0 ? 0 : (i+1)*4;
    }
  };

  class TCNTB0 : public Register16 {
    using Register16::Register16;
  };
  class TCMPB0 : public Register16 {
    using Register16::Register16;
  };
  class TCNTO0 : public Register16 {
    using Register16::Register16;
  };
  
  class TCNTB1 : public Register16 {
    using Register16::Register16;
  };
  class TCMPB1 : public Register16 {
    using Register16::Register16;
  };
  class TCNTO1 : public Register16 {
    using Register16::Register16;
  };

  class TCNTB2 : public Register16 {
    using Register16::Register16;
  };
  class TCMPB2 : public Register16 {
    using Register16::Register16;
  };
  class TCNTO2 : public Register16 {
    using Register16::Register16;
  };

  class TCNTB3 : public Register16 {
    using Register16::Register16;
  };
  class TCMPB3 : public Register16 {
    using Register16::Register16;
  };
  class TCNTO3 : public Register16 {
    using Register16::Register16;
  };

  REGS_REGISTER_AT(TCFG0, 0x00);
  REGS_REGISTER_AT(TCFG1, 0x04);
  REGS_REGISTER_AT(TCON, 0x08);
  REGS_REGISTER_AT(TCNTB0, 0x0C);
  REGS_REGISTER_AT(TCMPB0, 0x10);
  REGS_REGISTER_AT(TCNTO0, 0x14);
  REGS_REGISTER_AT(TCNTB1, 0x18);
  REGS_REGISTER_AT(TCMPB1, 0x1C);
  REGS_REGISTER_AT(TCNTO1, 0x20);
  REGS_REGISTER_AT(TCNTB2, 0x24);
  REGS_REGISTER_AT(TCMPB2, 0x28);
  REGS_REGISTER_AT(TCNTO2, 0x2C);
  REGS_REGISTER_AT(TCNTB3, 0x30);
  REGS_REGISTER_AT(TCMPB3, 0x34);

  constexpr PWM() {}
private:
  constexpr uint32_t Base() const {
    return 0x51000000;
  };
};

constexpr PWM PWM;

#endif
