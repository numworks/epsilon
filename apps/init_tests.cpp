#include "global_preferences.h"
#include "init.h"
#include "shared/continuous_function_store.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  Ion::Storage::FileSystem::sharedFileSystem
      ->initSystemRecord<GlobalPreferences>();

  ::Shared::GlobalContext::sequenceStore.init();
  ::Shared::GlobalContext::sequenceCache.init(
      Shared::GlobalContext::sequenceStore.get());
  ::Shared::GlobalContext::continuousFunctionStore.init();
}

}  // namespace Apps
