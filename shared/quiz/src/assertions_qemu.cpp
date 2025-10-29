#include <ion.h>
#include <quiz.h>
#include <stdlib.h>

bool sSkipAssertions = false;

void quiz_assert(bool condition) {
  if (!condition) {
    quiz_print("  ASSERTION FAILED");
    if (sSkipAssertions) {
      return;
    }
    exit(1);
  }
}
