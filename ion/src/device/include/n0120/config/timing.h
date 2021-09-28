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
constexpr static int LoopsPerMillisecondLowFrequency = 325;
constexpr static int LoopsPerMillisecondHighFrequency = 21060;
constexpr static int LoopsPerMicrosecond = 22;

}
}
}
}

#endif
