#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

constexpr static int LoopsPerMillisecondLowFrequency = 2200;
constexpr static int LoopsPerMillisecondHighFrequency = 2200; // TODO
constexpr static int LoopsPerMicrosecond = 9;

}
}
}
}

#endif

