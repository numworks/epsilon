#include <string.h>

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51205
void * memcpy(void * dst, const void * src, size_t n) __attribute__((externally_visible));

void * __attribute__((noinline)) memcpy(void * dst, const void * src, size_t n) {
  char * destination = (char *)dst;
  char * source = (char *)src;

  while (n--) {
    *destination++ = *source++;
  }

  return dst;
}
