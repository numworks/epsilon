#include <assert.h>
#include <stdlib.h>

void __assert(const char * expression, const char * file, int line) {
  abort();
}
