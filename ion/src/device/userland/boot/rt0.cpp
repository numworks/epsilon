#include "entry_point.h"
#include <shared/boot/rt0.h>

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
  while (1) {}
}

void __attribute__((noinline)) start() {
  Ion::Device::Init::configureRAM();
  ion_main(0, nullptr);
  abort();
}

