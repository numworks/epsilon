#include <strings.h>

void bzero(void * s, size_t n) {
  char *t = s;
  while (n != 0) {
    *t++ = 0;
    n--;
  }
}
