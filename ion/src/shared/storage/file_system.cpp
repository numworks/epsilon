#include <ion.h>
#include <string.h>
#include <assert.h>
#include <poincare/integer.h>
#include <new>
#if ION_STORAGE_LOG
#include<iostream>
#endif

namespace Ion {

namespace Storage {

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
 * placement-newing the Storage into that area on first use.
 * We use 'uintptr_t' to ensure the Storage to be correctly aligned accordingly
 * to the platform. */

uintptr_t staticStorageArea[sizeof(Storage::FileSystem)/sizeof(uintptr_t)] = {0};

FileSystem * FileSystem::sharedFileSystem() {
  static FileSystem * fileSystem = new (staticStorageArea) FileSystem();
  return fileSystem;
}

// STORAGE

#if ION_STORAGE_LOG
void FileSystem::log() {
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
    Record(currentName).log();
  }
}
#endif

size_t FileSystem::availableSize() {
  /* TODO maybe do: availableSize(char ** endBuffer) to get the endBuffer if it
   * is needed after calling availableSize */
  assert(k_storageSize >= (endBuffer() - m_buffer) + sizeof(record_size_t));
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

size_t FileSystem::putAvailableSpaceAtEndOfRecord(Record r) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  size_t availableStorageSize = availableSize();
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord + availableStorageSize,
      nextRecord,
      (m_buffer + k_storageSize - availableStorageSize) - nextRecord);
  size_t newRecordSize = previousRecordSize + availableStorageSize;
  overrideSizeAtPosition(p, (record_size_t)newRecordSize);
  return newRecordSize;
}

void FileSystem::getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord - recordAvailableSpace,
      nextRecord,
      m_buffer + k_storageSize - nextRecord);
  overrideSizeAtPosition(p, (record_size_t)(previousRecordSize - recordAvailableSpace));
}

uint32_t FileSystem::checksum() {
  return Ion::crc32Byte((const uint8_t *) m_buffer, endBuffer()-m_buffer);
}

void FileSystem::notifyChangeToDelegate(const Record record) const {
  m_lastRecordRetrieved = Record(nullptr);
  m_lastRecordRetrievedPointer = nullptr;
  if (m_delegate != nullptr) {
    m_delegate->storageDidChangeForRecord(record);
  }
}

Record::ErrorStatus FileSystem::notifyFullnessToDelegate() const {
  if (m_delegate != nullptr) {
    m_delegate->storageIsFull();
  }
  return Record::ErrorStatus::NotEnoughSpaceAvailable;
}

int FileSystem::firstAvailableNameFromPrefix(char * buffer, size_t prefixLength, size_t bufferSize, const char * const extensions[], size_t numberOfExtensions, int maxId) {
  /* With '?' being the prefix, fill buffer with the first available name for
   * the extension following this pattern : ?1, ?2, ?3, .. ?10, ?11, .. ?99 */
  int id = 1;
  while (id <= maxId) {
    int length = Poincare::Integer(id).serialize(buffer + prefixLength, bufferSize - prefixLength);
    assert(buffer[prefixLength+length] == 0);
    if (recordBaseNamedWithExtensions(buffer, extensions, numberOfExtensions).isNull()) {
      return prefixLength+length;
    }
    id++;
  }
  assert(false);
  return prefixLength;
}

Record::ErrorStatus FileSystem::createRecordWithFullNameAndDataChunks(const char * fullName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks, bool extensionCanOverrideItself) {
  Record::Name recordName = Record::CreateRecordNameFromFullName(fullName);
  return createRecordWithDataChunks(recordName, dataChunks, sizeChunks, numberOfChunks, extensionCanOverrideItself);
}

Record::ErrorStatus FileSystem::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size, bool extensionCanOverrideItself) {
  Record::Name recordName = Record::CreateRecordNameFromBaseNameAndExtension(baseName, extension);
  const void * dataChunks[] = {data};
  size_t sizeChunks[] = {size};
  return createRecordWithDataChunks(recordName, dataChunks, sizeChunks, 1, extensionCanOverrideItself);
}

