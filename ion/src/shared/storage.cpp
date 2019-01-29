#include <ion.h>
#include <string.h>
#include <assert.h>
#include <new>

namespace Ion {

/* We want to implement a simple singleton pattern, to make sure the storage is
 * initialized on first use, therefore preventing the static init order fiasco.
 * That being said, we rely on knowing where the storage resides in the device's
 * memory at compile time. Indeed, we want to advertise the static storage's
 * memory address in the PlatformInfo structure (so that we can read and write
 * it in DFU).
 * Using a "static Storage storage;" variable makes it a local symbol at best,
 * preventing the PlatformInfo from retrieving its address. And making the
 * Storage variable global yields the static init fiasco issue. We're working
 * around both issues by creating a global staticStorageArea buffer, and by
 * placement-newing the Storage into that area on first use. */

uint32_t staticStorageArea[sizeof(Storage)/sizeof(uint32_t)] = {0};

Storage * Storage::sharedStorage() {
  static Storage * storage = nullptr;
  if (storage == nullptr) {
    storage = new (staticStorageArea) Storage();
  }
  return storage;
}

// RECORD

Storage::Record::Record(const char * fullName) {
  if (fullName == nullptr) {
    m_fullNameCRC32 = 0;
    return;
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  assert(*dotChar != 0);
  assert(*(dotChar+1) != 0); // Assert there is an extension
  new (this) Record(fullName, dotChar - fullName, dotChar+1, (fullName + strlen(fullName)) - (dotChar+1));
}

Storage::Record::Record(const char * baseName, const char * extension) {
  if (baseName == nullptr) {
    assert(extension == nullptr);
    m_fullNameCRC32 = 0;
    return;
  }
  new (this) Record(baseName, strlen(baseName), extension, strlen(extension));
}

uint32_t Storage::Record::checksum() {
  uint32_t crc32Results[2];
  crc32Results[0] = m_fullNameCRC32;
  Data data = value();
  crc32Results[1] = Ion::crc32PaddedString((const char *)data.buffer, data.size);
  return Ion::crc32(crc32Results, 2);
}

Storage::Record::Record(const char * basename, int basenameLength, const char * extension, int extensionLength) {
  assert(basename != nullptr);
  assert(extension != nullptr);

  // We compute the CRC32 of the CRC32s of the basename and the extension
  uint32_t crc32Results[2];
  crc32Results[0] = Ion::crc32PaddedString(basename, basenameLength);
  crc32Results[1] = Ion::crc32PaddedString(extension, extensionLength);
  m_fullNameCRC32 = Ion::crc32(crc32Results, 2);
}

// STORAGE

Storage::Storage() :
  m_magicHeader(Magic),
  m_buffer(),
  m_magicFooter(Magic),
  m_delegate(nullptr)
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

size_t Storage::availableSize() {
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

uint32_t Storage::checksum() {
  return Ion::crc32PaddedString(m_buffer, endBuffer()-m_buffer);
}

void Storage::notifyChangeToDelegate(const Record record) const {
  if (m_delegate != nullptr) {
    m_delegate->storageDidChangeForRecord(record);
  }
}

Storage::Record::ErrorStatus Storage::notifyFullnessToDelegate() const {
  if (m_delegate != nullptr) {
    m_delegate->storageIsFull();
  }
  return Record::ErrorStatus::NotEnoughSpaceAvailable;
}

Storage::Record::ErrorStatus Storage::createRecordWithFullName(const char * fullName, const void * data, size_t size) {
  size_t recordSize = sizeOfRecordWithFullName(fullName, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (isFullNameTaken(fullName)) {
    return Record::ErrorStatus::NameTaken;
  }
  // Find the end of data
  char * newRecordAddress = endBuffer();
  char * newRecord = newRecordAddress;
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideFullNameAtPosition(newRecord, fullName);
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  notifyChangeToDelegate(Record(fullName));
  return Record::ErrorStatus::None;
}

Storage::Record::ErrorStatus Storage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
  size_t recordSize = sizeOfRecordWithBaseNameAndExtension(baseName, extension, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (isBaseNameWithExtensionTaken(baseName, extension)) {
    return Record::ErrorStatus::NameTaken;
  }
  // Find the end of data
  char * newRecordAddress = endBuffer();
  char * newRecord = newRecordAddress;
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideBaseNameWithExtensionAtPosition(newRecord, baseName, extension);
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  notifyChangeToDelegate(Record(fullNameOfRecordStarting(newRecordAddress)));
  return Record::ErrorStatus::None;
}

int Storage::numberOfRecordsWithExtension(const char * extension) {
  int count = 0;
  size_t extensionLength = strlen(extension);
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    if (FullNameHasExtension(name, extension, extensionLength)) {
      count++;
    }
  }
  return count;
}

Storage::Record Storage::recordWithExtensionAtIndex(const char * extension, int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  size_t extensionLength = strlen(extension);
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (FullNameHasExtension(currentName, extension, extensionLength)) {
      currentIndex++;
    }
    if (currentIndex == index) {
      name = currentName;
      break;
    }
  }
  if (name == nullptr) {
    return Record();
  }
  return Record(name);
}

Storage::Record Storage::recordNamed(const char * fullName) {
  if (fullName == nullptr) {
    return Record();
  }
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (strcmp(currentName, fullName) == 0) {
      return Record(fullName);
    }
  }
  return Record();
}

Storage::Record Storage::recordBaseNamedWithExtension(const char * baseName, const char * extension) {
  const char * extensions[1] = {extension};
  return recordBaseNamedWithExtensions(baseName, extensions, 1);
}

Storage::Record Storage::recordBaseNamedWithExtensions(const char * baseName, const char * extensions[], size_t numberOfExtensions) {
  size_t nameLength = strlen(baseName);
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (strncmp(baseName, currentName, nameLength) == 0) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(currentName+nameLength+1 /*+1 to pass the dot*/, extensions[i]) == 0) {
          assert(UTF8Helper::CodePointIs(currentName + nameLength, '.'));
          return Record(currentName);
        }
      }
    }
  }
  return Record();
}

