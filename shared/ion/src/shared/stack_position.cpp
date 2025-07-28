#include <ion.h>

namespace Ion {

// Stack start will be defined in ion_main.
static void* s_stackStart = nullptr;

void* stackStart() {
  assert(s_stackStart != nullptr);
  return s_stackStart;
}

void setStackStart(void* pointer) {
  assert(pointer != nullptr);
  s_stackStart = pointer;
}
}  // namespace Ion