Record::ErrorStatus FileSystem::createRecordWithDataChunks(Record::Name recordName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks, bool extensionCanOverrideItself) {
  if (Record::NameIsEmpty(recordName)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  if (m_delegate && !m_delegate->storageWillChangeForRecordName(recordName)) {
    return Record::ErrorStatus::CanceledByDelegate;
  }
  size_t totalSize = 0;
  for (size_t i=0; i<numberOfChunks; i++) {
    totalSize += sizeChunks[i];
  }
  size_t recordSize = sizeOfRecordWithName(recordName, totalSize);
  Record recordWithSameName(recordName);
  /* pointerOfRecord will find the record with same name in the FileSystem.
   * If the record does not already exist, pointerOfRecord == nullptr and
   * sameNameRecordSize == 0 */
  size_t sameNameRecordSize = sizeOfRecordStarting(pointerOfRecord(recordWithSameName));
  if (recordSize >= k_maxRecordSize || (recordSize > sameNameRecordSize && recordSize - sameNameRecordSize > availableSize())) {
    /* If there is an other record with the same name, it will be either
     * destroyed or this new record won't be created. So we only need the
     * difference of size between the two of available space. */
   return notifyFullnessToDelegate();
  }
  /* No need to call storageWillChangeForRecordName as long as
   * recordWithSameName is identical to recordName */
  assert(sameNameRecordSize == 0 || !m_delegate || strcmp(recordWithSameName.name().extension, recordName.extension) == 0);
  /* WARNING : This relies on the fact that when you create a python script or
   * a function, you first create it with a placeholder name and then let the
   * user set its name through setNameOfRecord. */
  /* We don't notify the delegate of the destruction here since it might move
   * things in the pool and invalidate the chunks we have. It will be notified
   * later on anyway. */
  if (!handleCompetingRecord(recordName, extensionCanOverrideItself, false)) {
    return Record::ErrorStatus::NameTaken;
  }

  assert(recordSize <= availableSize());

  // Find the end of data
  char * newRecordAddress = endBuffer();
  char * newRecord = newRecordAddress;
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideNameAtPosition(newRecord, recordName);
  // Fill data
  for (size_t i=0; i<numberOfChunks; i++) {
    newRecord += overrideValueAtPosition(newRecord, dataChunks[i], sizeChunks[i]);
  }
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  Record r = Record(recordName);
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  return Record::ErrorStatus::None;
}


int FileSystem::numberOfRecordsWithFilter(const char * extension, RecordFilter filter, const void * auxiliary) {
  int count = 0;
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
    assert(currentName.extension);
    if (!Record::NameIsEmpty(currentName) && filter(currentName, auxiliary) && strcmp(currentName.extension, extension) == 0) {
      count++;
    }
  }
  return count;
}

Record FileSystem::recordWithFilterAtIndex(const char * extension, int index, RecordFilter filter, const void * auxiliary) {
  int currentIndex = -1;
  Record::Name name = Record::EmptyName();
  char * recordAddress = nullptr;
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
    assert(currentName.extension);
    if (!Record::NameIsEmpty(currentName) && filter(currentName, auxiliary) &&  strcmp(currentName.extension, extension) == 0) {
      currentIndex++;
    }
    if (currentIndex == index) {
      recordAddress = p;
      name = currentName;
      break;
    }
  }
  if (Record::NameIsEmpty(name)) {
    return Record();
  }
  Record r = Record(name);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = recordAddress;
  return Record(name);
}

Record FileSystem::recordNamed(Record::Name name) {
  if (Record::NameIsEmpty(name)) {
    return Record();
  }
  Record r = Record(name);
  char * p = pointerOfRecord(r);
  if (p != nullptr) {
    return r;
  }
  return Record();
}

Record FileSystem::recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions) {
  return privateRecordBasedNamedWithExtensions(baseName, strlen(baseName), extensions, numberOfExtensions);
}

const char * FileSystem::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions) {
  const char * result = nullptr;
  privateRecordBasedNamedWithExtensions(baseName, baseNameLength, extensions, numberOfExtensions, &result);
  return result;
}

void FileSystem::destroyAllRecords() {
  overrideSizeAtPosition(m_buffer, 0);
  notifyChangeToDelegate();
}

