#include "converter.h"
#include <string.h>

int Converter::Parse(const char * text) {
  const char * currentChar = text;
  int value = 0;
  bool negative = false;
  if (*currentChar == '-') {
    currentChar++;
    negative = true;
  }
  while (*currentChar != 0) {
    value *= 10;
    value += *currentChar - '0';
    currentChar++;
  }
  return (negative ? -1 : 1) *value;
}

void Converter::Serialize(int value, char * serialization, int size) {
  // TODO: check for size overflow
  char * currentChar = serialization;
  if (value == 0) {
    *currentChar++ = '0';
  }
  if (value < 0) {
    *currentChar++ = '-';
    return Serialize(-value, currentChar, size - 1);
  }
  while (value != 0) {
    *currentChar++ = (value % 10) + '0';
    value /= 10;
  }
  *currentChar = 0;
  // Flip the serialization
  int l = strlen(serialization);
  for (int i = 0; i < l/2; i++) {
    char temp = serialization[i];
    serialization[i] =
      serialization[l - 1 - i];
    serialization[l - 1 - i] = temp;
  }
}
