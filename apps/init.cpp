#include "init.h"

#include "apps_container_storage.h"
#include "global_preferences.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  Ion::Storage::FileSystem::sharedFileSystem
      ->initSystemRecord<GlobalPreferences>();

  ::Shared::GlobalContext::sequenceStore.init();
  ::Shared::GlobalContext::sequenceCache.init(
      Shared::GlobalContext::sequenceStore.get());
  ::Shared::GlobalContext::continuousFunctionStore.init();
  ::AppsContainerStorage::sharedAppsContainerStorage.init();
}

}  // namespace Apps