void FileSystem::destroyRecordsWithExtension(const char * extension) {
  char * currentRecordStart = (char *)m_buffer;
  bool didChange = false;
  while (currentRecordStart != nullptr && sizeOfRecordStarting(currentRecordStart) != 0) {
    Record::Name currentName = nameOfRecordStarting(currentRecordStart);
    if (!Record::NameIsEmpty(currentName) && strcmp(currentName.extension, extension) == 0) {
      Record currentRecord(currentName);
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

bool FileSystem::handleCompetingRecord(Record::Name recordName, bool destroyRecordWithSameFullName, bool notifyDelegate) {
  Record sameNameRecord = Record(recordName);
  if (isNameOfRecordTaken(sameNameRecord)) {
    if (destroyRecordWithSameFullName) {
      destroyRecord(sameNameRecord, notifyDelegate);
      return true;
    }
    return false;
  }
  Record competingRecord = privateRecordBasedNamedWithExtensions(recordName.baseName, recordName.baseNameLength, m_recordNameVerifier.restrictiveExtensions(), m_recordNameVerifier.numberOfRestrictiveExtensions());
  if (competingRecord.isNull()) {
    return true;
  }
  RecordNameVerifier::OverrideStatus result = m_recordNameVerifier.canOverrideRecordWithNewExtension(competingRecord, recordName.extension);
  if (result == RecordNameVerifier::OverrideStatus::Forbidden) {
    return false;
  }
  if (result == RecordNameVerifier::OverrideStatus::Allowed) {
    destroyRecord(competingRecord, notifyDelegate);
  }
  return true;
}

// PRIVATE

FileSystem::FileSystem() :
  m_magicHeader(Magic),
  m_buffer(),
  m_magicFooter(Magic),
  m_delegate(nullptr),
  m_lastRecordRetrieved(nullptr),
  m_lastRecordRetrievedPointer(nullptr)
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

Record::Name FileSystem::nameOfRecord(const Record record) const {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    return nameOfRecordStarting(p);
  }
  return Record::EmptyName();
}

Record::ErrorStatus FileSystem::setNameOfRecord(Record * record, Record::Name name) {
  if (Record::NameIsEmpty(name)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  Record newRecord = Record(name);
  Record oldRecord = *record;
  if (newRecord == oldRecord) {
    // Name has not changed
    return Record::ErrorStatus::None;
  }
  if (m_delegate && !m_delegate->storageWillChangeForRecordName(record->name())) {
    return Record::ErrorStatus::CanceledByDelegate;
  }
  /* If you do not verifiy that the name has not changed if the previous 'if'
   * this will return false, and see the name as taken. */
  if (!handleCompetingRecord(name, false)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = Record::SizeOfName(name);
  char * p = pointerOfRecord(oldRecord);
  if (p != nullptr) {
    size_t previousNameSize = Record::SizeOfName(nameOfRecordStarting(p));
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    char * namePosition = p + sizeof(record_size_t);
    overrideNameAtPosition(namePosition, name);
    // Recompute the CRC32
    *record = newRecord;
    notifyChangeToDelegate(newRecord);
    m_lastRecordRetrieved = newRecord;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

Record::Data FileSystem::valueOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    Record::Name name = nameOfRecordStarting(p);
    record_size_t size = sizeOfRecordStarting(p);
    const void * value = valueOfRecordStarting(p);
    return {.buffer = value, .size = size - Record::SizeOfName(name) - sizeof(record_size_t)};
  }
  return {.buffer= nullptr, .size= 0};
}

Record::ErrorStatus FileSystem::setValueOfRecord(Record record, Record::Data data) {
  char * p = pointerOfRecord(record);
  /* TODO: if data.buffer == p, assert that size hasn't change and do not do any
   * memcopy, but still notify the delegate. Beware of scripts and the accordion
   * routine.*/
  if (p != nullptr) {
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    Record::Name name = nameOfRecordStarting(p);
    size_t newRecordSize = sizeOfRecordWithName(name, data.size);
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+previousRecordSize, newRecordSize-previousRecordSize)) {
      return notifyFullnessToDelegate();
    }
    if (m_delegate && !m_delegate->storageWillChangeForRecordName(record.name())) {
      return Record::ErrorStatus::CanceledByDelegate;
    }
    record_size_t nameSize = Record::SizeOfName(name);
    overrideSizeAtPosition(p, newRecordSize);
    overrideValueAtPosition(p + sizeof(record_size_t) + nameSize, data.buffer, data.size);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

void FileSystem::destroyRecord(Record record, bool notifyDelegate) {
  if (record.isNull()) {
    return;
  }
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    slideBuffer(p+previousRecordSize, -previousRecordSize);
    if (notifyDelegate) {
      notifyChangeToDelegate();
    }
  }
}

char * FileSystem::pointerOfRecord(const Record record) const {
  if (record.isNull()) {
    return nullptr;
  }
  if (!m_lastRecordRetrieved.isNull() && record == m_lastRecordRetrieved) {
    assert(m_lastRecordRetrievedPointer != nullptr);
    return m_lastRecordRetrievedPointer;
  }
  for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      m_lastRecordRetrieved = record;
      m_lastRecordRetrievedPointer = p;
      return p;
    }
  }
  return nullptr;
}

FileSystem::record_size_t FileSystem::sizeOfRecordStarting(char * start) const {
  if (start == nullptr) {
    return 0;
  }
  return StorageHelper::unalignedShort(start);
}

const void * FileSystem::valueOfRecordStarting(char * start) const {
  if (start == nullptr) {
    return nullptr;
  }
  char * currentChar = start + sizeof(record_size_t);
  size_t fullNameLength = strlen(currentChar);
  return currentChar + fullNameLength + 1;
}

Record::Name FileSystem::nameOfRecordStarting(char * start) const {
   if (start == nullptr) {
    return Record::EmptyName();
   }
    return Record::CreateRecordNameFromFullName(start + sizeof(record_size_t));
  }


size_t FileSystem::overrideSizeAtPosition(char * position, record_size_t size) {
  StorageHelper::writeUnalignedShort(size, position);
  return sizeof(record_size_t);
}

size_t FileSystem::overrideNameAtPosition(char * position, Record::Name name) {
  memcpy(position, name.baseName, name.baseNameLength);
  position += name.baseNameLength;
  assert(UTF8Decoder::CharSizeOfCodePoint(Record::k_dotChar) == 1);
  *(position) = Record::k_dotChar;
  position++;
  int extensionLength = strlen(name.extension);
  memcpy(position, name.extension, extensionLength);
  position += extensionLength;
  *(position) = 0;
  return name.baseNameLength + 1 + extensionLength + 1;
}

size_t FileSystem::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool FileSystem::isNameOfRecordTaken(Record r, const Record * recordToExclude) {
  if (r == Record()) {
    /* If the CRC32 of fullName is 0, we want to refuse the name as it would
     * interfere with our escape case in the Record constructor, when the given
     * name is nullptr. */
    return true;
  }
  for (char * p : *this) {
    Record s(nameOfRecordStarting(p));
    if (recordToExclude && s == *recordToExclude) {
      continue;
    }
    if (s == r) {
      return true;
    }
  }
  return false;
}

char * FileSystem::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t FileSystem::sizeOfRecordWithName(Record::Name name, size_t dataSize) {
  return Record::SizeOfName(name) + dataSize + sizeof(record_size_t);
}

bool FileSystem::slideBuffer(char * position, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  memmove(position+delta, position, endBuffer()+sizeof(record_size_t)-position);
  return true;
}

Record FileSystem::privateRecordBasedNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult) {
  Record::Name lastRetrievedRecordName = nameOfRecordStarting(m_lastRecordRetrievedPointer);
  if (m_lastRecordRetrievedPointer != nullptr && recordNameHasBaseNameAndOneOfTheseExtensions(lastRetrievedRecordName, baseName, baseNameLength, extensions, numberOfExtensions, extensionResult)) {
    return m_lastRecordRetrieved;
  }
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
    if (recordNameHasBaseNameAndOneOfTheseExtensions(currentName, baseName, baseNameLength, extensions, numberOfExtensions, extensionResult)) {
      return Record(currentName);
    }
  }
  if (extensionResult != nullptr) {
    *extensionResult = nullptr;
  }
  return Record();
}

bool FileSystem::recordNameHasBaseNameAndOneOfTheseExtensions(Record::Name name, const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult) {
    assert(name.baseName);
  if (!Record::NameIsEmpty(name) && strncmp(baseName, name.baseName, baseNameLength) == 0 && static_cast<size_t>(baseNameLength) == name.baseNameLength) {
    for (size_t i = 0; i < numberOfExtensions; i++) {
      if (strcmp(name.extension, extensions[i]) == 0) {
        if (extensionResult != nullptr) {
          *extensionResult = extensions[i];
        }
        return true;
      }
    }
  }
  return false;
}

FileSystem::RecordIterator & FileSystem::RecordIterator::operator++() {
  assert(m_recordStart);
  record_size_t size = StorageHelper::unalignedShort(m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = StorageHelper::unalignedShort(nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}

}
