#include <string.h>

size_t strlcat(char * dst, const char * src, size_t dstSize) {
  const size_t srcLen = strlen(src);
  size_t dstLen = strlen(dst);
  if (dstLen >= dstSize) {
    return dstSize+srcLen;
  }
  if (srcLen < dstSize-dstLen) {
    memcpy(dst+dstLen, src, srcLen+1);
  } else {
    memcpy(dst+dstLen, src, dstSize-1);
    dst[dstLen+dstSize-1] = 0;
  }
  return dstLen+srcLen;
}
