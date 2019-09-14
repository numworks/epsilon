#include <quiz.h>
#include <ion.h>
#include <assert.h>

using namespace Ion::Keyboard;
using namespace Ion::Events;

QUIZ_CASE(ion_events_from_keyboard) {
  /* Ensure all events generated from the keyboard are properly defined */
  for (Key k : ValidKeys) {
    quiz_assert(Event(k, false, false).isDefined());
    quiz_assert(Event(k, true, false).isDefined());
    quiz_assert(Event(k, false, true).isDefined());
    quiz_assert(Event(k, true, true).isDefined());
  }

  // Test some fallbacks
  quiz_assert(Event(Key::EXE, false, false) == EXE);
  quiz_assert(Event(Key::EXE, true, false) == EXE);
  quiz_assert(Event(Key::EXE, false, true) == EXE);
  quiz_assert(Event(Key::EXE, true, true) == EXE);
}
