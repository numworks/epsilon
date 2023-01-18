#include "init.h"
#include "apps_container_storage.h"
#include "global_preferences.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  ::GlobalPreferences::sharedGlobalPreferences.init();
  ::Shared::GlobalContext::sequenceStore.init();
  ::Shared::GlobalContext::continuousFunctionStore.init();
  ::AppsContainerStorage::sharedAppsContainerStorage.init();
}

}
