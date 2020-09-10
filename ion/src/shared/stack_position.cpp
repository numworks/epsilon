#include <ion.h>

namespace Ion {

#if PLATFORM_DEVICE
// On device, stack start address is always known. TODO : Factorize address
static void * s_stackStart = reinterpret_cast<void *>(0x20000000 + 256*1024);
#else
// Stack start will be defined in ion_main.
static void * s_stackStart = nullptr;
#endif

void * stackStart() {
  assert(s_stackStart != nullptr);
  return s_stackStart;
}

void setStackStart(void * pointer) {
  assert(pointer != nullptr);
  s_stackStart = pointer;
}
}