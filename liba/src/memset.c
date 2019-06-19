#include <string.h>

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51205
void * memset(void * b, int c, size_t len) __attribute__((externally_visible));

void * __attribute__((noinline)) memset(void * b, int c, size_t len) {
  char * destination = (char *)b;
  while (len--) {
    *destination++ = (unsigned char)c;
  }
  return b;
}
