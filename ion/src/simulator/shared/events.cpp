#include <ion/events.h>
#include "haptics.h"
#include <SDL.h>

namespace Ion {
namespace Events {

void didPressNewKey() {
  Simulator::Haptics::perform();
}

}
}
