#include "global_preferences.h"
#include "init.h"
#include "shared/continuous_function_store.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  Ion::Storage::FileSystem::sharedFileSystem
      ->initSystemRecord<GlobalPreferences>();

  ::Shared::GlobalContext::s_sequenceStore.init();
  ::Shared::GlobalContext::s_sequenceCache.init(
      Shared::GlobalContext::s_sequenceStore.get());
  ::Shared::GlobalContext::s_continuousFunctionStore.init();
}

}  // namespace Apps
