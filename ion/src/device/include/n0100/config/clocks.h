#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_CLOCKS_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_CLOCKS_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Clocks {
namespace Config {

/* If you want to considerably slow down the whole machine uniformely, which
 * can be very useful to diagnose performance issues, change the PLL
 * configuration to:
 * PLL_M = 25
 * PLL_N = 144
 * PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP6
 * PLL_Q = 3
 *
 * SYSCLK and HCLK will be set to 24 MHz.
 * Note that even booting takes a few seconds, so don't be surprised
 * if the screen is black for a short while upon booting. */

constexpr static int HSE = 25;
constexpr static int PLL_M = 25;
constexpr static int PLL_Q = 4;
constexpr static int PLL_N = 192;
constexpr static Regs::RCC::PLLCFGR::PLLP PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP2;
constexpr static int PLL_P = ((int)PLL_P_Reg | 1) << 1;
constexpr static int SYSCLKFrequency = ((HSE/PLL_M)*PLL_N)/PLL_P;
constexpr static int AHBPrescaler = 1;
/* To slow down the whole system, we prescale the AHB clock.
 * We could divide the system clock by 512. However, the HCLK clock
 * frequency must be >= 14.2MHz and <=216 MHz which forces the
 * AHBPrescaler to be below 96MHz/14.2MHz~6.7. */
constexpr static Regs::RCC::CFGR::AHBPrescaler AHBLowFrequencyPrescalerReg = Regs::RCC::CFGR::AHBPrescaler::SysClkDividedBy4;
constexpr static int AHBLowFrequencyPrescaler = 4;
constexpr static Regs::RCC::CFGR::APBPrescaler APB1PrescalerRegs = Regs::RCC::CFGR::APBPrescaler::AHBDividedBy2;
constexpr static int APB1Prescaler = 2;

enum class ClockSource : uint8_t {
  PLL,
  PLL_Prescaled, // Prescaled by a factor of 8
};

static constexpr ClockSource SuspendClockSource = ClockSource::PLL_Prescaled;

static constexpr float HCLK(ClockSource source) {
  return SYSCLKFrequency / (source == ClockSource::PLL ? AHBPrescaler : AHBLowFrequencyPrescaler);
}
static_assert(HCLK(ClockSource::PLL) == 96, "HCLK frequency changed!");

static constexpr float Systick(ClockSource source) {
  return HCLK(source);
}

static constexpr float APB1(ClockSource source) {
  return HCLK(source) / APB1Prescaler;
}

static constexpr float APB1Timer(ClockSource source) {
  return APB1(source) * 2;
}

static constexpr float AHB(ClockSource source) {
  return HCLK(source);
}

}
}
}
}

#endif


