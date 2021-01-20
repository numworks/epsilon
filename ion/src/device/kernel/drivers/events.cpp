#include <kernel/drivers/events.h>
#include <drivers/battery.h>
#include <drivers/usb.h>
#include <kernel/drivers/config/keyboard.h>
#include <kernel/drivers/keyboard_queue.h>
#include <ion/src/shared/events_modifier.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Events {

Ion::Events::Event sLastEvent = Ion::Events::None;
Ion::Keyboard::State sLastKeyboardState;
bool sLastEventShift;
bool sLastEventAlpha;

Ion::Events::Event eventFromKeyboard(Ion::Keyboard::State state) {
// Initialize all keys as up
  static uint64_t keysSeenUp = Keyboard::Config::ValidKeys(-1);
  uint64_t keysSeenTransitionningFromUpToDown = 0;
  keysSeenUp |= ~state;
  keysSeenTransitionningFromUpToDown = keysSeenUp & state;
  bool lock = Ion::Events::isLockActive();

  // Keyboard state changed so we reinitialize the repetition tracker
  Ion::Events::resetLongRepetition();

  if (keysSeenTransitionningFromUpToDown == 0) {
    // Keys might only have transitionned from down to up
    return Ion::Events::None;
  }

  /* The key that triggered the event corresponds to the first non-zero bit
   * in "match". This is a rather simple logic operation for the which many
   * processors have an instruction (ARM thumb uses CLZ).
   * Unfortunately there's no way to express this in standard C, so we have
   * to resort to using a builtin function. */
  Ion::Keyboard::Key key = (Ion::Keyboard::Key)(63-__builtin_clzll(keysSeenTransitionningFromUpToDown));
  bool shift = Ion::Events::isShiftActive() || state.keyDown(Ion::Keyboard::Key::Shift);
  bool alpha = Ion::Events::isAlphaActive() || state.keyDown(Ion::Keyboard::Key::Alpha);
  Ion::Events::Event event(key, shift, alpha, lock);
  sLastEventShift = shift;
  sLastEventAlpha = alpha;
  Ion::Events::updateModifiersFromEvent(event);
  sLastEvent = event;
  sLastKeyboardState = state;

  if (Ion::Events::canRepeatEvent(sLastEvent)) {
    // TODO EMILIE:
    // Launch a timer whose delay depends on delayBeforeRepeat and that will do nothing or call dispatchEvent with the same event and relaunch the timer for sEventIsRepeating
   }
  return event;
}

bool sLastUSBPlugged = false;
bool sLastUSBEnumerated = false;
bool sLastBatteryCharging = false;

Ion::Events::Event getPlatformEvent() {
  // First, check if the USB device has been connected to an USB host
  bool usbEnumerated = USB::isEnumerated();
  if (usbEnumerated != sLastUSBEnumerated) {
    sLastUSBPlugged = USB::isPlugged();
    sLastBatteryCharging = Battery::isCharging();
    sLastUSBEnumerated = usbEnumerated;
    if (usbEnumerated) {
      return Ion::Events::USBEnumeration;
    }
  }

  // Second, check if the USB plugged status has changed
  bool usbPlugged = USB::isPlugged();
  if (usbPlugged != sLastUSBPlugged) {
    sLastUSBPlugged = usbPlugged;
    sLastBatteryCharging = Battery::isCharging();
    return Ion::Events::USBPlug;
  }

  // Third, check if the battery changed charging state
  bool batteryCharging = Battery::isCharging();
  if (batteryCharging != sLastBatteryCharging) {
    sLastBatteryCharging = batteryCharging;
    return Ion::Events::BatteryCharging;
  }
  return Ion::Events::None;
}

Ion::Events::Event getEvent() {
  // TODO EMILIE: Make sure to time out at one point by launching a timer
  if (!Keyboard::Queue::sharedQueue()->isEmpty()) {
    return eventFromKeyboard(Keyboard::Queue::sharedQueue()->pop());
  }
  asm("wfi");
  return Keyboard::Queue::sharedQueue()->isEmpty() ? Ion::Events::None : eventFromKeyboard(Keyboard::Queue::sharedQueue()->pop());
}

}
}
}
