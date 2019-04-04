#ifndef ION_DEVICE_N0100_CONFIG_CLOCKS_H
#define ION_DEVICE_N0100_CONFIG_CLOCKS_H

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

//constexpr static int HSE = 25;
constexpr static int PLL_M = 25;
constexpr static int PLL_Q = 4;
//constexpr static int PLL_N = 192;
//constexpr static Regs::RCC::PLLCFGR::PLLP PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP2;
//constexpr static int PLL_P = 2*(int)PLL_P_Reg;
//constexpr static int SYSCLKFrequency = ((HSE/PLL_M)*PLL_N)/PLL_P;
//constexpr static int AHB_prescaler = 1;
//constexpr static int HCLKFrequency = SYSCLKFrequency/AHB_prescaler;
//constexpr static int AHBFrequency = HCLKFrequency;
constexpr static int APB1Prescaler = Regs::RCC::CFGR::APBPrescaler::AHBDividedBy2;

}
}
}
}

#endif

