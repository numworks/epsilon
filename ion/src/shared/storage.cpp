#include <ion.h>
#include <string.h>
#include <assert.h>
#include <poincare/integer.h>
#include <new>
#include <ion/unicode/utf8_helper.h>
#if ION_STORAGE_LOG
#include<iostream>
#endif

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
 * placement-newing the Storage into that area on first use.
 * We use 'uintptr_t' to ensure the Storage to be correctly aligned accordingly
 * to the platform. */

uintptr_t staticStorageArea[sizeof(Storage)/sizeof(uintptr_t)] = {0};

constexpr char Storage::eqExtension[];
constexpr char Storage::expExtension[];
constexpr char Storage::funcExtension[];
constexpr char Storage::lisExtension[];
constexpr char Storage::seqExtension[];
constexpr char Storage::matExtension[];

Storage * Storage::sharedStorage() {
  static Storage * storage = new (staticStorageArea) Storage();
  return storage;
}

// RECORD

Storage::Record::Record(const char * fullName) {
  if (fullName == nullptr) {
    m_fullNameCRC32 = 0;
    return;
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  // If no extension, return empty record
  if (*dotChar == 0 || *(dotChar+1) == 0) {
    m_fullNameCRC32 = 0;
    return;
  }
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


#if ION_STORAGE_LOG

void Storage::Record::log() {
  std::cout << "Name: " << fullName() << std::endl;
  std::cout << "        Value (" << value().size << "): " << (char *)value().buffer << "\n\n" << std::endl;
}
#endif

uint32_t Storage::Record::checksum() {
  uint32_t crc32Results[2];
  crc32Results[0] = m_fullNameCRC32;
  Data data = value();
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)data.buffer, data.size);
  return Ion::crc32Word(crc32Results, 2);
}

int Storage::firstAvailableNameFromPrefix(char * buffer, size_t prefixLength, size_t bufferSize, const char * const extensions[], size_t numberOfExtensions, int maxId) {
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

Storage::Record::Record(const char * basename, int basenameLength, const char * extension, int extensionLength) {
  assert(basename != nullptr);
  assert(extension != nullptr);

  // We compute the CRC32 of the CRC32s of the basename and the extension
  uint32_t crc32Results[2];
  crc32Results[0] = Ion::crc32Byte((const uint8_t *)basename, basenameLength);
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)extension, extensionLength);
  m_fullNameCRC32 = Ion::crc32Word(crc32Results, 2);
}

// STORAGE

#if ION_STORAGE_LOG
void Storage::log() {
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    Record(currentName).log();
  }
}
#endif

size_t Storage::availableSize() {
  /* TODO maybe do: availableSize(char ** endBuffer) to get the endBuffer if it
   * is needed after calling availableSize */
  assert(k_storageSize >= (endBuffer() - m_buffer) + sizeof(record_size_t));
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

size_t Storage::putAvailableSpaceAtEndOfRecord(Storage::Record r) {
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

void Storage::getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord - recordAvailableSpace,
      nextRecord,
      m_buffer + k_storageSize - nextRecord);
  overrideSizeAtPosition(p, (record_size_t)(previousRecordSize - recordAvailableSpace));
}

uint32_t Storage::checksum() {
  return Ion::crc32Byte((const uint8_t *) m_buffer, endBuffer()-m_buffer);
}

