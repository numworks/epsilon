#include <quiz.h>
#include <ion.h>
#include <assert.h>

using namespace Ion::Keyboard;

QUIZ_CASE(ion_keyboard) {
  for (Key k : ValidKeys) {
    for (Key l : ValidKeys) {
      assert(State(k).keyDown(l) == (k == l));
    }
  }
}

