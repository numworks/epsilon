#include "stm32_drivers.h"

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

void STM32::rcc_deinit() {
  SET_BIT(STM_32_RCC->CR, (0x1UL << (0U)));
  while (READ_BIT(STM_32_RCC->CR, (0x1UL << (1U))) == 0) {}
  SET_BIT(STM_32_RCC->CR, (0x10UL << (3U)));
  CLEAR_REG(STM_32_RCC->CFGR);
  while (READ_BIT(STM_32_RCC->CFGR, (0x3UL << (2U))) != 0) {}
  CLEAR_BIT(STM_32_RCC->CR, (0x1UL << (16U)) | (0x1UL << (18U)) | (0x1UL << (19U)));
  while (READ_BIT(STM_32_RCC->CR, (0x1UL << (17U))) != 0) {}
  CLEAR_BIT(STM_32_RCC->CR, (0x1UL << (24U)));
  while (READ_BIT(STM_32_RCC->CR, (0x1UL << (25U))) != 0) {}
  CLEAR_BIT(STM_32_RCC->CR, (0x1UL << (26U)));
  while (READ_BIT(STM_32_RCC->CR, (0x1UL << (27U))) != 0) {}
  CLEAR_BIT(STM_32_RCC->CR, (0x1UL << (28U)));
  while (READ_BIT(STM_32_RCC->CR, (0x1UL << (29U))) != 0) {}
  STM_32_RCC->PLLCFGR = ((0x10UL << (0x0U)) | (0x040UL << (6U)) | (0x080UL << (6U)) | (0x4UL << (24U)) | 0x20000000U);
  STM_32_RCC->PLLI2SCFGR = ((0x040UL << (6U)) | (0x080UL << (6U)) | (0x4UL << (24U)) | (0x2UL << (28U)));
  STM_32_RCC->PLLSAICFGR = ((0x040UL << (6U)) | (0x080UL << (6U)) | (0x4UL << (24U)) | 0x20000000U);
  CLEAR_BIT(STM_32_RCC->CIR, ((0x1UL << (8U)) | (0x1UL << (9U)) | (0x1UL << (10U)) | (0x1UL << (11U)) | (0x1UL << (12U)) | (0x1UL << (13U)) | (0x1UL << (14U))));
  SET_BIT(STM_32_RCC->CIR, ((0x1UL << (16U)) | (0x1UL << (17U)) | (0x1UL << (18U)) | (0x1UL << (19U)) | (0x1UL << (20U)) | (0x1UL << (21U)) | (0x1UL << (22U)) | (0x1UL << (23U))));
  CLEAR_BIT(STM_32_RCC->CSR, ((0x1UL << (0U))));
  SET_BIT(STM_32_RCC->CSR, ((0x1UL << (24U))));
  uint32_t sysclock = ((uint32_t)16000000U);
  uint32_t a = ((sysclock / 1000U));
  uint32_t b = 15U;
  STM_32_SysTick->LOAD = (uint32_t)(a - 1UL);
  STM_32_SCB->SHPR[(((uint32_t)(-1))&0xFUL)-4UL] = (uint8_t)((((1UL << 4U)-1UL) <<  (8U - 4UL)) & (uint32_t)0xFFUL);
  STM_32_SysTick->VAL = 0U;
  STM_32_SysTick->CTRL = (1UL << 2U) | (1UL << 1U) | (1UL);
  uint32_t c = ((uint32_t)((STM_32_SCB->AIRCR & (7UL << 8U)) >> 8U));
  uint32_t d = (c & (uint32_t)0x07UL);
  uint32_t e;
  uint32_t f;
  e = ((7UL - d) > (uint32_t)(4UL)) ? (uint32_t)(4UL) : (7UL - d);
  f = ((d + (uint32_t)(4UL)) < (uint32_t)(7UL)) ? (uint32_t)(0UL) : (uint32_t)((d - 7UL) + (uint32_t)(4UL));
  uint32_t g = (((b & (uint32_t)((1UL << (e)) - 1UL)) << f) | ((0UL & (uint32_t)((1UL << (f)) - 1UL))));
  STM_32_SCB->SHPR[(((uint32_t)(-1))&0xFUL)-4UL] = (uint8_t)((g << (8U - 4UL)) & (uint32_t)0xFFUL);
}

void STM32::hal_deinit() {
  STM_32_RCC->APB1RSTR = 0xFFFFFFFFU;
  STM_32_RCC->APB1RSTR = 0x00U;
  STM_32_RCC->APB2RSTR = 0xFFFFFFFFU;
  STM_32_RCC->APB2RSTR = 0x00U;
  STM_32_RCC->AHB1RSTR = 0xFFFFFFFFU;
  STM_32_RCC->AHB1RSTR = 0x00U;
  STM_32_RCC->AHB2RSTR = 0xFFFFFFFFU;
  STM_32_RCC->AHB2RSTR = 0x00U;
  STM_32_RCC->AHB3RSTR = 0xFFFFFFFFU;
  STM_32_RCC->AHB3RSTR = 0x00U;
}

void STM32::systick_deinit() {
  STM_32_SysTick->CTRL = STM_32_SysTick->LOAD = STM_32_SysTick->VAL = 0;
}
