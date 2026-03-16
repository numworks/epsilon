#include <string.h>

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51205
void* memclr(void* b, size_t len) __attribute__((externally_visible));

void* __attribute__((noinline)) memclr(void* b, size_t len) {
  char* destination = (char*)b;
  while (len--) {
    *destination++ = 0;
  }
  return b;
}
