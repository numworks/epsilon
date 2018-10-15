#include <string.h>

size_t strlcpy(char * dst, const char * src, size_t len) {
  if (len == 0) {
    return strlen(src);
  }
  const char * cur = src;
  const char * end = src+len-1;
  while (*cur != 0 && cur < end) {
    *dst++ = *cur++;
  }
  *dst = 0;
  while (*cur != 0) {
    cur++;
  }
  return cur-src;
}
