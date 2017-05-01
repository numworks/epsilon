#include <quiz.h>
#include <ion.h>
#include <assert.h>

using namespace Ion::Keyboard;
using namespace Ion::Events;

QUIZ_CASE(ion_events_from_keyboard) {
  /* Ensure all events generated from the keyboard are properly defined */
  for (Key k : ValidKeys) {
    assert(Event(k, false, false).isDefined());
    assert(Event(k, true, false).isDefined());
    assert(Event(k, false, true).isDefined());
    assert(Event(k, true, true).isDefined());
  }

  // Test some fallbacks
  assert(Event(Key::I5, false, false) == EXE);
  assert(Event(Key::I5, true, false) == EXE);
  assert(Event(Key::I5, false, true) == EXE);
  assert(Event(Key::I5, true, true) == EXE);
}
