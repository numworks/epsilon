#include <ion.h>

void Ion::getSerialNumber(char * buffer) {
  strlcpy(buffer, "000000000000000000000000", Ion::SerialNumberLength+1);
}
