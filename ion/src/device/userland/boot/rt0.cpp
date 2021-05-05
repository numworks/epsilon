#include "isr.h"
#include <shared/boot/rt0.h>
#include <ion.h>
#include <shared/drivers/config/board.h>

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

  ion_main(0, nullptr);
  abort();
}

