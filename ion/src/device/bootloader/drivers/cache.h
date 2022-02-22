#ifndef ION_DEVICE_N0110_CACHE_H
#define ION_DEVICE_N0110_CACHE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Cache {

/* Data memory barrier
 * Ensures that all explicit memory accesses that appear in program order before
 * the DMB instruction are observed before any explicit memory accesses that
 * appear in program order after the DMB instruction */
inline void dmb() {
  asm volatile("dmb 0xF":::"memory");
}

/* Data synchronisation barrier
 * Ensures that the processor stalls until the memory write is complete */
inline void dsb() {
  asm volatile("dsb 0xF":::"memory");
}

/* Instructions synchronisation barrier
 * Ensures that the subsequent instructions are loaded in the new context */
inline void isb() {
  asm volatile("isb 0xF":::"memory");
}

void enable();
void disable();

void invalidateDCache();
void cleanDCache();
void enableDCache();
void disableDCache();

void invalidateICache();
void enableICache();
void disableICache();

}
}
}

#endif
