#include "events.h"
#include "haptics.h"
#include <ion/events.h>
#include <layout_events.h>
#include <SDL.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
  Simulator::Haptics::rumble();
}

char * sharedExternalTextBuffer() {
  static char buffer[32];
  return buffer;
}

const char * Event::text() const {
  if (m_id >= 4*PageSize) {
    if (*this == ExternalText) {
      return const_cast<const char *>(sharedExternalTextBuffer());
    }
    return nullptr;
  }
  return s_dataForEvent[m_id].text();
}

bool Event::isDefined() const {
  if (isKeyboardEvent()) {
    return s_dataForEvent[m_id].isDefined();
  } else {
    return (*this == None || *this == Termination || *this == USBEnumeration || *this == USBPlug || *this == BatteryCharging || *this == ExternalText);
  }
}

}
}
