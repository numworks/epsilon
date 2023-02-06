#include "init.h"
#include "events.h"
#include "events_modifier.h"
#include <ion/storage/file_system.h>

namespace Ion {

void Init() {
  Events::SharedModifierState.init();
  Events::SharedState.init();
  Storage::FileSystem::sharedFileSystem.init();
}

}
