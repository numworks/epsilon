#include <ion.h>
#include <stdlib.h>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

uint32_t Ion::random() {
  /* rand() returns a pseudo-random integral number in the range between 0 and
   * RAND_MAX. We assert that RAND_MAX is at least 8 bit, this way we can merge
   * four rand() calls to create a single 32 bit value. Merging is done using
   * XOR, whose output is as likely to be a 1 as a 0. */
  #if __EMSCRIPTEN__
  return ((int)(emscripten_random() * 255) << 24) ^ ((int)(emscripten_random() * 255) << 16) ^ ((int)(emscripten_random() * 255) << 8) ^ ((int)(emscripten_random() * 255));
  #else
  static_assert(RAND_MAX >= (1<<8), "RAND_MAX is too small");
  return (rand() << 24) ^ (rand() << 16) ^ (rand() << 8) ^ rand();
  #endif
}
