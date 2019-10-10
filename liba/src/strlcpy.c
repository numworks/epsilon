#include <string.h>

size_t strlcpy(char * dst, const char * src, size_t dstSize) {
  if (dstSize == 0) {
    return strlen(src);
  }
  const char * cur = src;
  const char * end = src+dstSize-1;
  while (*cur != 0 && cur < end) {
    *dst++ = *cur++;
  }
  *dst = 0;
  while (*cur != 0) {
    cur++;
  }
  return cur-src;
}
