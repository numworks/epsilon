#include <ctype.h>

int isupper(int c) {
  return (c >= 'A' && c <= 'Z');
}

int isxdigit(int c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int isdigit(int c) {
  return (c >= '0' && c <= '9');
}

int tolower(int c) {
  return isupper(c) ? 'a' + c - 'A' : c;
}
