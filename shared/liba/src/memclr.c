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

void* __aeabi_memclr(void* b, size_t len) { return memclr(b, len); }

void* __aeabi_memclr8(void* b, size_t len) { return memclr(b, len); }

void* __aeabi_memclr4(void* b, size_t len) {
  // TODO false ?
  return memclr(b, len);
}
