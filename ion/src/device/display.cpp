extern "C" {
#include <ion/led.h>
}
#include "regs/regs.h"
#include "regs/fsmc.h"

extern "C" {
  void ion_lcd_init();
}

/* Pinout:
 * PA2  - D4
 * PA3  - D5
 * PA4  - D6
 * PA5  - D7
 * PB14 - D0
 * PC3  - A0
 * PC4  - NE4
 * PC5  - NOE
 * PC6  - D1
 * PC11 - D2
 * PC12 - D3
 * PD2  - NWE
 */

void ion_lcd_gpio_init() {
  // We use GPIOA to GPIOD
  RCC.AHB1ENR()->setGPIOAEN(true);
  RCC.AHB1ENR()->setGPIOBEN(true);
  RCC.AHB1ENR()->setGPIOCEN(true);
  RCC.AHB1ENR()->setGPIODEN(true);

  // Configure GPIOs to use AF

  GPIOA.MODER()->setMODER(2, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(3, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(4, GPIO::MODER::MODE::AlternateFunction);
  GPIOA.MODER()->setMODER(5, GPIO::MODER::MODE::AlternateFunction);

  GPIOB.MODER()->setMODER(14, GPIO::MODER::MODE::AlternateFunction);

  GPIOC.MODER()->setMODER(3, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(4, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(5, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(6, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(11, GPIO::MODER::MODE::AlternateFunction);
  GPIOC.MODER()->setMODER(12, GPIO::MODER::MODE::AlternateFunction);

  GPIOD.MODER()->setMODER(2, GPIO::MODER::MODE::AlternateFunction);

  /* More precisely, we want to use the FSMC alternate function.
   * Oddly enough, this isn't always the same AF number. That equals to:
   * AF12 for PA2,3,4,5
   * AF10 for PB14
   * AF12 for PC3,4,5
   * AF10 for PC6,11,12
   * AF10 for PD2 */
  GPIOA.AFR()->setAFR(2, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(3, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOA.AFR()->setAFR(5, GPIO::AFR::AlternateFunction::AF12);

  GPIOB.AFR()->setAFR(14, GPIO::AFR::AlternateFunction::AF10);

  GPIOC.AFR()->setAFR(3, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(4, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(5, GPIO::AFR::AlternateFunction::AF12);
  GPIOC.AFR()->setAFR(6, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAFR(11, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAFR(12, GPIO::AFR::AlternateFunction::AF10);

  GPIOD.AFR()->setAFR(2, GPIO::AFR::AlternateFunction::AF10);
}

void ion_lcd_fsmc_init() {
  // FSMC lives on the AHB3 bus. Let's enable its clock. */
  RCC.AHB3ENR()->setFSMCEN(true);

#if 0
		/* FSMC timing */
		FSMC_Bank1->BTCR[0+1] = (6) | (10 << 8) | (10 << 16);

		/* Bank1 NOR/SRAM control register configuration */
		FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif


  // Control register
  FSMC.BCR(4)->setMWID(FSMC::BCR::MWID::EIGHT_BITS);
  FSMC.BCR(4)->setWREN(true);
  FSMC.BCR(4)->setMBKEN(true);

  // Timing register
  /*
  FSMC.BTR(1)->setADDSET(6);
  FSMC.BTR(1)->setDATAST(10);
  FSMC.BTR(1)->setBUSTURN(10);
  */
}

void ion_lcd_init() {
  // Turn on the backlight
  RCC.AHB1ENR()->setGPIOCEN(true);
  GPIOC.MODER()->setMODER(9, GPIO::MODER::MODE::Output);
  GPIOC.ODR()->setODR(9, true);

  // Turn on the reset pin
  RCC.AHB1ENR()->setGPIOBEN(true);
  GPIOB.MODER()->setMODER(13, GPIO::MODER::MODE::Output);
  GPIOB.ODR()->setODR(13, true);

  ion_lcd_gpio_init();
  ion_lcd_fsmc_init();


  volatile uint16_t * command = (uint16_t *)0x6C000000;
  volatile uint16_t * data = (uint16_t *)0x6C020000;

  /*
  uint16_t j = 0;
  while (true) {
    *data = j++;
   // *command = j++;
  }
  */

  while (true) {

    for (int i=0; i< 100000; i++) {
    }

    *command = 0x11;
    for (int i=0; i< 100000; i++) {
    }
    *command = 0x29;

    for (int i=0; i< 1000000; i++) {
    }
  }
}
