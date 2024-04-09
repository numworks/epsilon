#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/preferences.h>

namespace Poincare {

void Preferences::Init() {
  Ion::Storage::FileSystem::sharedFileSystem->initSystemRecord<Preferences>();
}

Preferences* Preferences::SharedPreferences() {
  static Preferences* ptr = Ion::Storage::FileSystem::sharedFileSystem
                                ->findSystemRecord<Preferences>();
  assert(Ion::Storage::FileSystem::sharedFileSystem
             ->findSystemRecord<Preferences>() == ptr);
  return ptr;
}

}  // namespace Poincare
