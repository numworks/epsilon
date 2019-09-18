#include <ion.h>
#include <stdlib.h>
#include <quiz.h>

void quiz_assert(bool condition) {
  if (!condition) {
    abort();
  }
}
