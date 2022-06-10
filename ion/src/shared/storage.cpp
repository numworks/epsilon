#include <ion/storage.h>
#include <assert.h>
#include <new>
#include <string.h>
#include <ion/unicode/utf8_helper.h>

namespace Ion {

uint32_t staticStorageArea[sizeof(Storage)/sizeof(uint32_t)] = {0};

Storage * Storage::sharedStorage() {
  static Storage * storage = new (staticStorageArea) Storage();
  return storage;
}

size_t Storage::availableSize() {
  if (m_trashRecord != Record()) {
    int bufferSize = 0;
    for (char * p : *this) {
      if (Record(fullNameOfRecordStarting(p)) != m_trashRecord) {
        bufferSize += sizeOfRecordStarting(p);
      }
    }
    return k_storageSize-bufferSize-sizeof(record_size_t);
  } else {
    return InternalStorage::availableSize();
  }
}

size_t Storage::putAvailableSpaceAtEndOfRecord(Record r) {
  emptyTrash();
  return InternalStorage::putAvailableSpaceAtEndOfRecord(r);
}

void Storage::getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace) {
  emptyTrash();
  InternalStorage::getAvailableSpaceFromEndOfRecord(r, recordAvailableSpace);
}

int Storage::numberOfRecordsWithExtension(const char * extension) {
  int trashRecord = 0;
  if (FullNameHasExtension(m_trashRecord.fullName(), extension, strlen(extension))) {
    trashRecord = 1;
  }
  return InternalStorage::numberOfRecordsWithExtension(extension) - trashRecord;
}

Storage::Record::ErrorStatus Storage::createRecordWithFullName(const char * fullName, const void * data, size_t size) {
  emptyTrash();
  return InternalStorage::createRecordWithFullName(fullName, data, size);
}

Storage::Record::ErrorStatus Storage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
  emptyTrash();
  return InternalStorage::createRecordWithExtension(baseName, extension, data, size);
}

bool Storage::hasRecord(Record r) {
  return InternalStorage::hasRecord(r) && r != m_trashRecord;
}

void Storage::destroyRecord(Record record) {
  emptyTrash();
  const Record metadataRecord = recordBaseNamedWithExtension(record.fullName(), "sys");
  if (!metadataRecord.isNull()) {
    InternalStorage::destroyRecord(metadataRecord);
  }
  m_trashRecord = record;
}

Storage::Record Storage::recordWithExtensionAtIndex(const char * extension, int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  size_t extensionLength = strlen(extension);
  char * recordAddress = nullptr;
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (FullNameHasExtension(currentName, extension, extensionLength) && Record(currentName) != m_trashRecord) {
      currentIndex++;
      if (currentIndex == index) {
        recordAddress = p;
        name = currentName;
        break;
      }
    }
  }
  if (name == nullptr) {
    return Record();
  }
  Storage::Record r = Record(name);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = recordAddress;
  return Record(name);
}

Storage::Record Storage::recordNamed(const char * fullName) {
  Storage::Record r = InternalStorage::recordNamed(fullName);
  return r == m_trashRecord ? Record() : r;
}

Storage::Record Storage::recordBaseNamedWithExtension(const char * baseName, const char * extension) {
  Storage::Record r = InternalStorage::recordBaseNamedWithExtension(baseName, extension);
  return r == m_trashRecord ? Record() : r;
}

Storage::Record Storage::recordBaseNamedWithExtensions(const char * baseName, const char * const extension[], size_t numberOfExtensions) {
  Storage::Record r = InternalStorage::recordBaseNamedWithExtensions(baseName, extension, numberOfExtensions);
  return r == m_trashRecord ? Record() : r;
}

const char * Storage::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extension[], size_t numberOfExtensions) {
  size_t nameLength = baseNameLength < 0 ? strlen(baseName) : baseNameLength;
  {
    const char * lastRetrievedRecordFullName = fullNameOfRecordStarting(m_lastRecordRetrievedPointer);
    if (m_lastRecordRetrievedPointer != nullptr && strncmp(baseName, lastRetrievedRecordFullName, nameLength) == 0 && Record(lastRetrievedRecordFullName) != m_trashRecord) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(lastRetrievedRecordFullName+nameLength+1 /*+1 to pass the dot*/, extension[i]) == 0) {
          assert(UTF8Helper::CodePointIs(lastRetrievedRecordFullName + nameLength, '.'));
          return extension[i];
        }
      }
    }
  }
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (strncmp(baseName, currentName, nameLength) == 0 && Record(currentName) != m_trashRecord) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(currentName+nameLength+1 /*+1 to pass the dot*/, extension[i]) == 0) {
          assert(UTF8Helper::CodePointIs(currentName + nameLength, '.'));
          return extension[i];
        }
      }
    }
  }

  return nullptr;
}

int Storage::numberOfRecords() {
  return InternalStorage::numberOfRecords() - (m_trashRecord != NULL ? 1 : 0);
}

InternalStorage::Record Storage::recordAtIndex(int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  char * recordAddress = nullptr;
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (Record(currentName) != m_trashRecord) {
      currentIndex++;
      if (currentIndex == index) {
        recordAddress = p;
        name = currentName;
        break;
      }
    }
  }
  if (name == nullptr) {
    return Record();
  }
  Record r = Record(name);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = recordAddress;
  return Record(name);
}

void Storage::reinsertTrash(const char * extension) {
  if (!m_trashRecord.isNull()) {
    char * p = pointerOfRecord(m_trashRecord);
    const char * fullName = fullNameOfRecordStarting(p);
    if (FullNameHasExtension(fullName, extension, strlen(extension))) {
      m_trashRecord = Record();
    }
  }
}

void Storage::emptyTrash() {
  if (!m_trashRecord.isNull()) {
    InternalStorage::destroyRecord(m_trashRecord);
    m_trashRecord = Record();
  }
}

Storage::Metadata Storage::metadataForRecord(Record record) {
  return recordBaseNamedWithExtension(record.fullName(), "sys").value();
}

Storage::Record::ErrorStatus Storage::setMetadataForRecord(Record record, Metadata data) {
  Record metadataRecord = Record(record.fullName(), "sys");
  if (!hasRecord(metadataRecord)) {
    return createRecordWithExtension(record.fullName(), "sys", data.buffer, data.size);
  } else {
    return metadataRecord.setValue(data);
  }
}

void Storage::removeMetadataForRecord(Record record) {
  recordBaseNamedWithExtension(record.fullName(), "sys").destroy();
}

}
