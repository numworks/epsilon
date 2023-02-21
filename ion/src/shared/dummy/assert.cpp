#include <ion/assert.h>
#include <stdlib.h>

void assertionAbort(const char* expression, const char* file, int line) {
  abort();
}
