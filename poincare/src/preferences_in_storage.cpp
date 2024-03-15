#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/preferences.h>

namespace Poincare {

void Preferences::Init() {
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
}

static Preferences* fetchFromStorage() {
  Ion::Storage::Record record =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtension(
          Preferences::k_recordName, Ion::Storage::systemExtension);
  assert(!record.isNull());
  Ion::Storage::Record::Data data = record.value();
  assert(data.size == sizeof(Preferences));
  return static_cast<Preferences*>(const_cast<void*>(data.buffer));
}

Preferences* Preferences::SharedPreferences() {
  static Preferences* ptr = fetchFromStorage();
  assert(fetchFromStorage() == ptr);
  return ptr;
}

}  // namespace Poincare
