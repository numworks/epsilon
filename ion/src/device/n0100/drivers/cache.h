#ifndef ION_DEVICE_N0100_CACHE_H
#define ION_DEVICE_N0100_CACHE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

// No L1-cache on N0100

inline void dsb() {}

inline void enable() {}
inline void disable() {}

inline void invalidateDCache() {}
inline void cleanDCache() {}
inline void enableDCache() {}
inline void disableDCache() {}

inline void invalidateICache() {}
inline void enableICache() {}
inline void disableICache() {}

}
}
}

#endif
