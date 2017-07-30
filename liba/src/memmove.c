#include <string.h>

void * memmove(void * dst, const void * src, size_t n) {
  char * destination = (char *)dst;
  char * source = (char *)src;

  if (source < destination && destination < source + n) {
    /* Copy backwards to avoid overwrites */
    source += n;
    destination += n;
    while (n--) {
      *--destination = *--source;
    }
  } else {
    while (n--) {
      *destination++ = *source++;
    }
  }

  return dst;
}
