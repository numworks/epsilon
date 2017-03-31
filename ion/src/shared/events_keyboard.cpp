#include <ion.h>

namespace Ion {
namespace Events {

static bool sIsShiftActive = false;
static bool sIsAlphaActive = false;
static bool sIsAlphaLocked = false;

bool isShiftActive() {
  return sIsShiftActive;
}

bool isAlphaActive() {
  return sIsAlphaActive;
}

bool isAlphaLocked() {
  return sIsAlphaLocked;
}

void updateModifiersFromEvent(Event e) {
  if (e == Shift) {
    sIsShiftActive = !sIsShiftActive;
  } else if (e == Alpha) {
    if (sIsAlphaLocked) {
      sIsAlphaLocked = false;
      sIsAlphaActive = false;
    } else if (sIsAlphaActive) {
      sIsAlphaLocked = true;
      sIsAlphaActive = false;
    } else {
      sIsAlphaActive = true;
    }
  } else {
    sIsShiftActive = false;
    sIsAlphaActive = false;
  }
}

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

// Debouncing, and change to get_key event.
Event getEvent(int * timeout) {
  // Let's start by saving which keys we've seen up
  Keyboard::State keysSeenUp = ~Keyboard::scan();

  /* We'll need to sleep to prevent keys from repeating too fast.
   * As a rule of thumb, we'll assume that the user cannot input more than 20
   * keys per second. That gives a 50ms minimum delay between events. */
  int sleepDelay = 50;

  // The following two operations could be looped

  // 1 - Wait a little to deal with establishing contacts (debouncing)
  if (sleepWithTimeout(sleepDelay, timeout)) {
    return Ion::Events::None;
  }

  /* 2 - Let's discard the keys we previously saw up but which aren't anymore:
   * those were probably bouncing indeed. In terms of logic, that gives :
   * Initial keysSeenUp : 0001001
   * Current scan :       1111110
   * Updated keysSeenUp : 0000001
   * In other works, updated = initial "or not" scan.*/
  keysSeenUp |= ~Keyboard::scan();


  while (true) {
    Keyboard::State state = Keyboard::scan();
    /* We want to detect keys that are now down (1 in scan) and that were
     * previously up (1 in keysSeenUp). */
    Keyboard::State match = state & keysSeenUp;
    // Let's take this opportunity to update the keys we've seen up
    keysSeenUp |= ~state;
    if (match != 0) {
      /* The key that triggered the event corresponds to the first non-zero bit
       * in "match". This is a rather simple logic operation for the which many
       * processors have an instruction (ARM thumb uses CLZ).
       * Unfortunately there's no way to express this in standard C, so we have
       * to resort to using a builtin function. */
      Keyboard::Key key = (Keyboard::Key)(63-__builtin_clzll(match));
      Event result(key, sIsShiftActive, sIsAlphaActive || sIsAlphaLocked);
      updateModifiersFromEvent(result);
      return result;
    }
    // Last but not least, sleep for a bit if we didn't find anything
    if (sleepWithTimeout(sleepDelay, timeout)) {
      return Ion::Events::None;
    }
  }
}

}
}
