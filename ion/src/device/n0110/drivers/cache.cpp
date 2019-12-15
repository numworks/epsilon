#include "cache.h"

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

void privateCleanInvalidateDisableDCache(bool clean, bool invalidate, bool disable) {
  // Select Level 1 data cache
  CORTEX.CSSELR()->set(0);
  dsb();

  // Disable D-Cache
  if (disable) {
    CORTEX.CCR()->setDC(false);
    dsb();
  }

  uint32_t sets = CORTEX.CCSIDR()->getNUMSETS();
  uint32_t ways = CORTEX.CCSIDR()->getASSOCIATIVITY();

  do {
    uint32_t w = ways;
    do {
      class CORTEX::DCSW dcsw;
      dcsw.setSET(sets);
      dcsw.setWAY(w);
      volatile CORTEX::DCSW * target = nullptr;
      if (clean && invalidate) {
        target = CORTEX.DCCISW();
      } else if (clean) {
        target = CORTEX.DCCSW();
      } else {
        assert(invalidate);
        target = CORTEX.DCISW();
      }
      target->set(dcsw);
      __asm volatile("nop");
    } while (w-- != 0);
  } while (sets-- != 0);

  dsb();
  isb();
}

void enable() {
  enableICache();
  enableDCache();
}

void disable() {
  disableICache();
  disableDCache();
}

void invalidateDCache() {
  privateCleanInvalidateDisableDCache(false, true, false);
}

void cleanDCache() {
  privateCleanInvalidateDisableDCache(true, false, false);
}

void enableDCache() {
  invalidateDCache();
  CORTEX.CCR()->setDC(true);
  dsb();
  isb();
}

void disableDCache() {
  privateCleanInvalidateDisableDCache(true, true, true);
}

void invalidateICache() {
  dsb();
  isb();
  CORTEX.ICIALLU()->set(0);
  dsb();
  isb();
}

void enableICache() {
  invalidateICache();
  CORTEX.CCR()->setIC(true);
  dsb();
  isb();
}

void disableICache() {
  dsb();
  isb();
  CORTEX.CCR()->setIC(false);
  invalidateICache();
}


}
}
}
