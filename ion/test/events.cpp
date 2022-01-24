#include <quiz.h>
#include <ion.h>
#include <assert.h>

using namespace Ion::Keyboard;
using namespace Ion::Events;

QUIZ_CASE(ion_events_from_keyboard) {
  /* Ensure all events generated from the keyboard are properly defined */
  for (Key k : ValidKeys) {
    quiz_assert(Event(k, false, false, false).isDefined());
    quiz_assert(Event(k, true, false, false).isDefined());
    quiz_assert(Event(k, false, true, false).isDefined());
    quiz_assert(Event(k, true, true, false).isDefined());
    quiz_assert(Event(k, false, true, true).isDefined());
    quiz_assert(Event(k, true, true, true).isDefined());
  }

  // Test some fallbacks
  quiz_assert(Event(Key::EXE, false, false, false) == EXE);
  quiz_assert(Event(Key::EXE, true, false, false) == ShiftEXE);
  quiz_assert(Event(Key::EXE, false, true, false) == EXE);
  quiz_assert(Event(Key::EXE, true, true, false) == EXE);
  quiz_assert(Event(Key::EXE, false, true, true) == EXE);
  quiz_assert(Event(Key::EXE, true, true, true) == EXE);
}
