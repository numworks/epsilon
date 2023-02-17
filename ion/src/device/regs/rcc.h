#ifndef REGS_RCC_H
#define REGS_RCC_H

#include "register.h"

class RCC {
public:
  class CR : public Register32 {
  public:
    REGS_BOOL_FIELD(PLLRDY, 25);
    REGS_BOOL_FIELD(PLLON, 24);
  };

  class PLLCFGR : public Register32 {
  public:
    REGS_FIELD(PLLM, uint8_t, 5, 0);
    REGS_FIELD(PLLN, uint16_t, 14, 6);
    REGS_FIELD(PLLP, uint8_t, 17, 16);
    enum class PLLSRC {
      HSI = 0,
      HSE = 1
    };
    void setPLLSRC(PLLSRC s) volatile { setBitRange(22, 22, (uint8_t)s); }
    REGS_FIELD(PLLQ, uint8_t, 27, 24);
    REGS_FIELD(PLLR, uint8_t, 30, 28);
  };

  class CFGR : public Register32 {
  public:
    enum class SW {
      HSI = 0,
      HSE = 1,
      PLL = 2
    };
    void setSW(SW s) volatile { setBitRange(1, 0, (uint8_t)s); }
    SW getSWS() volatile { return (SW)getBitRange(3,2); }
    enum class AHBRatio {
      One = 0,
      DivideBy2 = 4,
      DivideBy4 = 5,
      DivideBy8 = 6,
      DivideBy16 = 7
    };
    void setPPRE1(AHBRatio r) volatile { setBitRange(12, 10, (uint32_t)r); }
  };

  class AHB1ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(GPIOAEN, 0);
    REGS_BOOL_FIELD(GPIOBEN, 1);
    REGS_BOOL_FIELD(GPIOCEN, 2);
    REGS_BOOL_FIELD(GPIODEN, 3);
    REGS_BOOL_FIELD(GPIOEEN, 4);
    REGS_BOOL_FIELD(GPIOFEN, 5);
    REGS_BOOL_FIELD(GPIOGEN, 6);
    REGS_BOOL_FIELD(GPIOHEN, 7);
    REGS_BOOL_FIELD(CRCEN, 12);
    REGS_BOOL_FIELD(DMA1EN, 21);
    REGS_BOOL_FIELD(DMA2EN, 22);
  };

  class AHB2ENR : Register32 {
  public:
    REGS_BOOL_FIELD(RNGEN, 6);
  };

  class AHB3ENR : Register32 {
  public:
    REGS_BOOL_FIELD(FSMCEN, 0);
    REGS_BOOL_FIELD(QSPIEN, 1);
  };

  class APB1ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM3EN, 1);
    REGS_BOOL_FIELD(TIM5EN, 3);
    REGS_BOOL_FIELD(RTCAPB, 10);
    REGS_BOOL_FIELD(SPI3EN, 15);
    REGS_BOOL_FIELD(USART3EN, 18);
    REGS_BOOL_FIELD(PWREN, 28);
  };

  class APB2ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM1EN, 0);
    REGS_BOOL_FIELD(USART1EN, 4);
    REGS_BOOL_FIELD(ADC1EN, 8);
    REGS_BOOL_FIELD(SDIOEN, 11);
    REGS_BOOL_FIELD(SYSCFGEN, 14);
  };

  class BDCR : public Register32 {
  public:
    REGS_BOOL_FIELD(BDRST, 16);
    REGS_BOOL_FIELD(RTCEN, 15);
    REGS_FIELD(RTCSEL, uint8_t, 9, 8);
  };

  class CSR : public Register32 {
  public:
    REGS_BOOL_FIELD(LSION, 0);
    REGS_BOOL_FIELD_R(LSIRDY, 1);
  };

  class DCKCFGR2 : Register32 {
  public:
    REGS_BOOL_FIELD(CK48MSEL, 27);
    REGS_BOOL_FIELD(CKSDIOSEL, 28);
  };

  constexpr RCC() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(PLLCFGR, 0x04);
  REGS_REGISTER_AT(CFGR, 0x08);
  REGS_REGISTER_AT(AHB1ENR, 0x30);
  REGS_REGISTER_AT(AHB2ENR, 0x34);
  REGS_REGISTER_AT(AHB3ENR, 0x38);
  REGS_REGISTER_AT(APB1ENR, 0x40);
  REGS_REGISTER_AT(APB2ENR, 0x44);
  REGS_REGISTER_AT(BDCR, 0x70);
  REGS_REGISTER_AT(CSR, 0x74);
  REGS_REGISTER_AT(DCKCFGR2, 0x94);
private:
  constexpr uint32_t Base() const {
    return 0x40023800;
  }
};

constexpr RCC RCC;

#endif
