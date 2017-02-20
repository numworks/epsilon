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
  bool keySeenUp[Keyboard::NumberOfKeys];
  for (int k=0; k<Keyboard::NumberOfKeys; k++) {
    keySeenUp[k] = !Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  // Wait a little to debounce the button.
  if (sleepWithTimeout(10, timeout)) {
    return Ion::Events::None;
  }

  /* Let's discard the keys we previously saw up but which aren't anymore: those
   * were probably bouncing! */
  for (int k=0; k<Keyboard::NumberOfKeys; k++) {
    keySeenUp[k] &= !Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  while (true) {
    for (int k=0; k<Keyboard::NumberOfKeys; k++) {
      if (Keyboard::keyDown((Ion::Keyboard::Key)k)) {
        if (keySeenUp[k]) {
          Event result((Keyboard::Key)k, sIsShiftActive, sIsAlphaActive || sIsAlphaLocked);
          updateModifiersFromEvent(result);
          return result;
        }
      } else {
        keySeenUp[k] = true;
      }
    }
    if (sleepWithTimeout(10, timeout)) {
      return Ion::Events::None;
    }
  }
}

}
}
