#include <string.h>

char * strchr(const char * s, int c) {
  while (*s != NULL && *s != c) {
    s++;
  }
  if (*s == c) {
    return (char *)s;
  }
  return NULL;
}