void Storage::notifyChangeToDelegate(const Record record) const {
  m_lastRecordRetrieved = Record(nullptr);
  m_lastRecordRetrievedPointer = nullptr;
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

Storage::Record::ErrorStatus Storage::createRecordWithFullName(const char * fullName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks) {
  size_t totalSize = 0;
  for (size_t i=0; i<numberOfChunks; i++) {
    totalSize += sizeChunks[i];
  }
  size_t recordSize = sizeOfRecordWithFullName(fullName, totalSize);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  int basenameLength = dotChar - fullName;
  if (!destroyCompetingRecord(fullName, dotChar + 1, basenameLength)) {
    return Record::ErrorStatus::NameTaken;
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
  for (size_t i=0; i<numberOfChunks; i++) {
    newRecord += overrideValueAtPosition(newRecord, dataChunks[i], sizeChunks[i]);
  }
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  Record r = Record(fullName);
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  return Record::ErrorStatus::None;
}

Storage::Record::ErrorStatus Storage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
  size_t recordSize = sizeOfRecordWithBaseNameAndExtension(baseName, extension, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (!destroyCompetingRecord(baseName, extension)) {
    return Record::ErrorStatus::NameTaken;
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
  newRecord += overrideBaseNameWithExtensionAtPosition(newRecord, baseName, strlen(baseName), extension, strlen(extension));
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  Record r = Record(fullNameOfRecordStarting(newRecordAddress));
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  return Record::ErrorStatus::None;
}

int Storage::numberOfRecordsWithFilter(const char * extension, RecordFilter filter, const void * auxiliary) {
  int count = 0;
  size_t extensionLength = strlen(extension);
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    if (filter(name, auxiliary) && FullNameHasExtension(name, extension, extensionLength)) {
      count++;
    }
  }
  return count;
}

Storage::Record Storage::recordWithFilterAtIndex(const char * extension, int index, RecordFilter filter, const void * auxiliary) {
  int currentIndex = -1;
  const char * name = nullptr;
  size_t extensionLength = strlen(extension);
  char * recordAddress = nullptr;
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (filter(currentName, auxiliary) && FullNameHasExtension(currentName, extension, extensionLength)) {
      currentIndex++;
    }
    if (currentIndex == index) {
      recordAddress = p;
      name = currentName;
      break;
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

Storage::Record Storage::recordNamed(const char * fullName) {
  if (fullName == nullptr) {
    return Record();
  }
  Record r = Record(fullName);
  char * p = pointerOfRecord(r);
  if (p != nullptr) {
    return r;
  }
  return Record();
}

Storage::Record Storage::recordBaseNamedWithExtension(const char * baseName, const char * extension) {
  const char * extensions[1] = {extension};
  return recordBaseNamedWithExtensions(baseName, extensions, 1);
}

Storage::Record Storage::recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions) {
  return privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions);
}

const char * Storage::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions) {
  const char * result = nullptr;
  privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions, &result, baseNameLength);
  return result;
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

bool Storage::destroyCompetingRecord(const char * baseName, const char * extension, int baseNameLength, Record * excludedRecord) {
  if (m_recordNameHelper == nullptr) {
    return true;
  }
  baseNameLength = baseNameLength < 0 ? strlen(baseName) : baseNameLength;
  Record competingRecord = privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, m_recordNameHelper->competingExtensions(), m_recordNameHelper->numberOfCompetingExtensions(), nullptr, baseNameLength);
  if (competingRecord.isNull() || (excludedRecord != nullptr && *excludedRecord == competingRecord)) {
    return true;
  }
  RecordNameHelper::OverrideStatus result = m_recordNameHelper->shouldRecordBeOverridenWithNewExtension(competingRecord, extension);
  if (result == RecordNameHelper::OverrideStatus::Forbidden) {
    return false;
  }
  if (result == RecordNameHelper::OverrideStatus::Allowed) {
    competingRecord.destroy();
  }
  return true;
}


// PRIVATE

Storage::Storage() :
  m_magicHeader(Magic),
  m_buffer(),
  m_magicFooter(Magic),
  m_delegate(nullptr),
  m_recordNameHelper(nullptr),
  m_lastRecordRetrieved(nullptr),
  m_lastRecordRetrievedPointer(nullptr)
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

const char * Storage::fullNameOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    return fullNameOfRecordStarting(p);
  }
  return nullptr;
}