void Storage::destroyAllRecords() {
  overrideSizeAtPosition(m_buffer, 0);
  notifyChangeToDelegate();
}

void Storage::destroyRecordWithBaseNameAndExtension(const char * baseName, const char * extension) {
  recordBaseNamedWithExtension(baseName, extension).destroy();
}

void Storage::destroyRecordsWithExtension(const char * extension) {
  size_t extensionLength = strlen(extension);
  char * currentRecordStart = (char *)m_buffer;
  bool didChange = false;
  while (currentRecordStart != nullptr && sizeOfRecordStarting(currentRecordStart) != 0) {
    const char * currentFullName = fullNameOfRecordStarting(currentRecordStart);
    if (FullNameHasExtension(currentFullName, extension, extensionLength)) {
      Record currentRecord(currentFullName);
      currentRecord.destroy();
      didChange = true;
      continue;
    }
    currentRecordStart = *(RecordIterator(currentRecordStart).operator++());
  }
  if (didChange) {
    notifyChangeToDelegate();
  }
}

const char * Storage::fullNameOfRecord(const Record record) {
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      return fullNameOfRecordStarting(p);
    }
  }
  return nullptr;
}

Storage::Record::ErrorStatus Storage::setFullNameOfRecord(const Record record, const char * fullName) {
  if (!FullNameCompliant(fullName)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  if (isFullNameTaken(fullName, &record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = strlen(fullName) + 1;
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
      if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
        return notifyFullnessToDelegate();
      }
      overrideSizeAtPosition(p, newRecordSize);
      overrideFullNameAtPosition(p+sizeof(record_size_t), fullName);
      notifyChangeToDelegate(record);
      return Record::ErrorStatus::None;
    }
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::ErrorStatus Storage::setBaseNameWithExtensionOfRecord(Record record, const char * baseName, const char * extension) {
  if (isBaseNameWithExtensionTaken(baseName, extension, &record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = sizeOfBaseNameAndExtension(baseName, extension);
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
      if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
        return notifyFullnessToDelegate();
      }
      overrideSizeAtPosition(p, newRecordSize);
      overrideBaseNameWithExtensionAtPosition(p+sizeof(record_size_t), baseName, extension);
      notifyChangeToDelegate(record);
      return Record::ErrorStatus::None;
    }
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::Data Storage::valueOfRecord(const Record record) {
 for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      const char * fullName = fullNameOfRecordStarting(p);
      record_size_t size = sizeOfRecordStarting(p);
      const void * value = valueOfRecordStarting(p);
      return {.buffer = value, .size = size-strlen(fullName)-1-sizeof(record_size_t)};
    }
  }
  return {.buffer= nullptr, .size= 0};
}

Storage::Record::ErrorStatus Storage::setValueOfRecord(Record record, Record::Data data) {
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      const char * fullName = fullNameOfRecordStarting(p);
      size_t newRecordSize = sizeOfRecordWithFullName(fullName, data.size);
      if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+previousRecordSize, newRecordSize-previousRecordSize)) {
        return notifyFullnessToDelegate();
      }
      record_size_t fullNameSize = strlen(fullName)+1;
      overrideSizeAtPosition(p, newRecordSize);
      overrideValueAtPosition(p+sizeof(record_size_t)+fullNameSize, data.buffer, data.size);
      notifyChangeToDelegate(record);
      return Record::ErrorStatus::None;
    }
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

