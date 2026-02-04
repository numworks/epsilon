#if !DEBUG
#include <ion.h>
#endif
#include <omg/unreachable.h>
#include <stdlib.h>

void __attribute__((noinline)) abort() {
#if DEBUG
  while (true) {
  }
#else
  Ion::Reset::core();
  OMG_UNREACHABLE;
#endif
}
