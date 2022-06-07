#include "isr.h"
#include <ion.h>
#include <config/board.h>
#include <shared/drivers/board.h>

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
