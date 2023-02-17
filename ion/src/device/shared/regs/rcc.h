#ifndef REGS_RCC_H
#define REGS_RCC_H

#include "register.h"
#include <regs/config/rcc.h>

namespace Ion {
namespace Device {
namespace Regs {

class RCC {
public:
  class CR : public Register32 {
  public:
    REGS_BOOL_FIELD(HSION, 0);
    REGS_BOOL_FIELD(HSIRDY, 1);
    REGS_BOOL_FIELD(HSITRIM1, 3);
    REGS_BOOL_FIELD(HSITRIM2, 4);
    REGS_BOOL_FIELD(HSITRIM3, 5);
    REGS_BOOL_FIELD(HSITRIM4, 6);
    REGS_BOOL_FIELD(HSITRIM5, 7);
    REGS_BOOL_FIELD(HSICAL, 8);
    REGS_BOOL_FIELD(HSEON, 16);
    REGS_BOOL_FIELD_R(HSERDY, 17);
    REGS_BOOL_FIELD(PLLON, 24);
    REGS_BOOL_FIELD(PLLRDY, 25);
  };

  class PLLCFGR : public Register32 {
  public:
    REGS_FIELD(PLLM, uint8_t, 5, 0);
    REGS_FIELD(PLLN, uint16_t, 14, 6);
    enum class PLLP {
      PLLP2 = 0b00,
      PLLP4 = 0b01,
      PLLP6 = 0b10,
      PLLP8 = 0b11
    };
    void setPLLP(PLLP s) volatile { setBitRange(17, 16, (uint8_t)s); }
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
    enum class AHBPrescaler {
      SysClk = 0,
      SysClkDividedBy2 = 8,
      SysClkDividedBy4 = 9,
      SysClkDividedBy8 = 10,
      SysClkDividedBy16 = 11,
      SysClkDividedBy64 = 12,
      SysClkDividedBy128 = 13,
      SysClkDividedBy256 = 14,
      SysClkDividedBy512 = 15
    };
    void setHPRE(AHBPrescaler p) volatile { setBitRange(7, 4, (uint32_t)p); }
    enum class APBPrescaler{
      AHB = 0,
      AHBDividedBy2 = 4,
      AHBDividedBy4 = 5,
      AHBDividedBy8 = 6,
      AHBDividedBy16 = 7
    };
    void setPPRE1(APBPrescaler r) volatile { setBitRange(12, 10, (uint32_t)r); }
    void setPPRE2(APBPrescaler r) volatile { setBitRange(15, 13, (uint32_t)r); }
    REGS_FIELD(RTCPRE, uint8_t, 20, 16);
  };

  class AHB3RSTR : Register32 {
  public:
    REGS_BOOL_FIELD(QSPIRST, 1);
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

  class AHB2ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(RNGEN, 6);
    REGS_BOOL_FIELD(OTGFSEN, 7);
  };

  class AHB3ENR : public Register32 {
  public:
    using Register32::Register32;
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
    REGS_BOOL_FIELD(USART6EN, 5);
    REGS_BOOL_FIELD(ADC1EN, 8);
    REGS_BOOL_FIELD(SDIOEN, 11);
    REGS_BOOL_FIELD(SPI1EN, 12);
    REGS_BOOL_FIELD(SYSCFGEN, 14);
  };

  class AHB1LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(GPIOALPEN, 0);
    REGS_BOOL_FIELD(GPIOBLPEN, 1);
    REGS_BOOL_FIELD(GPIOCLPEN, 2);
    REGS_BOOL_FIELD(GPIODLPEN, 3);
    REGS_BOOL_FIELD(GPIOELPEN, 4);
    REGS_BOOL_FIELD(GPIOFLPEN, 5);
    REGS_BOOL_FIELD(GPIOGLPEN, 6);
    REGS_BOOL_FIELD(GPIOHLPEN, 7);
    REGS_BOOL_FIELD(GPIOILPEN, 8);
    REGS_BOOL_FIELD(CRCLPEN, 12);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(AXILPEN, 13);
#endif
    REGS_BOOL_FIELD(FLITFLPEN, 15);
    REGS_BOOL_FIELD(SRAM1LPEN, 16);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(SRAM2LPEN, 17);
    REGS_BOOL_FIELD(BKPSRAMLPEN, 18);
    REGS_BOOL_FIELD(DTCMLPEN, 20);
#endif
    REGS_BOOL_FIELD(DMA1LPEN, 21);
    REGS_BOOL_FIELD(DMA2LPEN, 22);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(OTGHSLPEN, 29);
    REGS_BOOL_FIELD(OTGHSULPILPEN, 30);
#endif
  };

  class AHB2LPENR : public Register32 {
  public:
    using Register32::Register32;
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(AESLPEN, 4);
#endif
    REGS_BOOL_FIELD(RNGLPEN, 6);
    REGS_BOOL_FIELD(OTGFSLPEN, 7);
  };

  class AHB3LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(FMCLPEN, 0);
    REGS_BOOL_FIELD(QSPILPEN, 1);
  };

