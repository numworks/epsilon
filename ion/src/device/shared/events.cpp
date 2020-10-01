#include <ion/events.h>
#include <layout_events.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
}

bool Event::isDefined() const {
  if (isKeyboardEvent()) {
    return s_dataForEvent[m_id].isDefined();
  } else {
    return (*this == None || *this == Termination || *this == USBEnumeration || *this == USBPlug || *this == BatteryCharging);
  }
}

const char * Event::text() const {
  if (m_id >= 4*PageSize) {
    return nullptr;
  }
  return s_dataForEvent[m_id].text();
}

}
}
