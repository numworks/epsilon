#ifndef ION_DEVICE_N0110_CONFIG_TIMING_H
#define ION_DEVICE_N0110_CONFIG_TIMING_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

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
