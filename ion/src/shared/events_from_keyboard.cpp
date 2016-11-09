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

// Debouncing, and change to get_key event.
Event getEvent() {
  // Let's start by saving which keys we've seen up
  bool keySeenUp[Keyboard::NumberOfKeys];
  for (int k=0; k<Keyboard::NumberOfKeys; k++) {
    keySeenUp[k] = !Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  // Wait a little to debounce the button.
  msleep(10);

  /* Let's discard the keys we previously saw up but which aren't anymore: those
   * were probably bouncing! */
  for (int k=0; k<Keyboard::NumberOfKeys; k++) {
    keySeenUp[k] &= !Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  while (1) {
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
    msleep(10);
  }
}

}
}
