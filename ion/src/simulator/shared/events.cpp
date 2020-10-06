#include "events.h"
#include "haptics.h"
#include <ion/events.h>
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
  if (*this == ExternalText) {
    return const_cast<const char *>(sharedExternalTextBuffer());
  }
  return defaultText();
}

bool Event::isDefined() const {
  if (*this == ExternalText) {
    return true;
  }
  return defaultIsDefined();
}

}
}
