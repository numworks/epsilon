#include "events.h"

#include <SDL.h>
#include <assert.h>
#include <ion/circuit_breaker.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <ion/src/shared/dummy/usb.h>
#include <ion/src/shared/events.h>
#include <ion/src/shared/events_modifier.h>
#include <ion/src/shared/keyboard.h>
#include <ion/src/shared/keyboard_queue.h>
#include <ion/timing.h>

#include <algorithm>

#include "haptics.h"
#include "ion/src/simulator/shared/clipboard_helper.h"

#if ESCHER_LOG_EVENTS_NAME
#include <ion/console.h>
#endif

#if ION_SIMULATOR_FILES
#include "screenshot.h"
#endif

namespace Ion {
namespace Events {

// ion/src/simulator/shared/events.h

char* sharedExternalTextBuffer() {
  static char buffer[sharedExternalTextBufferSize];
  return buffer;
}

// ion/src/shared/events.h

bool handlePreemption(bool) { return false; }

void setPreemptiveKeyboardState(Keyboard::State state) {}

void resetPreemptiveKeyboardState() {}

/* getPlatformEvent defined in ./events_platform.cpp */

void didPressNewKey() { Simulator::Haptics::rumble(); }

void openURL(const char* url) { SDL_OpenURL(url); }

bool waitForInterruptingEvent(int maximumDelay, int* timeout) {
  Keyboard::scan();
  /* As pressing keys on the simulator does not generate interruptions, we need
   * to poll the keyboard more regularly than on the device. */
  constexpr int simulatorDelay = 10;
  maximumDelay = std::min(simulatorDelay, maximumDelay);
  if (*timeout < maximumDelay) {
    Timing::msleep(*timeout);
    *timeout = 0;
  } else {
    Timing::msleep(maximumDelay);
    *timeout -= maximumDelay;
  }
  return !Keyboard::Queue::sharedQueue()->isEmpty();
}

// ion/include/ion/events.h

const char* Event::text() const {
  if (*this == ExternalText) {
    return const_cast<const char*>(sharedExternalTextBuffer());
  }
  return defaultText();
}

#if ION_EVENTS_JOURNAL

static Journal* sSourceJournal = nullptr;
static Journal* sDestinationJournal = nullptr;
void replayFrom(Journal* l) { sSourceJournal = l; }
void logTo(Journal* l) { sDestinationJournal = l; }

Event getEvent(int* timeout) {
  Event nextEvent = Events::None;
  // Replay
  if (sSourceJournal != nullptr) {
    if (sSourceJournal->isEmpty()) {
      sSourceJournal = nullptr;
#if ESCHER_LOG_EVENTS_NAME
      if (Ion::Events::LogEvents()) {
        Ion::Console::writeLine("----- STATE FILE FULLY LOADED -----");
      }
#endif
    } else {
      nextEvent = sSourceJournal->popEvent();
#if ESCHER_LOG_EVENTS_NAME
      if (Ion::Events::LogEvents()) {
        Ion::Console::writeLine("(From state file) ", false);
      }
#endif
    }
#if ION_SIMULATOR_FILES
    Simulator::Screenshot::commandlineScreenshot()->capture(nextEvent);
#endif
  }

  if (nextEvent == Events::None) {
    nextEvent = sharedGetEvent(timeout);
  }
  if (nextEvent == Events::USBPlug) {
    /* Keep track of whether the simulator is plugged or unplugged. */
    Ion::USB::togglePlug();
  }
  if (sDestinationJournal != nullptr) {
    sDestinationJournal->pushEvent(nextEvent);
  }
  return nextEvent;
}

#else

Event getEvent(int* timeout) { return sharedGetEvent(timeout); }

#endif

size_t copyText(uint8_t eventId, char* buffer, size_t bufferSize) {
  return sharedCopyText(eventId, buffer, bufferSize);
}

bool isDefined(uint8_t eventId) { return sharedIsDefined(eventId); }

void setSpinner(bool) {}

}  // namespace Events
}  // namespace Ion
