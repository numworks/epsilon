#include <assert.h>
#include <ion.h>
#include <quiz.h>

using namespace Ion::Keyboard;
using namespace Ion::Events;

QUIZ_CASE(ion_events_from_keyboard) {
  /* FIXME Userland does not have access to method
   * Ion::Events::Event::Event(Keyboard::State, bool, bool, bool) */

#if 0
  /* Ensure all events generated from the keyboard are properly defined */
  for (Key k : ValidKeys) {
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, false, false, false))));
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, true, false, false))));
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, false, true, false))));
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, true, true, false))));
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, false, true, true))));
    quiz_assert(isDefined(static_cast<uint8_t>(Event(k, true, true, true))));
  }

  // Test some fallbacks
  quiz_assert(Event(Key::EXE, false, false, false) == EXE);
  quiz_assert(Event(Key::EXE, true, false, false) == EXE);
  quiz_assert(Event(Key::EXE, false, true, false) == EXE);
  quiz_assert(Event(Key::EXE, true, true, false) == EXE);
  quiz_assert(Event(Key::EXE, false, true, true) == EXE);
  quiz_assert(Event(Key::EXE, true, true, true) == EXE);
#endif
}
