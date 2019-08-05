#include <ion.h>
#include <stdlib.h>
#include <quiz.h>

void quiz_assert(bool condition) {
  if (!condition) {
    // We want to keep what the screen is displaying so we cannot abort
    while (1) {
    }
  }
}
