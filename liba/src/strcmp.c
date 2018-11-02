#include <string.h>

int strcmp(const char *s1, const char *s2) {
  while(*s1 != NULL && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (*(unsigned char *)s1) - (*(unsigned char *)s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0) {
    if (*s1 == NULL || *s2 != *s1) {
      return (*(unsigned char *)s1) - (*(unsigned char *)s2);
    }
    s1++;
    s2++;
  }
  return 0;
}
