#include <string.h>

void * memset(void * b, int c, size_t len) {
  char * destination = (char *)b;
  while (len--) {
    *destination++ = (unsigned char)c;
  }
  return b;
}
