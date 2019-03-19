#ifndef ION_DEVICE_CACHE_H
#define ION_DEVICE_CACHE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Cache {

inline void dsb() {
  asm volatile("dsb 0xF":::"memory");
}

inline void isb() {
  asm volatile("isb 0xF":::"memory");
}

void enableDCache();

void enableICache();

}
}
}

#endif
