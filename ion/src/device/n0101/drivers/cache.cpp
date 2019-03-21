#include "cache.h"

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

void privateCleanInvalidateDisableDCache(bool clean, bool invalidate, bool disable) {
  CM4.CSSELR()->set(0);
  dsb();

  // Associativity = 6

  uint32_t sets = CM4.CCSIDR()->getNUMSETS();
  uint32_t ways = CM4.CCSIDR()->getASSOCIATIVITY();

  if (disable) {
    CM4.CCR()->setDC(false);
  }

  do {
    uint32_t w = ways;
    do {
      if (clean) {
        if (invalidate) {
          class CM4::DCCISW dccisw;
          dccisw.setSET(sets);
          dccisw.setWAY(w);
          CM4.DCCISW()->set(dccisw);
        } else {
          class CM4::DCCSW dccsw;
          dccsw.setSET(sets);
          dccsw.setWAY(w);
          CM4.DCCSW()->set(dccsw);
        }
      } else if (invalidate) {
        class CM4::DCISW dcisw;
        dcisw.setSET(sets);
        dcisw.setWAY(w);
        CM4.DCISW()->set(dcisw);
      }
    } while (w-- != 0);
  } while (sets-- != 0);

  dsb();
}

void invalidateDCache() {
  privateCleanInvalidateDisableDCache(false, true, false);
}

void cleanDCache() {
  privateCleanInvalidateDisableDCache(true, false, false);
}

void enableDCache() {
  invalidateDCache();
  CM4.CCR()->setDC(true);
  dsb();
  isb();
}

void disableDCache() {
  privateCleanInvalidateDisableDCache(true, true, true);
}

void invalidateICache() {
  dsb();
  isb();
  CM4.ICIALLU()->set(0);
  dsb();
  isb();
}

void enableICache() {
  invalidateICache();
  CM4.CCR()->setIC(true);
  dsb();
  isb();
}

void disableICache() {
  dsb();
  isb();
  CM4.CCR()->setIC(false);
  invalidateICache();
}


}
}
}