  class APB1LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM2LPEN, 0);
    REGS_BOOL_FIELD(TIM3LPEN, 1);
    REGS_BOOL_FIELD(TIM4LPEN, 2);
    REGS_BOOL_FIELD(TIM5LPEN, 3);
    REGS_BOOL_FIELD(TIM6LPEN, 4);
    REGS_BOOL_FIELD(TIM7LPEN, 5);
    REGS_BOOL_FIELD(TIM12LPEN, 6);
    REGS_BOOL_FIELD(TIM13LPEN, 7);
    REGS_BOOL_FIELD(TIM14LPEN, 8);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(LPTIM1LPEN, 9);
#endif
    REGS_BOOL_FIELD(RTCAPBLPEN, 10);
    REGS_BOOL_FIELD(WWDGLPEN, 11);
    REGS_BOOL_FIELD(SPI2LPEN, 14);
    REGS_BOOL_FIELD(SPI3LPEN, 15);
    REGS_BOOL_FIELD(USART2LPEN, 17);
    REGS_BOOL_FIELD(USART3LPEN, 18);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(USART4LPEN, 19);
    REGS_BOOL_FIELD(USART5LPEN, 20);
#elif REGS_RCC_CONFIG_F412
    REGS_BOOL_FIELD(I2CFMP1LPEN, 24);
    REGS_BOOL_FIELD(CAN2LPEN, 26);
#endif
    REGS_BOOL_FIELD(I2C1LPEN, 21);
    REGS_BOOL_FIELD(I2C2LPEN, 22);
    REGS_BOOL_FIELD(I2C3LPEN, 23);
    REGS_BOOL_FIELD(CAN1LPEN, 25);
    REGS_BOOL_FIELD(PWRLPEN, 28);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(OTGHSLPEN, 29);
    REGS_BOOL_FIELD(OTGHSULPILPEN, 30);
#endif
  };

  class APB2LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM1LPEN, 0);
    REGS_BOOL_FIELD(TIM8LPEN, 1);
    REGS_BOOL_FIELD(USART1LPEN, 4);
    REGS_BOOL_FIELD(USART6LPEN, 5);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(SDMMC2LPEN, 7);
#endif
    REGS_BOOL_FIELD(ADC1LPEN, 8);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(ADC2LPEN, 9);
    REGS_BOOL_FIELD(ADC3LPEN, 10);
    REGS_BOOL_FIELD(SDMMC1LPEN, 11);
#elif REGS_RCC_CONFIG_F412
    REGS_BOOL_FIELD(SDIOLPEN, 11);
#endif
    REGS_BOOL_FIELD(SPI1LPEN, 12);
    REGS_BOOL_FIELD(SPI4LPEN, 13);
    REGS_BOOL_FIELD(SYSCFGLPEN, 14);
#if REGS_RCC_CONFIG_F412
    REGS_BOOL_FIELD(EXTITEN, 15);
#endif
    REGS_BOOL_FIELD(TIM9LPEN, 16);
    REGS_BOOL_FIELD(TIM10LPEN, 17);
    REGS_BOOL_FIELD(TIM11LPEN, 18);
    REGS_BOOL_FIELD(SPI5LPEN, 20);
#if REGS_RCC_CONFIG_F730
    REGS_BOOL_FIELD(SAI1LPEN, 22);
    REGS_BOOL_FIELD(SAI2LPEN, 23);
#elif REGS_RCC_CONFIG_F412
    REGS_BOOL_FIELD(DFSDM1LPEN, 24);
#endif
  };

  class SSCGR : public Register32 {
  public:
    using Register32::Register32;
    REGS_FIELD(MODPER, uint16_t, 12, 0);
    REGS_FIELD(INCSTEP, uint16_t, 27, 13);
    enum class SPREADSEL {
      CenterSpread = 0,
      DownSpread = 1
    };
    void setSPREADSEL(SPREADSEL s) volatile { setBitRange(30, 30, (uint8_t)s); }
    REGS_BOOL_FIELD(SSCGEN, 31);
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
  REGS_REGISTER_AT(AHB3RSTR, 0x18);
  REGS_REGISTER_AT(AHB1ENR, 0x30);
  REGS_REGISTER_AT(AHB2ENR, 0x34);
  REGS_REGISTER_AT(AHB3ENR, 0x38);
  REGS_REGISTER_AT(APB1ENR, 0x40);
  REGS_REGISTER_AT(APB2ENR, 0x44);
  REGS_REGISTER_AT(AHB1LPENR, 0x50);
  REGS_REGISTER_AT(AHB2LPENR, 0x54);
  REGS_REGISTER_AT(AHB3LPENR, 0x58);
  REGS_REGISTER_AT(APB1LPENR, 0x60);
  REGS_REGISTER_AT(APB2LPENR, 0x64);
  REGS_REGISTER_AT(BDCR, 0x70);
  REGS_REGISTER_AT(CSR, 0x74);
  REGS_REGISTER_AT(SSCGR, 0x80);
  REGS_REGISTER_AT(DCKCFGR2, 0x94);
private:
  constexpr uint32_t Base() const {
    return 0x40023800;
  }
};

constexpr RCC RCC;

}
}
}

#endif
