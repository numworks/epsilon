#ifndef ION_DEVICE_H
#define ION_DEVICE_H

namespace Ion {
namespace Device {

void init();
void shutdown();

void initFPU();
void coreReset();
void jumpReset();

void initPeripherals();
void shutdownPeripherals();
void initClocks();
void shutdownClocks();

/* The serial number is 96 bits long. That's equal to 16 digits in base 64. We
 * expose a convenient "copySerialNumber" routine which can be called without
 * using a static variable (and therefore without a .bss section). This is used
 * in the RAM'ed DFU bootloader. */
constexpr static int SerialNumberLength = 16;
void copySerialNumber(char * buffer);


/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA0 | Battery sensing   |                       |
 *  PA2 | LCD D4            | Alternate Function 12 | FMSC_D4
 *  PA3 | LCD D5            | Alternate Function 12 | FSMC_D5
 *  PA4 | LCD D6            | Alternate Function 12 | FSMC_D6
 *  PA5 | LCD D7            | Alternate Function 12 | FSMC_D7
 *  PA6 | LED red           | Alternate Function XX | TIM3_CH1
 *  PA7 | LED green         | Alternate Function XX | TIM3_CH2
 *  PA9 | USB VBUS prob     | Input, pulled down    |
 *  PB0 | LED blue          | Alternate Function XX | TIM3_CH3
 *  PB1 | Keyboard row A    | Output, open-drain    |
 *  PB2 | Keyboard row B    | Output, open-drain    |
 *  PB3 | Keyboard row C    | Output, open-drain    |
 *  PB4 | Keyboard row D    | Output, open-drain    |
 *  PB5 | Keyboard row E    | Output, open-drain    |
 *  PB6 | Keyboard row F    | Output, open-drain    |
 *  PB7 | Keyboard row G    | Output, open-drain    |
 *  PB8 | Keyboard row H    | Output, open-drain    |
 *  PB9 | Keyboard row I    | Output, open-drain    |
 * PB10 | Keyboard row J    | Output, open-drain    |
 * PB14 | LCD D0            | Alternate Function 12 | FSMC_D0
 *  PC0 | Keyboard column 1 | Input, pulled-up      |
 *  PC1 | Keyboard column 2 | Input, pulled-up      |
 *  PC3 | LCD data/command  | Alternate Function 12 | FSMC_A0
 *  PC4 | LCD chip select   | Alternate Function 12 | FSMC_NE4
 *  PC5 | LCD read signal   | Alternate Function 12 | FSMC_NOE
 *  PC6 | LCD D1            | Alternate Function 12 | FSMC_D7
 *  PC9 | LCD backlight     | Alternate Function 12 | TIM3_CH4
 * PC11 | LCD D2            | Alternate Function 12 | FSMC_D2
 * PC12 | LCD D3            | Alternate Function 12 | FSMC_D3
 * PC13 | Keyboard column 3 | Input, pulled-up      |
 * PC14 | Keyboard column 4 | Input, pulled-up      |
 * PC15 | Keyboard column 5 | Input, pulled-up      |
 *  PD2 | LCD write signal  | Alternate Function 12 | FSMC_NWE
 */

}
}

#endif
