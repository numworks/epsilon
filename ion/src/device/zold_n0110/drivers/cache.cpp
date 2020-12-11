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

  // Pick the right DC??SW register according to invalidate/disable parameters
  volatile CORTEX::DCSW * target = nullptr;
  if (clean && invalidate) {
    target = CORTEX.DCCISW();
  } else if (clean) {
    target = CORTEX.DCCSW();
  } else {
    assert(invalidate);
    target = CORTEX.DCISW();
  }

  class CORTEX::CCSIDR ccsidr = CORTEX.CCSIDR()->get();
  uint32_t sets = ccsidr.getNUMSETS();
  uint32_t ways = ccsidr.getASSOCIATIVITY();

  for (int set = sets; set >= 0; set--) {
    for (int way = ways; way >= 0; way--) {
      class CORTEX::DCSW dcsw;
      dcsw.setSET(set);
      dcsw.setWAY(way);
      target->set(dcsw);
    }
  }

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
  CORTEX.CCR()->setDC(true); // Enable D-cache
  dsb();
  isb();
}

void disableDCache() {
  privateCleanInvalidateDisableDCache(true, true, true);
}

void invalidateICache() {
  dsb();
  isb();
  CORTEX.ICIALLU()->set(0); // Invalidate I-cache
  dsb();
  isb();
}

void enableICache() {
  invalidateICache();
  CORTEX.CCR()->setIC(true); // Enable I-cache
  dsb();
  isb();
}

void disableICache() {
  dsb();
  isb();
  CORTEX.CCR()->setIC(false); // Disable I-cache
  CORTEX.ICIALLU()->set(0); // Invalidate I-cache
  dsb();
  isb();
}


}
}
}
