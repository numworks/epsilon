#include <ion.h>
#include <quiz.h>

void quiz_assert(bool condition) {
  if (!condition) {
    abort();
  }
}
