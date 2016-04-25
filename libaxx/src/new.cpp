#include <stdlib.h>

// See the C++ standard, section 3.7.4 for those definitions

void * operator new (unsigned int size) {
  return malloc(size);
}

void operator delete (void * ptr) noexcept {
  free(ptr);
}
