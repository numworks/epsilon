#include <ion.h>
#include <stdlib.h>
#include <quiz.h>

bool sSkipAssertions = false;

void quiz_assert(bool condition) {
  if(!condition) {
    quiz_print("  ASSERTION FAILED");
    if (sSkipAssertions) {
      return;
    }
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
