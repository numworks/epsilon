#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_CONFIG_TIMING_H

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

/* FIXME LoopsPerMillisecondLowFrequency must be recalculated, since the "low"
 * frequency is much slower than eight times slower. */
/* FIXME Those values calculated theorically by adapting the values from a
 * 192MHz cpu to a 540MHz cpu, but are in practice 1.9 times to slow. We could
 * avoid this issue altogether by reworking the sleep. */

#warning experimental values designed for Portuguese prototype (with the frequency alternance removed)
constexpr static int LoopsPerMillisecondLowFrequency = 84919;//10*325;//50;//325;//152;
//constexpr static int LoopsPerMillisecondHighFrequency = 21060;
constexpr static int LoopsPerMicrosecond = 86;

}
}
}
}

#endif
