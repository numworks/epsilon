#include <quiz.h>
#include <ion/storage/container.h>
#include <assert.h>
#include <string.h>

using namespace Ion;

Storage::Record::ErrorStatus putRecordInSharedStorage(const char * baseName, const char * extension, const char * data) {
  size_t dataSize = strlen(data);
  return Storage::Container::sharedStorage()->createRecordWithExtension(baseName, extension, data, dataSize);
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
  size_t initialStorageAvailableStage = Storage::Container::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retrieve, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retrieve the record
  Storage::Record retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retrievedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retrievedRecord.value().buffer)) == 0);

  // Destroy it
  retrievedRecord.destroy();
  retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retrievedRecord == Storage::Record());
  quiz_assert(Storage::Container::sharedStorage()->availableSize() == initialStorageAvailableStage);
}


QUIZ_CASE(ion_storage_put_record_twice) {
  size_t initialStorageAvailableStage = Storage::Container::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record";
  const char * dataRecord = "This is a test to ensure one can create, retrieve, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Put the same record again: an error should be issued
  error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::NameTaken);

  // Retrieve the record
  Storage::Record retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retrievedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retrievedRecord.value().buffer)) == 0);

  // Destroy it
  retrievedRecord.destroy();
  retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retrievedRecord == Storage::Record());
  quiz_assert(Storage::Container::sharedStorage()->availableSize() == initialStorageAvailableStage);
}

QUIZ_CASE(ion_storage_invalid_renaming) {
  size_t initialStorageAvailableStage = Storage::Container::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retrieve, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retrieve the record
  Storage::Record retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retrievedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retrievedRecord.value().buffer)) == 0);

  // Rename the record with an invalid name
  const char * fullNameRecord2 = "invalidNameWithoutDot";
  error = Ion::Storage::Record::SetFullName(&retrievedRecord, fullNameRecord2);
  quiz_assert(error == Storage::Record::ErrorStatus::NonCompliantName);

  // Destroy it
  retrievedRecord.destroy();
  retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(retrievedRecord == Storage::Record());
  quiz_assert(Storage::Container::sharedStorage()->availableSize() == initialStorageAvailableStage);
}

QUIZ_CASE(ion_storage_valid_renaming) {
  size_t initialStorageAvailableStage = Storage::Container::sharedStorage()->availableSize();

  const char * baseNameRecord = "ionTestStorage";
  const char * extensionRecord = "record1";
  const char * dataRecord = "This is a test to ensure one can create, retrieve, modify and delete records in ion's shared storage.";

  // Put a record in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord, extensionRecord, dataRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retrieve the record
  Storage::Record retrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  size_t dataRecordSize = strlen(dataRecord);
  quiz_assert(retrievedRecord.value().size == dataRecordSize);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(retrievedRecord.value().buffer)) == 0);

  // Rename the record with a valid name
  const char * newFullNameRecord = "testStorage.record2";
  error = Ion::Storage::Record::SetFullName(&retrievedRecord, newFullNameRecord);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retrieve the previous record
  Storage::Record oldRetrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(oldRetrievedRecord == Storage::Record());

  // Retrieve the new record
  Storage::Record newRetrievedRecord = Storage::Container::sharedStorage()->recordNamed(newFullNameRecord);
  quiz_assert(strcmp(dataRecord, static_cast<const char *>(newRetrievedRecord.value().buffer)) == 0);

  // Destroy it
  newRetrievedRecord.destroy();
  newRetrievedRecord = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord, extensionRecord);
  quiz_assert(newRetrievedRecord == Storage::Record());
  quiz_assert(Storage::Container::sharedStorage()->availableSize() == initialStorageAvailableStage);
}

QUIZ_CASE(ion_storage_available_space_moving) {
  const char * extensionRecord = "record1";
  const char * baseNameRecord1 = "ionTestStorage1";
  const char * dataRecord1 = "This is a test to ensure one can edit a record in the shared storage - first record.";
  const char * baseNameRecord2 = "ionTestStorage2";
  const char * dataRecord2 = "This is a test to ensure one can edit a record in the shared storage - second record.";
  const char * baseNameRecord3 = "ionTestStorage3";
  const char * baseNameRecord4 = "ionTestStorage4";
  const char * dataRecord3 = R"(
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaa




aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaa
aaaaaa)";

  // Put the records in the store
  Storage::Record::ErrorStatus error = putRecordInSharedStorage(baseNameRecord1, extensionRecord, dataRecord1);
  quiz_assert(error == Storage::Record::ErrorStatus::None);
  error = putRecordInSharedStorage(baseNameRecord2, extensionRecord, dataRecord2);
  quiz_assert(error == Storage::Record::ErrorStatus::None);
  error = putRecordInSharedStorage(baseNameRecord3, extensionRecord, dataRecord3);
  quiz_assert(error == Storage::Record::ErrorStatus::None);
  error = putRecordInSharedStorage(baseNameRecord4, extensionRecord, dataRecord3);
  quiz_assert(error == Storage::Record::ErrorStatus::None);

  // Retrieve the record
  Storage::Record retrievedRecord1 = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord1, extensionRecord);
  Storage::Record retrievedRecord2 = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord2, extensionRecord);
  Storage::Record retrievedRecord3 = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord3, extensionRecord);
  Storage::Record retrievedRecord4 = Storage::Container::sharedStorage()->recordBaseNamedWithExtension(baseNameRecord4, extensionRecord);

  // Put the the available space at the end of the first record and remove it
  size_t availableSpace = Storage::Container::sharedStorage()->availableSize();
  uint32_t checksumBeforeChanges = Storage::Container::sharedStorage()->checksum();
  Storage::Container::sharedStorage()->putAvailableSpaceAtEndOfRecord(retrievedRecord1);
  Storage::Container::sharedStorage()->getAvailableSpaceFromEndOfRecord(retrievedRecord1, availableSpace);
  quiz_assert(Storage::Container::sharedStorage()->availableSize() == availableSpace);
  quiz_assert(Storage::Container::sharedStorage()->checksum() == checksumBeforeChanges);

  // Destroy it
  retrievedRecord1.destroy();
  retrievedRecord2.destroy();
  retrievedRecord3.destroy();
  retrievedRecord4.destroy();
}
