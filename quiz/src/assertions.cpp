#include <ion.h>
#include <stdlib.h>
#include <quiz.h>

void quiz_assert(bool condition) {
#if PLATFORM_DEVICE
  /* Freeze on device so that the error message can be read. */
  while (!condition) {}
#else
  if (!condition) {
    abort();
  }
#endif
}
