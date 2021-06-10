#include "events.h"
#include "haptics.h"
#include <ion/circuit_breaker.h>
#include <ion/events.h>
#include <ion/timing.h>
#include <ion/src/shared/events.h>
#include <ion/src/shared/events_modifier.h>
#include <assert.h>
#include <algorithm>
#include <layout_events.h>


namespace Ion {
namespace Events {

// ion/src/simulator/shared/events.h

char * sharedExternalTextBuffer() {
  static char buffer[sharedExternalTextBufferSize];
  return buffer;
}

// ion/src/shared/events.h

bool handlePreemption(bool) {
  return false;
}

/* getPlatformEvent defined in ./events_platform.cpp */

void didPressNewKey(Keyboard::Key key) {
  Simulator::Haptics::rumble();
  if (key == Keyboard::Key::Home && CircuitBreaker::hasCheckpoint(CircuitBreaker::CheckpointType::Home)) {
    CircuitBreaker::loadCheckpoint(CircuitBreaker::CheckpointType::Home);
  }
}

Keyboard::State popKeyboardState() {
  return Keyboard::popState();
}

bool waitForInterruptingEvent(int maximumDelay, int * timeout) {
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
  return false;
}

// ion/include/ion/events.h

const char * Event::text() const {
  if (*this == ExternalText) {
    return const_cast<const char *>(sharedExternalTextBuffer());
  }
  return defaultText();
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
    } else {
      res = sSourceJournal->popEvent();
    }
  }

  if (res == Events::None) {
    res = sharedGetEvent(timeout);
  }
  if (sDestinationJournal != nullptr) {
    sDestinationJournal->pushEvent(res);
  }
  return res;
}

#else

Event getEvent(int * timeout) {
  return sharedGetEvent(timeout);
}

#endif

size_t copyText(uint8_t eventId, char * buffer, size_t bufferSize) {
  return sharedCopyText(eventId, buffer, bufferSize);
}

bool isDefined(uint8_t eventId) {
  return sharedIsDefined(eventId);
}

void setSpinner(bool) {}

}
}
