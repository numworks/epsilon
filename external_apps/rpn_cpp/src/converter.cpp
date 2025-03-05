#include "converter.h"
#include <string.h>

int Converter::Parse(const char * text) {
  return 0;
}

void Converter::Serialize(int value, char * serialization, int size) {
  strncpy(serialization, "demo", size);
}
