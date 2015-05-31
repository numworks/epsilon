#include <string.h>

size_t strlen(const char * s) {
  const char * str = s;
  while (*str)
    str++;
  return str - s;
}
