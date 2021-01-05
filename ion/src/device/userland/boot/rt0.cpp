#include "entry_point.h"
#include <shared/boot/rt0.h>
#include <ion.h>
#include <ion/display.h> // TODO: remove
#include <ion/usb.h> // TODO: remove
#include <ion/events.h> // TODO: remove

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

#if 0
  Ion::USB::enable();
  while (!Ion::USB::isEnumerated()) {
    for (volatile uint32_t i=0; i<10000; i++) {
      __asm volatile("nop");
    }
  }
  Ion::USB::DFU();
  Ion::USB::disable();
#endif

  c = KDColorBlue;
  while (1) {
    int timeout = 300;
    Ion::Events::Event e = Ion::Events::getEvent(&timeout);
    if (e != Ion::Events::None) {
      Ion::Display::pushRectUniform(KDRect(0,0,100,100), c);
      c = c == KDColorBlue ? KDColorGreen : KDColorBlue;
    }
    if (Ion::Battery::isCharging()) {
      Ion::Display::pushRectUniform(KDRect(0,40,100,100), KDColorWhite);
    } else {
      Ion::Display::pushRectUniform(KDRect(0,40,100,100), KDColorBlack);
    }
    /*for (volatile uint32_t i=0; i<10000; i++) {
      __asm volatile("nop");
    }*/
  }
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  ion_main(0, nullptr);
  abort();
}

