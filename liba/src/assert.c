#include <assert.h>
#include <stdlib.h>

void __attribute__((noinline)) __assert(const char * expression, const char * file, int line) {
  abort();
}
