#include <ion/events.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
}

bool Event::isDefined() const {
  return defaultIsDefined();
}

const char * Event::text() const {
  return defaultText();
}

}
}
