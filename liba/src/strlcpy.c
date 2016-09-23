#include <string.h>

size_t strlcpy(char * dst, const char * src, size_t len) {
  if (len == 0) {
    return 0;
  }
  const char * cur = src;
  const char * end = src+len-1;
  while (*cur != 0 && cur < end) {
    *dst++ = *cur++;
  }
  *dst = 0;
  return cur-src;
}
