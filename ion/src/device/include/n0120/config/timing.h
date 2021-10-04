#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

#warning experimental values
#if NDEBUG
constexpr static int LoopsPerMillisecond_PLL_Prescaled512 = 146;
#else
constexpr static int LoopsPerMillisecond_PLL_Prescaled512 = 35;
#endif
constexpr static int LoopsPerMicrosecond = 86;

}
}
}
}

#endif
