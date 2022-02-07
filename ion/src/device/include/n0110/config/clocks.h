#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_CLOCKS_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_CLOCKS_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Clocks {
namespace Config {

/* If you want to considerably slow down the whole machine uniformely, which
 * can be very useful to diagnose performance issues, change the PLL
 * configuration to:
 * PLL_M = 8
 * PLL_N = 192
 * PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP8
 * PLL_Q = 4
 *
 * SYSCLK and HCLK will be set to 24 MHz.
 * Note that even booting takes a few seconds, so don't be surprised
 * if the screen is black for a short while upon booting. */

constexpr int HSE = 8;
constexpr int PLL_M = 8;
constexpr int PLL_N = 384;
constexpr Regs::RCC::PLLCFGR::PLLP PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP2;
constexpr int PLL_P = ((int)PLL_P_Reg | 1) << 1;
constexpr int PLL_Q = 8;
constexpr int SYSCLKFrequency = ((HSE/PLL_M)*PLL_N)/PLL_P;
constexpr int AHBPrescaler = 1;
constexpr int AHBLowFrequencyPrescaler = 8;
constexpr Regs::RCC::CFGR::AHBPrescaler AHBLowFrequencyPrescalerReg = Regs::RCC::CFGR::AHBPrescaler::SysClkDividedBy8;
constexpr int APB1Prescaler = 4;
constexpr Regs::RCC::CFGR::APBPrescaler APB1PrescalerReg = Regs::RCC::CFGR::APBPrescaler::AHBDividedBy4;
constexpr Regs::RCC::CFGR::APBPrescaler APB2PrescalerReg = Regs::RCC::CFGR::APBPrescaler::AHBDividedBy2;

enum class ClockSource : uint8_t {
  PLL,
  PLL_Prescaled, // Prescaled by a factor of 8
};

constexpr ClockSource SuspendClockSource = ClockSource::PLL_Prescaled;

constexpr float HCLK(ClockSource source) {
  return SYSCLKFrequency / (source == ClockSource::PLL ? AHBPrescaler : AHBLowFrequencyPrescaler);
}
static_assert(HCLK(ClockSource::PLL) == 192, "HCLK frequency changed!");

constexpr float Systick(ClockSource source) {
  return HCLK(source);
}

constexpr float APB1(ClockSource source) {
  return HCLK(source) / APB1Prescaler;
}

constexpr float APB1Timer(ClockSource source) {
  return APB1(source) * 2;
}

constexpr float AHB(ClockSource source) {
  return HCLK(source);
}

/* According to AN4850 about Spread Spectrum clock generation
 * MODPER = round[HSE/(4 x fMOD)] with fMOD the target modulation frequency. */
constexpr int fMod = 8; // in KHz. Must be <= 10KHz
constexpr uint32_t SSCG_MODPER = HSE*1000/(4*fMod); // *1000 to put HSE in KHz
/* According to the USB specification 2, "For full-speed only functions, the
 * required data-rate when transmitting (TFDRATE) is 12.000 Mb/s ±0.25%". */
constexpr double modulationDepth = 0.25; // Must be (0.25% <= md <= 2%)
// INCSTEP = round[(2^15 -1)xmdxPLLN)/(100x5xMODPER)
constexpr uint32_t SSCG_INCSTEP = (32767*modulationDepth*PLL_N)/(1.0*100*5*SSCG_MODPER);
static_assert(SSCG_MODPER == 250, "SSCG_MODPER changed");
static_assert(SSCG_INCSTEP == 25, "SSCG_INCSTEP changed");
static_assert(SSCG_INCSTEP * SSCG_MODPER < 32767, "Wrong values for the Spread spectrun clock generator");

}
}
}
}

#endif
