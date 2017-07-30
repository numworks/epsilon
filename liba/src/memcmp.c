#include <string.h>

int memcmp(const void * s1, const void * s2, size_t n) {
  char * source1 = (char *)s1;
  char * source2 = (char *)s2;
  while (n--) {
    if (*source1 != *source2) {
      return *source1 - *source2;
    }
    source1++;
    source2++;
  }
  return 0;
}
