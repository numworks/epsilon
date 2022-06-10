#ifndef ION_STORAGE_H
#define ION_STORAGE_H

#include "internal_storage.h"

namespace Ion {

class Storage : public InternalStorage {
public:
  using InternalStorage::Record;

  using InternalStorage::expExtension;
  using InternalStorage::funcExtension;
  using InternalStorage::seqExtension;
  using InternalStorage::eqExtension;

  static Storage * sharedStorage();

  size_t availableSize();
  size_t putAvailableSpaceAtEndOfRecord(Record r);
  void getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace);

  int numberOfRecordsWithExtension(const char * extension);

  Record::ErrorStatus createRecordWithFullName(const char * fullName, const void * data, size_t size);
  Record::ErrorStatus createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size);

  bool hasRecord(Record r);

  Record recordWithExtensionAtIndex(const char * extension, int index);
  Record recordNamed(const char * fullName);
  Record recordBaseNamedWithExtension(const char * baseName, const char * extension);
  Record recordBaseNamedWithExtensions(const char * baseName, const char * const extension[], size_t numberOfExtensions);
  const char * extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extension[], size_t numberOfExtensions);

  int numberOfRecords();
  Record recordAtIndex(int index);
  void destroyRecord(Record record);

  // Trash
  void reinsertTrash(const char * extension);
  void emptyTrash();

  // Metadata
  typedef Record::Data Metadata;
  Metadata metadataForRecord(Record record);
  Record::ErrorStatus setMetadataForRecord(Record record, Metadata metadata);
  void removeMetadataForRecord(Record record);

private:
  Storage():
    InternalStorage() {}

  Record m_trashRecord;
};

}

#endif
