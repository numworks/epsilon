#include "stddef.h"

extern "C" {

void * __attribute__((noinline)) memcpy(void * dst, const void * src, size_t n) {
  char * destination = (char *)dst;
  char * source = (char *)src;

  while (n--) {
    *destination++ = *source++;
  }

  return dst;
}

}
