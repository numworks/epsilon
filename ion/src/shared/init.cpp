#include "init.h"
#include "events.h"
#include "events_modifier.h"

namespace Ion {

void Init() {
  Events::SharedModifierState.init();
  Events::SharedState.init();
}

}
