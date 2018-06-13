#ifndef REGS_GPIO_H
#define REGS_GPIO_H

#include "register.h"

class GPIO {
public:
  class GPBCON : public Register32 {
  public:
    enum class Mode {
      Input = 0,
      Output = 1,
      Alternate0 = 2,
      Alternate3 = 3
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class GPBDAT : public Register32 {
    bool getStatus(int index) { return getBitRange(index, index); }
    void setStatus(int index, bool on) volatile { setBitRange(index, index, (uint32_t)on); }
  };

  class GPCCON : public Register32 {
  public:
    enum class Mode {
      Input = 0,
      Output = 1
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class GPCDAT : public Register32 {
  };

  class GPDCON : public Register32 {
  public:
    enum class Mode {
      Input = 0,
      Output = 1,
      Alternate = 2
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class GPDDAT : public Register32 {
  };


  class GPDUDP : public Register32 {
  public:
    enum class Mode {
      Disabled = 0,
      PullDown = 1,
      PullUp = 2
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class GPGCON : public Register32 {
  public:
    enum class Mode {
      Input = 0,
      Output = 1,
      Alternate = 2
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class GPGDAT : public Register32 {
  };

  class GPGUDP : public Register32 {
  public:
    enum class Mode {
      Disabled = 0,
      PullDown = 1,
      PullUp = 2
    };

    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  REGS_REGISTER_AT(GPBCON, 0x10);
  REGS_REGISTER_AT(GPBDAT, 0x14);
  REGS_REGISTER_AT(GPCCON, 0x20);
  REGS_REGISTER_AT(GPCDAT, 0x24);
  REGS_REGISTER_AT(GPDCON, 0x30);
  REGS_REGISTER_AT(GPDDAT, 0x34);
  REGS_REGISTER_AT(GPDUDP, 0x38);
  REGS_REGISTER_AT(GPGCON, 0x60);
  REGS_REGISTER_AT(GPGDAT, 0x64);
  REGS_REGISTER_AT(GPGUDP, 0x68);

  constexpr GPIO() {}
private:
  constexpr uint32_t Base() const {
    return 0x56000000;
  };
};

constexpr GPIO GPIO;

#endif
