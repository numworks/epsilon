#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/preferences.h>

namespace Poincare {

void Preferences::Init(Preferences::PartialInterface* partial) {
  Ion::Storage::FileSystem::sharedFileSystem->initSystemRecord<Instance>();
  s_otherPreferences = partial;
}

Preferences::Interface* Preferences::SharedPreferences() {
  static Instance* ptr =
      Ion::Storage::FileSystem::sharedFileSystem->findSystemRecord<Instance>();
  assert(Ion::Storage::FileSystem::sharedFileSystem
             ->findSystemRecord<Instance>() == ptr);
  return ptr;
}
}  // namespace Poincare
