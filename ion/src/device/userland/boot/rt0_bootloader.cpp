#include "isr.h"
#include <ion.h>
#include <shared/boot/rt0.h>
#include <config/board.h>
#include <shared/drivers/usb.h>

extern "C" {
  void abort();
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  // Initialize slotInfo to be accessible to Kernel
  Ion::Device::USB::slotInfo();
  ion_main(0, nullptr);
  abort();
}

