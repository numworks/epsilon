#include <assert.h>
#include <stdlib.h>
#include <ion/assert.h>

void __attribute__((noinline)) __assert(const char * expression, const char * file, int line) {
  assertionAbort(expression, file, line);
}
