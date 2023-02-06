#include "isr.h"
#include <ion.h>
#include <config/board.h>
#include <shared/boot/rt0.h>
#include <shared/drivers/usb.h>
#include <ion/src/shared/init.h>

extern "C" {
  void abort();
}

void abort() {
#if DEBUG
  while (1) {
  }
#else
  Ion::Reset::core();
#endif
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  Ion::Init();
  // Initialize slotInfo to be accessible to Kernel
  Ion::Device::USB::slotInfo();
  Ion::ExternalApps::deleteApps();
  ion_main(0, nullptr);
  abort();
}
