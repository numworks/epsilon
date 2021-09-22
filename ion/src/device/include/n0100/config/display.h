#ifndef ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_DISPLAY_H
#define ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_DISPLAY_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function | Note
 * -----+-------------------+-----------------------+----------|------
 *  PA2 | LCD D4            | Alternate Function 12 | FSMC_D4  |
 *  PA3 | LCD D5            | Alternate Function 12 | FSMC_D5  |
 *  PA4 | LCD D6            | Alternate Function 12 | FSMC_D6  |
 * PB12 | LCD D13           | Alternate Function 12 | FSMC_D13 |
 * PB14 | LCD power         | Output                |          | LCD controller is powered directly from GPIO
 *  PD0 | LCD D2            | Alternate Function 12 | FSMC_D2  |
 *  PD1 | LCD D3            | Alternate Function 12 | FSMC_D3  |
 *  PD4 | LCD read signal   | Alternate Function 12 | FSMC_NOE |
 *  PD5 | LCD write signal  | Alternate Function 12 | FSMC_NWE
 *  PD7 | LCD chip select   | Alternate Function 12 | FSMC_NE1 | Memory bank 1
 *  PD9 | LCD D14           | Alternate Function 12 | FSMC_D14 |
 * PD10 | LCD D15           | Alternate Function 12 | FSMC_D15 |
 * PD11 | LCD data/command  | Alternate Function 12 | FSMC_A16 | Data/Command is address bit 16
 * PD14 | LCD D0            | Alternate Function 12 | FSMC_D0  |
 * PD15 | LCD D1            | Alternate Function 12 | FSMC_D1  |
 *  PE9 | LCD reset         | Output                |          |
 * PE10 | LCD D7            | Alternate Function 12 | FSMC_D7  |
 * PE11 | LCD D8            | Alternate Function 12 | FSMC_D8  |
 * PE12 | LCD D9            | Alternate Function 12 | FSMC_D9  |
 * PE13 | LCD D10           | Alternate Function 12 | FSMC_D10 |
 * PE14 | LCD D11           | Alternate Function 12 | FSMC_D11 |
 * PE15 | LCD D12           | Alternate Function 12 | FSMC_D12 |
 */


namespace Ion {
namespace Device {
namespace Display {
namespace Config {

using namespace Regs;

constexpr GPIO::AFR::AlternateFunction FSMC_AF = GPIO::AFR::AlternateFunction::AF12;
constexpr GPIO::PUPDR::Pull FSMC_Pull = GPIO::PUPDR::Pull::None;
constexpr GPIO::OSPEEDR::OutputSpeed FSMC_Speed = GPIO::OSPEEDR::OutputSpeed::Medium;
constexpr static AFGPIOPin FSMCPins[] = {
  AFGPIOPin(GPIOA, 2, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOA, 3, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOA, 4, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOB, 12, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOB, 12, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 0, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 1, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 4, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 5, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 7, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 9, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 10, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 11, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 14, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 15, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 10, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 11, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 12, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 13, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 14, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 15, FSMC_AF, FSMC_Pull, FSMC_Speed),
};

constexpr static GPIOPin PowerPin = GPIOPin(GPIOB, 14);
constexpr static GPIOPin ResetPin = GPIOPin(GPIOE, 9);
constexpr static GPIOPin ExtendedCommandPin = GPIOPin(GPIOB, 13);
constexpr static GPIOPin TearingEffectPin = GPIOPin(GPIOB, 10);

constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 0;

constexpr static int HCLKFrequencyInMHz = 96;

constexpr static bool DisplayInversion = false;


}
}
}
}

#endif
