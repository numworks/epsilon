#include "entry_point.h"
#include <shared/boot/rt0.h>
#include <ion.h>
#include <ion/display.h> // TODO: remove
#include <ion/usb.h> // TODO: remove

extern "C" {
  void abort();
}

void abort() { //TODO EMILIE: expose in ion API
#ifdef NDEBUG
  //svc(SVC_RESET_CORE);
#else
  while (1) {
  }
#endif
}

void ion_main(int argc, const char * const argv[]) {
  KDRect rect(0,0, 199, 100);
  KDColor c = KDColorRed;
  Ion::Display::pushRectUniform(rect, c);
  KDRect r(195,0, 10, 10);
  KDColor pixels[100];
  Ion::Display::pullRect(r, pixels);
  Ion::Display::pushRect(KDRect(0,0,10,10), pixels);

  Ion::USB::enable();
  while (!Ion::USB::isEnumerated()) {
    for (volatile uint32_t i=0; i<10000; i++) {
      __asm volatile("nop");
    }
  }
  Ion::USB::DFU();
  Ion::USB::disable();

  while (1) {}
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  ion_main(0, nullptr);
  abort();
}

