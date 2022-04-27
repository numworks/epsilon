#include <stdint.h>

/*
  Here we implement a very little part of the code from the default stm32 libs because we only need the unload function.
  Now we include the license of the original code as required.
*/

/**
 *  THIS CODE COMES FROM THE STM32_HAL LIBRARY (LICENSE ABOVE) AND HAVE BEEN MODIFIED
 *  WE USE ONLY THE HAL_deinit, RCC_deinit and systick_deninit FUNCTIONS AND ONLY COPIED THE CODE NEEDED.
 *  WE NEEDED THIS CODE TO BE ABLE TO BOOT THE STM32 BOOTLOADER
 */

/* 
This software component is provided to you as part of a software package and
applicable license terms are in the  Package_license file. If you received this
software component outside of a package or without applicable license terms,
the terms of the BSD-3-Clause license shall apply. 
You may obtain a copy of the BSD-3-Clause at:
https://opensource.org/licenses/BSD-3-Clause
*/

namespace STM32 {

  typedef struct
  {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t      RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t      RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t      RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    uint32_t      RESERVED3[2];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    volatile uint32_t AHB3LPENR;
    uint32_t      RESERVED4;
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    uint32_t      RESERVED5[2];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    uint32_t      RESERVED6[2];
    volatile uint32_t SSCGR;  
    volatile uint32_t PLLI2SCFGR;
    volatile uint32_t PLLSAICFGR;
    volatile uint32_t DCKCFGR1;
    volatile uint32_t DCKCFGR2;
  } STM32_RCC_TypeDef;

  typedef struct
  {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile const uint32_t CALIB;
  } STM32_SysTick_Type;

  typedef struct
  {
    volatile uint32_t ISER[8U];
          uint32_t RESERVED0[24U];
    volatile uint32_t ICER[8U];
          uint32_t RSERVED1[24U];
    volatile uint32_t ISPR[8U];
          uint32_t RESERVED2[24U];
    volatile uint32_t ICPR[8U];
          uint32_t RESERVED3[24U];
    volatile uint32_t IABR[8U];
          uint32_t RESERVED4[56U];
    volatile uint8_t  IP[240U];
          uint32_t RESERVED5[644U];
    volatile  uint32_t STIR;
  } STM32_NVIC_Type;

  typedef struct {
    volatile const uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint8_t  SHPR[12U];
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
    volatile const uint32_t ID_PFR[2U];
    volatile const uint32_t ID_DFR;
    volatile const uint32_t ID_AFR;
    volatile const uint32_t ID_MFR[4U];
    volatile const uint32_t ID_ISAR[5U];
        uint32_t RESERVED0[1U];
    volatile const uint32_t CLIDR;
    volatile const uint32_t CTR;
    volatile const uint32_t CCSIDR;
    volatile uint32_t CSSELR;
    volatile uint32_t CPACR;
        uint32_t RESERVED3[93U];
    volatile uint32_t STIR;
        uint32_t RESERVED4[15U];
    volatile const uint32_t MVFR0;
    volatile const uint32_t MVFR1;
    volatile const uint32_t MVFR2;
        uint32_t RESERVED5[1U];
    volatile uint32_t ICIALLU;
        uint32_t RESERVED6[1U];
    volatile uint32_t ICIMVAU;
    volatile uint32_t DCIMVAC;
    volatile uint32_t DCISW;
    volatile uint32_t DCCMVAU;
    volatile uint32_t DCCMVAC;
    volatile uint32_t DCCSW;
    volatile uint32_t DCCIMVAC;
    volatile uint32_t DCCISW;
        uint32_t RESERVED7[6U];
    volatile uint32_t ITCMCR;
    volatile uint32_t DTCMCR;
    volatile uint32_t AHBPCR;                 
    volatile uint32_t CACR;
    volatile uint32_t AHBSCR;
        uint32_t RESERVED8[1U];
    volatile uint32_t ABFSR;
  } STM32_SCB_Type;

  #define RCC_BASE 0x40023800UL
  #define SysTick_BASE 0xE000E010UL
  #define NVIC_BASE 0xE000E100UL
  #define SCB_BASE 0xE000ED00UL

  #define SET_BIT(REG, BIT) ((REG) |= (BIT))
  #define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
  #define READ_BIT(REG, BIT) ((REG) & (BIT))
  #define CLEAR_REG(REG) ((REG) = (0x0))
  #define READ_REG(REG) ((REG))

  #define STM_32_RCC ((STM32::STM32_RCC_TypeDef *) RCC_BASE)
  #define STM_32_SysTick ((STM32::STM32_SysTick_Type *) SysTick_BASE)
  #define STM_32_NVIC ((STM32::STM32_NVIC_Type *) NVIC_BASE)
  #define STM_32_SCB ((STM32_SCB_Type *) SCB_BASE)

  extern void rcc_deinit();
  extern void hal_deinit();
  extern void systick_deinit();
}
