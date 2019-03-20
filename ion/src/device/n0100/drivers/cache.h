#ifndef ION_DEVICE_CACHE_H
#define ION_DEVICE_CACHE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

inline void dsb() {} // No L1-cache on N0100
inline void invalidateDCache() {}
inline void invalidateICache() {}

}
}
}

#endif
