#include <ion.h>
#include <stdlib.h>
#include <quiz.h>
#ifndef NDEBUG
#include <ion/console.h>
#endif

void quiz_assert(bool condition) {
#if PLATFORM_DEVICE
  /* Freeze on device so that the error message can be read. */
  while (!condition) {}
#else
  if (!condition) {
#ifdef NDEBUG
    abort();
#else
    Ion::Console::writeLine("Error: Test failed!");
#endif
  }
#endif
}
