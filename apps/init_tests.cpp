#include "global_preferences.h"
#include "init.h"
#include "shared/continuous_function_store.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  assert(!Ion::Storage::FileSystem::sharedFileSystem->hasRecord(
      Ion::Storage::Record(GlobalPreferences::k_recordName,
                           Ion::Storage::systemExtension)));
  GlobalPreferences defaultPreferences;
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      GlobalPreferences::k_recordName, Ion::Storage::systemExtension,
      &defaultPreferences, sizeof(defaultPreferences));
  assert(Ion::Storage::FileSystem::sharedFileSystem->hasRecord(
      Ion::Storage::Record(GlobalPreferences::k_recordName,
                           Ion::Storage::systemExtension)));

  ::Shared::GlobalContext::sequenceStore.init();
  ::Shared::GlobalContext::continuousFunctionStore.init();
}

}  // namespace Apps
