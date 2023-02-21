#include "random.h"

#include <stdlib.h>
#include <time.h>

void Ion::Simulator::Random::init() {
  // Set the seed for random using the current time
  srand(time(NULL));
}

uint32_t Ion::random() {
  /* rand() returns a pseudo-random integral number in the range between 0 and
   * RAND_MAX. We assert that RAND_MAX is at least 8 bit, this way we can merge
   * four rand() calls to create a single 32 bit value. Merging is done using
   * XOR, whose output is as likely to be a 1 as a 0. */
  static_assert(RAND_MAX >= (1 << 8), "RAND_MAX is too small");
  return (rand() << 24) ^ (rand() << 16) ^ (rand() << 8) ^ rand();
}
