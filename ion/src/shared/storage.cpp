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

Storage * Storage::sharedStorage() {
  static Storage * storage = new (staticStorageArea) Storage();
  return storage;
}

// RECORD

Storage::Record::Name Storage::Record::CreateRecordNameFromFullName(const char * fullName) {
  if (fullName == nullptr || fullName[0] == 0) {
    return EmptyName();
  }
  const char * dotChar = UTF8Helper::CodePointSearch(fullName, k_dotChar);
  if (*dotChar == 0 || *(dotChar + 1) == 0 || *(UTF8Helper::CodePointSearch(dotChar + 1, k_dotChar)) != 0) {
    // Name not compliant
    return EmptyName();
  }
  return Name({fullName, static_cast<size_t>(dotChar - fullName), dotChar + 1});
}

Storage::Record::Name Storage::Record::CreateRecordNameFromBaseNameAndExtension(const char * baseName, const char * extension) {
  return Name({baseName, strlen(baseName), extension});
}

size_t Storage::Record::SizeOfName(Record::Name name) {
  // +1 for the dot and +1 for the null terminating char.
  return name.baseNameLength + 1 + strlen(name.extension) + 1;
}

bool Storage::Record::NameIsEmpty(Name name) {
  return name.baseName == nullptr || name.extension == nullptr;
}

Storage::Record::Record(Storage::Record::Name name) {
  if (NameIsEmpty(name)) {
    m_fullNameCRC32 = 0;
    return;
  }
  // We compute the CRC32 of the CRC32s of the basename and the extension
  uint32_t crc32Results[2];
  crc32Results[0] = Ion::crc32Byte((const uint8_t *)name.baseName, name.baseNameLength);
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)name.extension, strlen(name.extension));
  m_fullNameCRC32 = Ion::crc32Word(crc32Results, 2);
}

Storage::Record::Record(const char * fullName) {
  Name name = CreateRecordNameFromFullName(fullName);
  new (this) Record(name);
}

