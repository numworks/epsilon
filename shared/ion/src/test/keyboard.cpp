#include <assert.h>
#include <ion.h>
#include <quiz.h>

using namespace Ion::Keyboard;

QUIZ_CASE(ion_keyboard) {
  for (Key k : ValidKeys) {
    for (Key l : ValidKeys) {
      quiz_assert(State(k).keyDown(l) == (k == l));
    }
  }
}
