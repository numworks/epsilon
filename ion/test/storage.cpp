#include <quiz.h>
#include <ion/storage.h>
#include <assert.h>
#include <string.h>

using namespace Ion;

Storage::Record::ErrorStatus putRecordInSharedStorage(const char * baseName, const char * extension, const char * data) {
  size_t dataSize = strlen(data);
  return Storage::sharedStorage()->createRecordWithExtension(baseName, extension, data, dataSize);
}

QUIZ_CASE(ion_storage_records_crc32) {
  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * fullNameRecord = "ionTestStorage.record1";
  Storage::Record a(baseNameRecord, extensionRecord);
  Storage::Record b(fullNameRecord);
  quiz_assert(a==b);

  Storage::Record c("A.exp");
  Storage::Record d("B.exp");
  quiz_assert(c!=d);
}

QUIZ_CASE(ion_storage_store_and_destroy_record) {
  size_t initialStorageAvailableStage = Storage::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retreive, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retreive the record
  Storage::Record retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retreivedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retreivedRecord.value().buffer)) == 0);

  // Destroy it
  retreivedRecord.destroy();
  retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retreivedRecord == Storage::Record());
  quiz_assert(Storage::sharedStorage()->availableSize() == initialStorageAvailableStage);
}


QUIZ_CASE(ion_storage_put_record_twice) {
  size_t initialStorageAvailableStage = Storage::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record";
  const char * dataRecord = "This is a test to ensure one can create, retreive, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Put the same record again: an error should be issued
  error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::NameTaken);

  // Retreive the record
  Storage::Record retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retreivedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retreivedRecord.value().buffer)) == 0);

  // Destroy it
  retreivedRecord.destroy();
  retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retreivedRecord == Storage::Record());
  quiz_assert(Storage::sharedStorage()->availableSize() == initialStorageAvailableStage);
}

QUIZ_CASE(ion_storage_invalid_renaming) {
  size_t initialStorageAvailableStage = Storage::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retreive, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retreive the record
  Storage::Record retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retreivedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retreivedRecord.value().buffer)) == 0);

  // Rename the record with an invalid name
  const char * fullNameRecord2 = "invalidNameWithoutDot";
  error = retreivedRecord.setName(fullNameRecord2);
  quiz_assert(error == Storage::Record::ErrorStatus::NonCompliantName);

  // Destroy it
  retreivedRecord.destroy();
  retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retreivedRecord == Storage::Record());
  quiz_assert(Storage::sharedStorage()->availableSize() == initialStorageAvailableStage);
}

QUIZ_CASE(ion_storage_valid_renaming) {
  size_t initialStorageAvailableStage = Storage::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retreive, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retreive the record
  Storage::Record retreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retreivedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retreivedRecord.value().buffer)) == 0);

  // Rename the record with a valid name
  const char * newFullNameRecord = "testStorage.record2";
  error = retreivedRecord.setName(newFullNameRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retreive the previous record
  Storage::Record oldRetreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(oldRetreivedRecord == Storage::Record());

  // Retreive the new record
  Storage::Record newRetreivedRecord = Storage::sharedStorage()->recordNamed(newFullNameRecord);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(newRetreivedRecord.value().buffer)) == 0);

  // Destroy it
  newRetreivedRecord.destroy();
  newRetreivedRecord = Storage::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(newRetreivedRecord == Storage::Record());
  quiz_assert(Storage::sharedStorage()->availableSize() == initialStorageAvailableStage);
}
