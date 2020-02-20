#include <string.h>

size_t strlcpy(char * dst, const char * src, size_t dstSize) {
  const size_t srcLen = strlen(src);
  if (srcLen+1 < dstSize) {
    memcpy(dst, src, srcLen+1);
  } else if (dstSize != 0) {
    memcpy(dst, src, dstSize-1);
    dst[dstSize-1] = 0;
  }
  return srcLen;
}
