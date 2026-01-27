#include <ion/console.h>
#include <quiz.h>
#include <stdlib.h>

#include "runner.h"

void quiz_assert(bool condition) {
  if (!condition) {
    quiz_print("ASSERTION FAILED");
    if (!Quiz::Runner::s_instance.skipAssertions()) {
      abort();
    }
  }
}

void quiz_print(const char* message) { Ion::Console::writeLine(message); }
