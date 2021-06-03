#include "events.h"
#include "haptics.h"
#include <ion/events.h>
#include <layout_events.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
  Simulator::Haptics::rumble();
}

char * sharedExternalTextBuffer() {
  static char buffer[sharedExternalTextBufferSize];
  return buffer;
}

const char * Event::text() const {
  if (*this == ExternalText) {
    return const_cast<const char *>(sharedExternalTextBuffer());
  }
  return defaultText();
}

size_t copyText(uint8_t eventId, char * buffer, size_t bufferSize) {
  Ion::Events::Event e(eventId);
  if (e.text()) {
    return strlcpy(buffer, e.text(), bufferSize);
  } else {
    return 0;
  }
}

bool isDefined(uint8_t eventId) {
  Event e(eventId);
  if (e.isKeyboardEvent()) {
    return s_dataForEvent[static_cast<uint8_t>(e)].isDefined();
  } else {
    return (e == None || e == Termination || e == USBEnumeration || e == USBPlug || e == BatteryCharging || e == ExternalText);
  }
}

void setSpinner(bool) {}

}
}