Storage::Record::ErrorStatus Storage::setFullNameOfRecord(Record * record, const char * fullName) {
  if (!FullNameCompliant(fullName)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  int basenameLength = dotChar - fullName;
  const char * extension = fullName + basenameLength + 1;
  return setBaseNameWithExtensionOfRecord(record, fullName, basenameLength, extension, strlen(extension));
}

Storage::Record::ErrorStatus Storage::setBaseNameWithExtensionOfRecord(Record * record, const char * baseName, int baseNameLength, const char * extension, int extensionLength) {
  Record newRecord = Record(baseName, baseNameLength, extension, extensionLength);
  Record oldRecord = *record;
  if (isNameOfRecordTaken(newRecord, record) || !destroyCompetingRecord(baseName, extension, baseNameLength, record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = baseNameLength + 1 + extensionLength + 1;
  char * p = pointerOfRecord(oldRecord);
  if (p != nullptr) {
    size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    char * fullNamePosition = p + sizeof(record_size_t);
    overrideBaseNameWithExtensionAtPosition(fullNamePosition, baseName, baseNameLength, extension, extensionLength);
    // Recompute the CRC32
    *record = newRecord;
    notifyChangeToDelegate(newRecord);
    m_lastRecordRetrieved = newRecord;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::Data Storage::valueOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    const char * fullName = fullNameOfRecordStarting(p);
    record_size_t size = sizeOfRecordStarting(p);
    const void * value = valueOfRecordStarting(p);
    return {.buffer = value, .size = size-strlen(fullName)-1-sizeof(record_size_t)};
  }
  return {.buffer= nullptr, .size= 0};
}

Storage::Record::ErrorStatus Storage::setValueOfRecord(Record record, Record::Data data) {
  char * p = pointerOfRecord(record);
  /* TODO: if data.buffer == p, assert that size hasn't change and do not do any
   * memcopy, but still notify the delegate. Beware of scripts and the accordion
   * routine.*/
  if (p != nullptr) {
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
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

void Storage::destroyRecord(Record record) {
  if (record.isNull()) {
    return;
  }
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    slideBuffer(p+previousRecordSize, -previousRecordSize);
    notifyChangeToDelegate();
  }
}

char * Storage::pointerOfRecord(const Record record) const {
  if (record.isNull()) {
    return nullptr;
  }
  if (!m_lastRecordRetrieved.isNull() && record == m_lastRecordRetrieved) {
    assert(m_lastRecordRetrievedPointer != nullptr);
    return m_lastRecordRetrievedPointer;
  }
  for (char * p : *this) {
    Record currentRecord(fullNameOfRecordStarting(p));
    if (record == currentRecord) {
      m_lastRecordRetrieved = record;
      m_lastRecordRetrievedPointer = p;
      return p;
    }
  }
  return nullptr;
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

size_t Storage::overrideBaseNameWithExtensionAtPosition(char * position, const char * baseName, int basenameLength, const char * extension, int extensionLength) {
  memcpy(position, baseName, basenameLength);
  position += basenameLength;
  assert(UTF8Decoder::CharSizeOfCodePoint(k_dotChar) == 1);
  *(position) = k_dotChar;
  position++;
  memcpy(position, extension, extensionLength);
  position += extensionLength;
  *(position) = 0;
  return basenameLength + 1 + extensionLength + 1;
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
     * interfere with our escape case in the Record constructor, when the given
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

Storage::Record Storage::privateRecordAndExtensionOfRecordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult, int baseNameLength) {
  size_t nameLength = baseNameLength < 0 ? strlen(baseName) : baseNameLength;
  {
    const char * lastRetrievedRecordFullName = fullNameOfRecordStarting(m_lastRecordRetrievedPointer);
    if (m_lastRecordRetrievedPointer != nullptr && strncmp(baseName, lastRetrievedRecordFullName, nameLength) == 0) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(lastRetrievedRecordFullName+nameLength+1 /*+1 to pass the dot*/, extensions[i]) == 0) {
          assert(UTF8Helper::CodePointIs(lastRetrievedRecordFullName + nameLength, '.'));
          if (extensionResult != nullptr) {
            *extensionResult = extensions[i];
          }
          return m_lastRecordRetrieved;
        }
      }
    }
  }
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (strncmp(baseName, currentName, nameLength) == 0) {
      for (size_t i = 0; i < numberOfExtensions; i++) {
        if (strcmp(currentName+nameLength+1 /*+1 to pass the dot*/, extensions[i]) == 0) {
          assert(UTF8Helper::CodePointIs(currentName + nameLength, '.'));
          if (extensionResult != nullptr) {
            *extensionResult = extensions[i];
          }
          return Record(currentName);
        }
      }
    }
  }
  if (extensionResult != nullptr) {
    *extensionResult = nullptr;
  }
  return Record();
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
