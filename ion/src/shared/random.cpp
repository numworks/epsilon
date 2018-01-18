#include <ion.h>
#include <stdlib.h>

uint32_t Ion::random() {
  /* rand() returns a pseudo-random integral number in the range between 0 and
   * RAND_MAX which is garantueed to be at least 32767.*/
  uint32_t result = 0;
  for (int i = 0; i < 131077; i++) {
    result += rand();
  }
  return result;
}
