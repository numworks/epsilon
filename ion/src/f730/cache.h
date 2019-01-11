#ifndef ION_DEVICE_CACHE_H
#define ION_DEVICE_CACHE_H

namespace Ion {
namespace Device {
namespace Cache {

void dsb() {
  asm volatile("dsb 0xF":::"memory");
}

void isb() {
  asm volatile("isb 0xF":::"memory");
}

void enableDCache() {
  dsb();
  isb();
  // FIXME: Invalidate D-Cache
  CM4.CCR()->setDC(true);
  dsb();
  isb();
}

void enableICache() {
  dsb();
  isb();
  CM4.ICIALLU->set(0); // Invalidate I-Cache
  CM4.CCR()->setIC(true);
  dsb();
  isb();
}

}
}
}

#endif
