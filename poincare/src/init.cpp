#include <ion/storage/file_system.h>
#include <poincare/init.h>
#include <poincare/preferences.h>
#include <poincare/tree_pool.h>

namespace Poincare {

void Init() {
  assert(!Ion::Storage::FileSystem::sharedFileSystem->hasRecord(
      Ion::Storage::Record(Preferences::k_recordName,
                           Ion::Storage::systemExtension)));
  Preferences defaultPreferences;
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      Preferences::k_recordName, Ion::Storage::systemExtension,
      &defaultPreferences, sizeof(defaultPreferences));
  assert(Ion::Storage::FileSystem::sharedFileSystem->hasRecord(
      Ion::Storage::Record(Preferences::k_recordName,
                           Ion::Storage::systemExtension)));

  TreePool::sharedPool.init();
}

}  // namespace Poincare
