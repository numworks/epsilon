#if !DEBUG
#include <ion.h>
#endif
#include <stdlib.h>

void abort() {
#if DEBUG
  while (true) {
  }
#else
  Ion::Reset::core();
#endif
}
