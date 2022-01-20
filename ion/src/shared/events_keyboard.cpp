#include <ion/events.h>
#include <ion/timing.h>
#include <assert.h>

namespace Ion {
namespace Events {

static bool sleepWithTimeout(int duration, int * timeout) {
  if (*timeout >= duration) {
    Timing::msleep(duration);
    *timeout -= duration;
    return false;
  } else {
    Timing::msleep(*timeout);
    *timeout = 0;
    return true;
  }
}

Event sLastEvent = Events::None;
Keyboard::State sLastKeyboardState;
bool sLastEventShift;
bool sLastEventAlpha;
bool sEventIsRepeating = 0;
int sEventRepetitionCount = 0;
constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

static bool canRepeatEvent(Event e) {
  return e == Events::Left
    || e == Events::Up
    || e == Events::Down
    || e == Events::Right
    || e == Events::Backspace
    || e == Events::ShiftLeft
    || e == Events::ShiftRight
    || e == Events::ShiftUp
    || e == Events::ShiftDown;
}

Event getPlatformEvent();

void ComputeAndSetRepetitionFactor(int eventRepetitionCount) {
  // The Repetition factor is increased by 4 every 20 loops in getEvent(2 sec)
  setLongRepetition((eventRepetitionCount / 20) * 4 + 1);
}

void resetLongRepetition() {
  sEventRepetitionCount = 0;
  ComputeAndSetRepetitionFactor(sEventRepetitionCount);
}

static Keyboard::Key keyFromState(Keyboard::State state) {
  return static_cast<Keyboard::Key>(63 - __builtin_clzll(state));
}

static inline Event innerGetEvent(int * timeout) {
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);
  int time = 0;
  uint64_t keysSeenUp = 0;
  uint64_t keysSeenTransitioningFromUpToDown = 0;
  while (true) {
    Event platformEvent = getPlatformEvent();
    if (platformEvent != None) {
      return platformEvent;
    }

    Keyboard::State state = Keyboard::scan();
    keysSeenUp |= ~state;
    keysSeenTransitioningFromUpToDown = keysSeenUp & state;

    bool lock = isLockActive();

    if (keysSeenTransitioningFromUpToDown != 0) {
      sEventIsRepeating = false;
      resetLongRepetition();
      didPressNewKey();
      /* The key that triggered the event corresponds to the first non-zero bit
       * in "match". This is a rather simple logic operation for the which many
       * processors have an instruction (ARM thumb uses CLZ).
       * Unfortunately there's no way to express this in standard C, so we have
       * to resort to using a builtin function. */
      Keyboard::Key key = (Keyboard::Key)(63-__builtin_clzll(keysSeenTransitioningFromUpToDown));
      bool shift = isShiftActive() || state.keyDown(Keyboard::Key::Shift);
      bool alpha = isAlphaActive() || state.keyDown(Keyboard::Key::Alpha);
      bool lock = isLockActive();
      
      if (   key == Keyboard::Key::Left
          || key == Keyboard::Key::Right
          || key == Keyboard::Key::Up
          || key == Keyboard::Key::Backspace
          || key == Keyboard::Key::Down) {
        if (lock) {
          lock = false;
          alpha = false;
          // shift = false;
        }
      }
      
      Event event(key, shift, alpha, lock);
      sLastEventShift = shift;
      sLastEventAlpha = alpha;
      updateModifiersFromEvent(event);
      sLastEvent = event;
      sLastKeyboardState = state;
      return event;
    }

    if (sleepWithTimeout(10, timeout)) {
      // Timeout occurred
      resetLongRepetition();
      return Events::None;
    }
    time += 10;

    // At this point, we know that keysSeenTransitioningFromUpToDown has *always* been zero
    // In other words, no new key has been pressed
    if (canRepeatEvent(sLastEvent)
        && state == sLastKeyboardState
        && sLastEventShift == state.keyDown(Keyboard::Key::Shift)
        && sLastEventAlpha == (state.keyDown(Keyboard::Key::Alpha) || lock))
    {
      int delay = (sEventIsRepeating ? delayBetweenRepeat : delayBeforeRepeat);
      if (time >= delay) {
        sEventIsRepeating = true;
        sEventRepetitionCount++;
        ComputeAndSetRepetitionFactor(sEventRepetitionCount);
        return sLastEvent;
      }
    }
  }
}

#if ION_EVENTS_JOURNAL

static Journal * sSourceJournal = nullptr;
static Journal * sDestinationJournal = nullptr;
void replayFrom(Journal * l) { sSourceJournal = l; }
void logTo(Journal * l) { sDestinationJournal = l; }

Event getEvent(int * timeout) {
  if (sSourceJournal != nullptr) {
    if (sSourceJournal->isEmpty()) {
      sSourceJournal = nullptr;
    } else {
      return sSourceJournal->popEvent();
    }
  }
  Event e = innerGetEvent(timeout);
  if (sDestinationJournal != nullptr) {
    sDestinationJournal->pushEvent(e);
  }
  return e;
}

#else

Event getEvent(int * timeout) {
  return innerGetEvent(timeout);
}

#endif


}
}
