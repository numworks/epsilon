#ifndef ION_DEVICE_CACHE_H
#define ION_DEVICE_CACHE_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Cache {

using namespace Regs;

inline void dsb() {
  asm volatile("dsb 0xF":::"memory");
}

inline void isb() {
  asm volatile("isb 0xF":::"memory");
}

void enableDCache() {
  CM4.CSSELR()->set(0);
  dsb();

  //Associativity = 6

  uint32_t sets = CM4.CCSIDR()->getNUMSETS();
  do {
    uint32_t ways = CM4.CCSIDR()->getASSOCIATIVITY();
    do {
      class CM4::DCISW dcisw;
      dcisw.setSET(sets);
      dcisw.setWAY(ways);
      CM4.DCISW()->set(dcisw);
    } while (ways-- != 0);
  } while (sets-- != 0);

  dsb();
  CM4.CCR()->setDC(true);
  dsb();
  isb();
}

void enableICache() {
  dsb();
  isb();
  CM4.ICIALLU()->set(0); // Invalidate I-Cache
  dsb();
  isb();
  CM4.CCR()->setIC(true);
  dsb();
  isb();
}

}
}
}

#endif
