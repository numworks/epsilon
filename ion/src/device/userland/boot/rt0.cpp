#include "isr.h"
#include <shared/boot/rt0.h>
#include <ion.h>
#include <shared/drivers/config/board.h>
#include <userland/drivers/svcall.h>

extern "C" {
  void abort();
}

void abort() {
#ifdef NDEBUG
  SVC_RETURNING_VOID(SVC_RESET_CORE);
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

