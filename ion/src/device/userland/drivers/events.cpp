#include <ion/events.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
}

const char * Event::text() const {
  return defaultText();
}

}
}
