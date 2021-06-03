#include "events.h"
#include "../../shared/events_modifier.h"
#include <ion/events.h>
#include <ion/timing.h>
#include <assert.h>
#if ESCHER_LOG_EVENTS_NAME
#include <ion/console.h>
#endif
#if ION_SIMULATOR_FILES
#include <ion/src/simulator/shared/screenshot.h>
#endif

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
bool sEventIsRepeating = false;
constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

Event getPlatformEvent();

static inline Event innerGetEvent(int * timeout) {
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);

  int time = 0;
  uint64_t keysSeenUp = 0;
  uint64_t keysSeenTransitionningFromUpToDown = 0;

  while (true) {
    Event platformEvent = getPlatformEvent();
    if (platformEvent != None) {
      return platformEvent;
    }

    Keyboard::State state = Keyboard::scan();
    keysSeenUp |= ~state;
    keysSeenTransitionningFromUpToDown = keysSeenUp & state;

    bool lock = isLockActive();

    if (keysSeenTransitionningFromUpToDown != 0) {
      sEventIsRepeating = false;
      resetLongRepetition();
      didPressNewKey();
      /* The key that triggered the event corresponds to the first non-zero bit
       * in "match". This is a rather simple logic operation for the which many
       * processors have an instruction (ARM thumb uses CLZ).
       * Unfortunately there's no way to express this in standard C, so we have
       * to resort to using a builtin function. */
      Keyboard::Key key = (Keyboard::Key)(63-__builtin_clzll(keysSeenTransitionningFromUpToDown));
      bool shift = isShiftActive() || state.keyDown(Keyboard::Key::Shift);
      bool alpha = isAlphaActive() || state.keyDown(Keyboard::Key::Alpha);
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

    // At this point, we know that keysSeenTransitionningFromUpToDown has *always* been zero
    // In other words, no new key has been pressed
    if (canRepeatEvent(sLastEvent)
        && state == sLastKeyboardState
        && sLastEventShift == state.keyDown(Keyboard::Key::Shift)
        && sLastEventAlpha == (state.keyDown(Keyboard::Key::Alpha) || lock))
    {
      int delay = (sEventIsRepeating ? delayBetweenRepeat : delayBeforeRepeat);
      if (time >= delay) {
        sEventIsRepeating = true;
        incrementRepetitionFactor();
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
  Event res = Events::None;
  // Replay
  if (sSourceJournal != nullptr) {
    if (sSourceJournal->isEmpty()) {
      sSourceJournal = nullptr;
#if ESCHER_LOG_EVENTS_NAME
      Ion::Console::writeLine("----- STATE FILE FULLY LOADED -----");
#endif

    } else {
      res = sSourceJournal->popEvent();
#if ESCHER_LOG_EVENTS_NAME
      Ion::Console::writeLine("(From state file) ", false);
#endif

    }
  }

  if (res == Events::None) {
    res = innerGetEvent(timeout);
  }
  if (sDestinationJournal != nullptr) {
    sDestinationJournal->pushEvent(res);
  }
  return res;
}

#else

Event getEvent(int * timeout) {
  return innerGetEvent(timeout);
}

#endif


#if ION_EVENTS_JOURNAL

static Journal * sSourceJournal = nullptr;
static Journal * sDestinationJournal = nullptr;
void replayFrom(Journal * l) { sSourceJournal = l; }
void logTo(Journal * l) { sDestinationJournal = l; }

Event getEvent(int * timeout) {
  Event res = Events::None;
  // Replay
  if (sSourceJournal != nullptr) {
    if (sSourceJournal->isEmpty()) {
      sSourceJournal = nullptr;
#if ESCHER_LOG_EVENTS_NAME
      Ion::Console::writeLine("----- STATE FILE FULLY LOADED -----");
#endif
#if ION_SIMULATOR_FILES
      // Save screenshot
      Simulator::Screenshot::commandlineScreenshot()-> capture();
#endif
    } else {
      res = sSourceJournal->popEvent();
#if ESCHER_LOG_EVENTS_NAME
      Ion::Console::writeLine("(From state file) ", false);
#endif

    }
  }

  if (res == Events::None) {
    res = innerGetEvent(timeout);
  }
  if (sDestinationJournal != nullptr) {
    sDestinationJournal->pushEvent(res);
  }
  return res;
}

#else

Event getEvent(int * timeout) {
  return innerGetEvent(timeout);
}

#endif


}
}