Storage::Record::Record(const char * baseName, const char * extension) {
  Name name = CreateRecordNameFromBaseNameAndExtension(baseName, extension);
  new (this) Record(name);
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

// STORAGE

#if ION_STORAGE_LOG
void Storage::log() {
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
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

Storage::Record::ErrorStatus Storage::createRecordWithFullNameAndDataChunks(const char * fullName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks) {
  Record::Name recordName = Record::CreateRecordNameFromFullName(fullName);
  return createRecordWithDataChunks(recordName, dataChunks, sizeChunks, numberOfChunks);
}

Storage::Record::ErrorStatus Storage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
  Record::Name recordName = Record::CreateRecordNameFromBaseNameAndExtension(baseName, extension);
  const void * dataChunks[] = {data};
  size_t sizeChunks[] = {size};
  return createRecordWithDataChunks(recordName, dataChunks, sizeChunks, 1);
}

Storage::Record::ErrorStatus Storage::createRecordWithDataChunks(Record::Name recordName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks) {
  if (Record::NameIsEmpty(recordName)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  size_t totalSize = 0;
  for (size_t i=0; i<numberOfChunks; i++) {
    totalSize += sizeChunks[i];
  }
  size_t recordSize = sizeOfRecordWithName(recordName, totalSize);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return notifyFullnessToDelegate();
  }
  if (!destroyCompetingRecord(recordName) || isNameTaken(recordName)) {
    return Record::ErrorStatus::NameTaken;
  }

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


int Storage::numberOfRecordsWithFilter(const char * extension, RecordFilter filter, const void * auxiliary) {
  int count = 0;
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
    if (!Record::NameIsEmpty(currentName) && filter(currentName, auxiliary) && strcmp(currentName.extension, extension) == 0) {
      count++;
    }
  }
  return count;
}

Storage::Record Storage::recordWithFilterAtIndex(const char * extension, int index, RecordFilter filter, const void * auxiliary) {
  int currentIndex = -1;
  Record::Name name = Record::EmptyName();
  char * recordAddress = nullptr;
  for (char * p : *this) {
    Record::Name currentName = nameOfRecordStarting(p);
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

Storage::Record Storage::recordNamed(Record::Name name) {
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

Storage::Record Storage::recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions) {
  return privateRecordBasedNamedWithExtensions(baseName, strlen(baseName), extensions, numberOfExtensions);
}

const char * Storage::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions) {
  const char * result = nullptr;
  privateRecordBasedNamedWithExtensions(baseName, baseNameLength, extensions, numberOfExtensions, &result);
  return result;
}

void Storage::destroyAllRecords() {
  overrideSizeAtPosition(m_buffer, 0);
  notifyChangeToDelegate();
}

void Storage::destroyRecordsWithExtension(const char * extension) {
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

bool Storage::destroyCompetingRecord(Record::Name recordName, Record * excludedRecord) {
  if (m_recordDelegate == nullptr) {
    return true;
  }
  Record competingRecord = privateRecordBasedNamedWithExtensions(recordName.baseName, recordName.baseNameLength, m_recordDelegate->competingExtensions(), m_recordDelegate->numberOfCompetingExtensions());
  if (competingRecord.isNull() || (excludedRecord != nullptr && *excludedRecord == competingRecord)) {
    return true;
  }
  RecordDelegate::OverrideStatus result = m_recordDelegate->shouldRecordBeOverridenWithNewExtension(competingRecord, recordName.extension);
  if (result == RecordDelegate::OverrideStatus::Forbidden) {
    return false;
  }
  if (result == RecordDelegate::OverrideStatus::Allowed) {
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
  m_recordDelegate(nullptr),
  m_lastRecordRetrieved(nullptr),
  m_lastRecordRetrievedPointer(nullptr)
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

Storage::Record::Name Storage::nameOfRecord(const Record record) const {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    return nameOfRecordStarting(p);
  }
  return Record::EmptyName();
}

Storage::Record::ErrorStatus Storage::setNameOfRecord(Record * record, Record::Name name) {
  if (Record::NameIsEmpty(name)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  Record newRecord = Record(name);
  Record oldRecord = *record;
  if (isNameOfRecordTaken(newRecord, record) || !destroyCompetingRecord(name, record)) {
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

Storage::Record::Data Storage::valueOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    Record::Name name = nameOfRecordStarting(p);
    record_size_t size = sizeOfRecordStarting(p);
    const void * value = valueOfRecordStarting(p);
    return {.buffer = value, .size = size - Record::SizeOfName(name) - sizeof(record_size_t)};
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
    Record::Name name = nameOfRecordStarting(p);
    size_t newRecordSize = sizeOfRecordWithName(name, data.size);
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+previousRecordSize, newRecordSize-previousRecordSize)) {
      return notifyFullnessToDelegate();
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
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      m_lastRecordRetrieved = record;
      m_lastRecordRetrievedPointer = p;
      return p;
    }
  }
  return nullptr;
}

Storage::record_size_t Storage::sizeOfRecordStarting(char * start) const {
  if (start == nullptr) {
    return 0;
  }
  return StorageHelper::unalignedShort(start);
}

const void * Storage::valueOfRecordStarting(char * start) const {
  if (start == nullptr) {
    return nullptr;
  }
  char * currentChar = start + sizeof(record_size_t);
  size_t fullNameLength = strlen(currentChar);
  return currentChar + fullNameLength + 1;
}

Storage::Record::Name Storage::nameOfRecordStarting(char * start) const {
   if (start == nullptr) {
    return Record::EmptyName();
   }
    return Record::CreateRecordNameFromFullName(start + sizeof(record_size_t));
  }


size_t Storage::overrideSizeAtPosition(char * position, record_size_t size) {
  StorageHelper::writeUnalignedShort(size, position);
  return sizeof(record_size_t);
}

size_t Storage::overrideNameAtPosition(char * position, Record::Name name) {
  memcpy(position, name.baseName, name.baseNameLength);
  position += name.baseNameLength;
  assert(UTF8Decoder::CharSizeOfCodePoint(k_dotChar) == 1);
  *(position) = k_dotChar;
  position++;
  int extensionLength = strlen(name.extension);
  memcpy(position, name.extension, extensionLength);
  position += extensionLength;
  *(position) = 0;
  return name.baseNameLength + 1 + extensionLength + 1;
}

size_t Storage::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool Storage::isNameTaken(Record::Name name, const Record * recordToExclude) {
  Record r = Record(name);
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

char * Storage::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t Storage::sizeOfRecordWithName(Record::Name name, size_t dataSize) {
  return Record::SizeOfName(name) + dataSize + sizeof(record_size_t);
}

bool Storage::slideBuffer(char * position, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  memmove(position+delta, position, endBuffer()+sizeof(record_size_t)-position);
  return true;
}

Storage::Record Storage::privateRecordBasedNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult) {
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

bool Storage::recordNameHasBaseNameAndOneOfTheseExtensions(Record::Name name, const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult) {
  if (!Record::NameIsEmpty(name) && strncmp(baseName, name.baseName, baseNameLength) == 0 && baseNameLength == name.baseNameLength) {
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

Storage::RecordIterator & Storage::RecordIterator::operator++() {
  assert(m_recordStart);
  record_size_t size = StorageHelper::unalignedShort(m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = StorageHelper::unalignedShort(nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}
