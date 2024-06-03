#include <ion/events.h>
#include <ion/src/shared/events.h>

namespace Ion {
namespace Events {

// ion/src/shared/ion/events.h

bool handlePreemption(bool) { return false; }

void setPreemptiveKeyboardState(Keyboard::State state) {}

void resetPreemptiveKeyboardState() {}

void didPressNewKey() {}

void openURL(const char *url) {}

bool waitForInterruptingEvent(int maximumDelay, int *timeout) { return false; }

// ion/include/ion/events.h

const char *Event::text() const { return ""; }

Event getEvent(int *timeout) { return Events::None; }

Event getPlatformEvent() { return Events::None; }

size_t copyText(uint8_t eventId, char *buffer, size_t bufferSize) { return 0; }

bool isDefined(uint8_t eventId) { return true; }

void setSpinner(bool) {}

}  // namespace Events
}  // namespace Ion
