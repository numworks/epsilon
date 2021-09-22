#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_DISPLAY_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_DISPLAY_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Display {
namespace Config {

using namespace Regs;

constexpr GPIO::AFR::AlternateFunction FSMC_AF = GPIO::AFR::AlternateFunction::AF12;
constexpr GPIO::PUPDR::Pull FSMC_Pull = GPIO::PUPDR::Pull::None;
constexpr GPIO::OSPEEDR::OutputSpeed FSMC_Speed = GPIO::OSPEEDR::OutputSpeed::Medium;
constexpr static AFGPIOPin FSMCPins[] = {
  AFGPIOPin(GPIOD, 0, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 1, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 4, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 5, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 7, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 8, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 9, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 10, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 11, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 14, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOD, 15, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 7, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 8, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 9, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 10, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 11, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 12, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 13, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 14, FSMC_AF, FSMC_Pull, FSMC_Speed),
  AFGPIOPin(GPIOE, 15, FSMC_AF, FSMC_Pull, FSMC_Speed),
};

constexpr static GPIOPin PowerPin = GPIOPin(GPIOC, 8);
constexpr static GPIOPin ResetPin = GPIOPin(GPIOE, 1);
constexpr static GPIOPin ExtendedCommandPin = GPIOPin(GPIOD, 6);
constexpr static GPIOPin TearingEffectPin = GPIOPin(GPIOB, 11);

constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 0;

constexpr static int HCLKFrequencyInMHz = 192;

constexpr static bool DisplayInversion = true;

}
}
}
}

#endif