void Storage::destroyRecord(Record record) {
  if (record.isNull()) {
    return;
  }
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      slideBuffer(p+previousRecordSize, -previousRecordSize);
      notifyChangeToDelegate();
      return;
    }
  }
}

Storage::record_size_t Storage::sizeOfRecordStarting(char * start) const {
  return StorageHelper::unalignedShort(start);
}

const char * Storage::fullNameOfRecordStarting(char * start) const {
  return start+sizeof(record_size_t);
}

const void * Storage::valueOfRecordStarting(char * start) const {
  char * currentChar = start+sizeof(record_size_t);
  size_t fullNameLength = strlen(currentChar);
  return currentChar+fullNameLength+1;
}

size_t Storage::overrideSizeAtPosition(char * position, record_size_t size) {
  StorageHelper::writeUnalignedShort(size, position);
  return sizeof(record_size_t);
}

size_t Storage::overrideFullNameAtPosition(char * position, const char * fullName) {
  return strlcpy(position, fullName, strlen(fullName)+1) + 1;
}

size_t Storage::overrideBaseNameWithExtensionAtPosition(char * position, const char * baseName, const char * extension) {
  size_t result = strlcpy(position, baseName, strlen(baseName)+1); // strlcpy copies the null terminating char
  assert(UTF8Decoder::CharSizeOfCodePoint(k_dotChar) == 1);
  *(position+result) = k_dotChar; // Replace the null terminating char with a dot
  result++;
  result += strlcpy(position+result, extension, strlen(extension)+1);
  return result+1;
}

size_t Storage::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool Storage::isFullNameTaken(const char * fullName, const Record * recordToExclude) {
  Record r = Record(fullName);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool Storage::isBaseNameWithExtensionTaken(const char * baseName, const char * extension, Record * recordToExclude) {
  Record r = Record(baseName, extension);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool Storage::isNameOfRecordTaken(Record r, const Record * recordToExclude) {
  if (r == Record()) {
    /* If the CRC32 of fullName is 0, we want to refuse the name as it would
     * interfere with our escape case in the Record contructor, when the given
     * name is nullptr. */
    return true;
  }
  for (char * p : *this) {
    Record s(fullNameOfRecordStarting(p));
    if (recordToExclude && s == *recordToExclude) {
      continue;
    }
    if (s == r) {
      return true;
    }
  }
  return false;
}

bool Storage::FullNameCompliant(const char * fullName) {
  // We check that there is one dot and one dot only.
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  if (*dotChar == 0) {
    return false;
  }
  if (*(UTF8Helper::CodePointSearch(dotChar+1, k_dotChar)) == 0) {
    return true;
  }
  return false;
}

bool Storage::FullNameHasExtension(const char * fullName, const char * extension, size_t extensionLength) {
  if (fullName == nullptr) {
    return false;
  }
  size_t fullNameLength = strlen(fullName);
  if (fullNameLength > extensionLength) {
    const char * ext = fullName + fullNameLength - extensionLength;
    if (UTF8Helper::PreviousCodePointIs(fullName, ext, k_dotChar) && strcmp(ext, extension) == 0) {
      return true;
    }
  }
  return false;
}

char * Storage::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t Storage::sizeOfBaseNameAndExtension(const char * baseName, const char * extension) const {
  // +1 for the dot and +1 for the null terminating char
  return strlen(baseName)+1+strlen(extension)+1;
}

size_t Storage::sizeOfRecordWithBaseNameAndExtension(const char * baseName, const char * extension, size_t dataSize) const {
  return sizeOfBaseNameAndExtension(baseName, extension) + dataSize + sizeof(record_size_t);
}

size_t Storage::sizeOfRecordWithFullName(const char * fullName, size_t dataSize) const {
  size_t nameSize = strlen(fullName)+1;
  return nameSize+dataSize+sizeof(record_size_t);
}

bool Storage::slideBuffer(char * position, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  memmove(position+delta, position, endBuffer()+sizeof(record_size_t)-position);
  return true;
}

Storage::RecordIterator & Storage::RecordIterator::operator++() {
  assert(m_recordStart);
  record_size_t size = StorageHelper::unalignedShort(m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = StorageHelper::unalignedShort(nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}
