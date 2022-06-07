#include "isr.h"
#include <ion.h>
#include <shared/boot/rt0.h>
#include <shared/drivers/usb.h>
#include <shared/drivers/display.h>
#include <shared/drivers/display_image.h>

extern "C" {
  void abort();
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();

  // Display NumWorks Logo
  Ion::Device::DisplayImage::logo();
  Ion::Display::waitForVBlank();
  Ion::Timing::msleep(4000);

  // Initialize slotInfo to be accessible to Kernel
  Ion::Device::USB::slotInfo();

  ion_main(0, nullptr);
  abort();
}

