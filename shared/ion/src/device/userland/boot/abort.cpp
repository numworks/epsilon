#if !DEBUG
#include <ion.h>
#endif
#include <stdlib.h>

void __attribute__((noinline)) abort() {
#if DEBUG
  while (true) {
  }
#else
  Ion::Reset::core();
#endif
}
