#include <string.h>

char * strcpy(char * dst, const char * src) {
  char * cur = dst;
  while (*src != 0) {
    *cur++ = *src++;
  }
  *cur = 0;
  return dst;
}
