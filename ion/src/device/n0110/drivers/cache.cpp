#include "cache.h"

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

void privateCleanInvalidateDisableDCache(bool clean, bool invalidate, bool disable) {
  CORTEX.CSSELR()->set(0);
  dsb();

  // Associativity = 6

  uint32_t sets = CORTEX.CCSIDR()->getNUMSETS();
  uint32_t ways = CORTEX.CCSIDR()->getASSOCIATIVITY();

  if (disable) {
    CORTEX.CCR()->setDC(false);
    dsb();
  }

  do {
    uint32_t w = ways;
    do {
      if (clean) {
        if (invalidate) {
          class CORTEX::DCCISW dccisw;
          dccisw.setSET(sets);
          dccisw.setWAY(w);
          CORTEX.DCCISW()->set(dccisw);
        } else {
          class CORTEX::DCCSW dccsw;
          dccsw.setSET(sets);
          dccsw.setWAY(w);
          CORTEX.DCCSW()->set(dccsw);
        }
      } else if (invalidate) {
        class CORTEX::DCISW dcisw;
        dcisw.setSET(sets);
        dcisw.setWAY(w);
        CORTEX.DCISW()->set(dcisw);
      }
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
