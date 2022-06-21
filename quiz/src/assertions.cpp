#include <ion.h>
#include <stdlib.h>
#include <quiz.h>

bool * abort_when_assert_fails() {
  static bool shouldAbort;
  return &shouldAbort;
}

void quiz_assert(bool condition) {
  if (*abort_when_assert_fails() && !condition) {
#if PLATFORM_DEVICE
    /* Freeze on device so that the error message can be read.
     * Note : "while (!condition) {}" could be omitted by the compiler because
     * an iteration statement whose controlling expression is not a constant
     * expression, that performs no input/output operations, does not access
     * volatile objects, and performs no synchronization or atomic operations in
     * its body, controlling expression, or (in the case of a for statement) its
     * expression-3, may be assumed by the implementation to terminate.
    */
    while (true) {}
#else
    abort();
#endif
  }
}
