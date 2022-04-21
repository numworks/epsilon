#ifndef ION_DEVICE_N0110_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0110_CONFIG_EXTERNAL_FLASH_H

#include <regs/regs.h>

/*  Pin | Role                 | Mode                  | Function
 * -----+----------------------+-----------------------+-----------------
 *  PB2 | QUADSPI CLK          | Alternate Function  9 | QUADSPI_CLK
 *  PB6 | QUADSPI BK1_NCS      | Alternate Function 10 | QUADSPI_BK1_NCS
 *  PE2 | QUADSPI BK1_IO2/WP   | Alternate Function  9 | QUADSPI_BK1_IO2
 *  PC9 | QUADSPI BK1_IO0/SO   | Alternate Function  9 | QUADSPI_BK1_IO0
 * PD12 | QUADSPI BK1_IO1/SI   | Alternate Function  9 | QUADSPI_BK1_IO1
 * PD13 | QUADSPI BK1_IO3/HOLD | Alternate Function  9 | QUADSPI_BK1_IO3
 */

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

using namespace Regs;

constexpr uint32_t StartAddress = 0x90000000;
constexpr uint32_t EndAddress = 0x90800000;
constexpr uint32_t TotalSize = EndAddress - StartAddress;
constexpr uint32_t MaximalSectorSize = 0x10000; // 64kiB

constexpr int NumberOf4KSectors = 8;
constexpr int NumberOf32KSectors = 1;
constexpr int NumberOf64KSectors = 128 - 1;
constexpr int NumberOfSectors = NumberOf4KSectors + NumberOf32KSectors + NumberOf64KSectors;

constexpr uint8_t NumberOfAddressBitsIn64KbyteBlock = 16;
constexpr uint8_t NumberOfAddressBitsIn32KbyteBlock = 15;
constexpr uint8_t NumberOfAddressBitsIn4KbyteBlock = 12;

constexpr AFGPIOPin Pins[] = {
  AFGPIOPin(GPIOB, 2,  GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOB, 6,  GPIO::AFR::AlternateFunction::AF10,  GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOC, 9,  GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOD, 12, GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOD, 13, GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOE, 2,  GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
};

constexpr int ClockFrequencyDivisor = 2; // F(QUADSPI) = F(AHB) / ClockFrequencyDivisor

typedef QUADSPI::CCR::FunctionalMode QSPI_FunctionalMode;
typedef QUADSPI::CCR::OperatingMode QSPI_OperatingMode;

}
}
}
}

#endif
