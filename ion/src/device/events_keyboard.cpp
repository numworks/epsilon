#include <ion.h>
#include <assert.h>

namespace Ion {
namespace Events {

static bool sleepWithTimeout(int duration, int * timeout) {
  if (*timeout >= duration) {
    msleep(duration);
    *timeout -= duration;
    return false;
  } else {
    msleep(*timeout);
    *timeout = 0;
    return true;
  }
}

Event sLastEvent = Events::None;
Keyboard::State sLastKeyboardState;
bool sEventIsRepeating = 0;
constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

bool canRepeatEvent(Event e) {
  return (e == Events::Left || e == Events::Up || e == Events::Down || e == Events::Right || e == Events::Backspace);
}

Event getEvent(int * timeout) {
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);
  int time = 0;
  uint64_t keysSeenUp = 0;
  uint64_t keysSeenTransitionningFromUpToDown = 0;
  while (true) {
    Keyboard::State state = Keyboard::scan();
    keysSeenUp |= ~state;
    keysSeenTransitionningFromUpToDown = keysSeenUp & state;

    if (keysSeenTransitionningFromUpToDown != 0) {
      sEventIsRepeating = false;
      /* The key that triggered the event corresponds to the first non-zero bit
       * in "match". This is a rather simple logic operation for the which many
       * processors have an instruction (ARM thumb uses CLZ).
       * Unfortunately there's no way to express this in standard C, so we have
       * to resort to using a builtin function. */
      Keyboard::Key key = (Keyboard::Key)(63-__builtin_clzll(keysSeenTransitionningFromUpToDown));
      Event event(key, isShiftActive(), isAlphaActive());
      updateModifiersFromEvent(event);
      sLastEvent = event;
      sLastKeyboardState = state;
      return event;
    }

    if (sleepWithTimeout(10, timeout)) {
      // Timeout occured
      return Events::None;
    }
    time += 10;

    // At this point, we know that keysSeenTransitionningFromUpToDown has *always* been zero
    // In other words, no new key has been pressed
    if (canRepeatEvent(sLastEvent) && (state == sLastKeyboardState)) {
      int delay = (sEventIsRepeating ? delayBetweenRepeat : delayBeforeRepeat);
      if (time >= delay) {
        sEventIsRepeating = true;
        sLastKeyboardState = state;
        return sLastEvent;
      }
    }
  }
}

}
}
