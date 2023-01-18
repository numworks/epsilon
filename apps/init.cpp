#include "init.h"
#include "apps_container_storage.h"
#include "global_preferences.h"

namespace Apps {

void Init() {
  ::GlobalPreferences::sharedGlobalPreferences.init();
  ::AppsContainerStorage::sharedAppsContainerStorage.init();
}

}
