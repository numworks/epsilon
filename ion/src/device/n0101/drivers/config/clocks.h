#ifndef ION_DEVICE_N0101_CONFIG_CLOCKS_H
#define ION_DEVICE_N0101_CONFIG_CLOCKS_H

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

constexpr static int HSE = 8;
constexpr static int PLL_M = 8;
constexpr static int PLL_N = 384;
constexpr static Regs::RCC::PLLCFGR::PLLP PLL_P_Reg = Regs::RCC::PLLCFGR::PLLP::PLLP2;
constexpr static int PLL_P = ((int)PLL_P_Reg | 1) << 1;
constexpr static int PLL_Q = 8;
constexpr static int SYSCLKFrequency = ((HSE/PLL_M)*PLL_N)/PLL_P;
constexpr static int AHB_prescaler = 1;
constexpr static int HCLKFrequency = SYSCLKFrequency/AHB_prescaler;
constexpr static int AHBFrequency = HCLKFrequency;
constexpr static int LoopsPerMillisecond = 4811;
constexpr static int LoopsPerMicrosecond = 38;
// CPU clock is 192 MHz, and systick clock source is divided by 8
// To get 1 ms systick overflow we need to reset it to
// 192 000 000 (Hz) / 8 / 1 000 (ms/s)
constexpr static int SysTickPerMillisecond = 24000;

}
}
}
}

#endif
