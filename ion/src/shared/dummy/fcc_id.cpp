#include <ion.h>

void Ion::fccId(char * buffer) {
  strlcpy(buffer, "NA", FccIDLength + 1);
}
