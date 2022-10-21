#include "isr.h"
#include <ion.h>
#include <config/board.h>
#include <shared/boot/rt0.h>
#include <shared/drivers/usb.h>

extern "C" {
  void abort();
}

void abort() {
#ifdef NDEBUG
  Ion::Reset::core();
#else
  while (1) {
  }
#endif
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  // Initialize slotInfo to be accessible to Kernel
  Ion::Device::USB::slotInfo();
  Ion::ExternalApps::deleteApps();
  ion_main(0, nullptr);
  abort();
}
