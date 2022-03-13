#include <ion.h>
#include <string.h>
#include <assert.h>
#include <new>
#include <ion/internal_storage.h>
#if ION_STORAGE_LOG
#include <iostream>
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
 * placement-newing the Storage into that area on first use. */

/* The InternalStorage is handle all records. Then the "normal" Storage handle
 * a trash */

constexpr char InternalStorage::expExtension[];
constexpr char InternalStorage::funcExtension[];
constexpr char InternalStorage::seqExtension[];
constexpr char InternalStorage::eqExtension[];

// RECORD

const char * InternalStorage::Record::fullName() const {
  return Storage::sharedStorage()->fullNameOfRecord(*this);
}
InternalStorage::Record::ErrorStatus InternalStorage::Record::setBaseNameWithExtension(const char * baseName, const char * extension) {
  return Storage::sharedStorage()->setBaseNameWithExtensionOfRecord(*this, baseName, extension);
}
InternalStorage::Record::ErrorStatus InternalStorage::Record::setName(const char * fullName) {
  return Storage::sharedStorage()->setFullNameOfRecord(*this, fullName);
}
InternalStorage::Record::Data InternalStorage::Record::value() const {
  return Storage::sharedStorage()->valueOfRecord(*this);
}
InternalStorage::Record::ErrorStatus InternalStorage::Record::setValue(Data data) {
  return Storage::sharedStorage()->setValueOfRecord(*this, data);
}
void InternalStorage::Record::destroy() {
  return Storage::sharedStorage()->destroyRecord(*this);
}

InternalStorage::Record::Record(const char * fullName) {
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

InternalStorage::Record::Record(const char * baseName, const char * extension) {
  if (baseName == nullptr) {
    assert(extension == nullptr);
    m_fullNameCRC32 = 0;
    return;
  }
  new (this) Record(baseName, strlen(baseName), extension, strlen(extension));
}


#if ION_STORAGE_LOG

void InternalStorage::Record::log() {
  std::cout << "Name: " << fullName() << std::endl;
  std::cout << "        Value (" << value().size << "): " << (char *)value().buffer << "\n\n" << std::endl;
}
#endif

uint32_t InternalStorage::Record::checksum() {
  uint32_t crc32Results[2];
  crc32Results[0] = m_fullNameCRC32;
  Data data = value();
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)data.buffer, data.size);
  return Ion::crc32Word(crc32Results, 2);
}

InternalStorage::Record::Record(const char * basename, int basenameLength, const char * extension, int extensionLength) {
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
void InternalStorage::log() {
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    Record(currentName).log();
  }
}
#endif

size_t InternalStorage::availableSize() {
  /* TODO maybe do: availableSize(char ** endBuffer) to get the endBuffer if it
   * is needed after calling availableSize */
  assert(k_storageSize >= (endBuffer() - m_buffer) + sizeof(record_size_t));
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

size_t InternalStorage::putAvailableSpaceAtEndOfRecord(InternalStorage::Record r) {
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

void InternalStorage::getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace) {
  char * p = pointerOfRecord(r);
  size_t previousRecordSize = sizeOfRecordStarting(p);
  char * nextRecord = p + previousRecordSize;
  memmove(nextRecord - recordAvailableSpace,
      nextRecord,
      m_buffer + k_storageSize - nextRecord);
  overrideSizeAtPosition(p, (record_size_t)(previousRecordSize - recordAvailableSpace));
}

uint32_t InternalStorage::checksum() {
  return Ion::crc32Byte((const uint8_t *) m_buffer, endBuffer()-m_buffer);
}

void InternalStorage::notifyChangeToDelegate(const Record record) const {
  m_lastRecordRetrieved = Record(nullptr);
  m_lastRecordRetrievedPointer = nullptr;
  if (m_delegate != nullptr) {
    m_delegate->storageDidChangeForRecord(record);
  }
}

InternalStorage::Record::ErrorStatus InternalStorage::notifyFullnessToDelegate() const {
  if (m_delegate != nullptr) {
    m_delegate->storageIsFull();
  }
  return Record::ErrorStatus::NotEnoughSpaceAvailable;
}

InternalStorage::Record::ErrorStatus InternalStorage::createRecordWithFullName(const char * fullName, const void * data, size_t size) {
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
  Record r = Record(fullName);
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  return Record::ErrorStatus::None;
}

InternalStorage::Record::ErrorStatus InternalStorage::createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size) {
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
  Record r = Record(fullNameOfRecordStarting(newRecordAddress));
  notifyChangeToDelegate(r);
  m_lastRecordRetrieved = r;
  m_lastRecordRetrievedPointer = newRecordAddress;
  return Record::ErrorStatus::None;
}

int InternalStorage::numberOfRecordsWithExtension(const char * extension) {
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

int InternalStorage::numberOfRecords() {
  int count = 0;
  for (char * p : *this) {
    const char * name = fullNameOfRecordStarting(p);
    count++;
  }
  return count;
}

InternalStorage::Record InternalStorage::recordAtIndex(int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  char * recordAddress = nullptr;
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    currentIndex++;
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

InternalStorage::Record InternalStorage::recordWithExtensionAtIndex(const char * extension, int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  size_t extensionLength = strlen(extension);
  char * recordAddress = nullptr;
  for (char * p : *this) {
    const char * currentName = fullNameOfRecordStarting(p);
    if (FullNameHasExtension(currentName, extension, extensionLength)) {
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

InternalStorage::Record InternalStorage::recordNamed(const char * fullName) {
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

InternalStorage::Record InternalStorage::recordBaseNamedWithExtension(const char * baseName, const char * extension) {
  const char * extensions[1] = {extension};
  return recordBaseNamedWithExtensions(baseName, extensions, 1);
}

InternalStorage::Record InternalStorage::recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions) {
  return privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions);
}

const char * InternalStorage::extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions) {
  const char * result = nullptr;
  privateRecordAndExtensionOfRecordBaseNamedWithExtensions(baseName, extensions, numberOfExtensions, &result, baseNameLength);
  return result;
}

void InternalStorage::destroyAllRecords() {
  overrideSizeAtPosition(m_buffer, 0);
  notifyChangeToDelegate();
}

void InternalStorage::destroyRecordWithBaseNameAndExtension(const char * baseName, const char * extension) {
  recordBaseNamedWithExtension(baseName, extension).destroy();
}

void InternalStorage::destroyRecordsWithExtension(const char * extension) {
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

InternalStorage::InternalStorage() :
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

// PRIVATE

const char * InternalStorage::fullNameOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    return fullNameOfRecordStarting(p);
  }
  return nullptr;
}

InternalStorage::Record::ErrorStatus InternalStorage::setFullNameOfRecord(const Record record, const char * fullName) {
  if (!FullNameCompliant(fullName)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  if (isFullNameTaken(fullName, &record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = strlen(fullName) + 1;
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    overrideFullNameAtPosition(p+sizeof(record_size_t), fullName);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

InternalStorage::Record::ErrorStatus InternalStorage::setBaseNameWithExtensionOfRecord(Record record, const char * baseName, const char * extension) {
  if (isBaseNameWithExtensionTaken(baseName, extension, &record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = sizeOfBaseNameAndExtension(baseName, extension);
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    size_t previousNameSize = strlen(fullNameOfRecordStarting(p))+1;
    record_size_t previousRecordSize = sizeOfRecordStarting(p);
    size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
    if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
      return notifyFullnessToDelegate();
    }
    overrideSizeAtPosition(p, newRecordSize);
    char * fullNamePosition = p + sizeof(record_size_t);
    overrideBaseNameWithExtensionAtPosition(fullNamePosition, baseName, extension);
    // Recompute the CRC32
    record = Record(fullNamePosition);
    notifyChangeToDelegate(record);
    m_lastRecordRetrieved = record;
    m_lastRecordRetrievedPointer = p;
    return Record::ErrorStatus::None;
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

InternalStorage::Record::Data InternalStorage::valueOfRecord(const Record record) {
  char * p = pointerOfRecord(record);
  if (p != nullptr) {
    const char * fullName = fullNameOfRecordStarting(p);
    record_size_t size = sizeOfRecordStarting(p);
    const void * value = valueOfRecordStarting(p);
    return {.buffer = value, .size = size-strlen(fullName)-1-sizeof(record_size_t)};
  }
  return {.buffer= nullptr, .size= 0};
}

InternalStorage::Record::ErrorStatus InternalStorage::setValueOfRecord(Record record, Record::Data data) {
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

void InternalStorage::destroyRecord(Record record) {
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

char * InternalStorage::pointerOfRecord(const Record record) const {
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

InternalStorage::record_size_t InternalStorage::sizeOfRecordStarting(char * start) const {
  return StorageHelper::unalignedShort(start);
}

const char * InternalStorage::fullNameOfRecordStarting(char * start) const {
  return start+sizeof(record_size_t);
}

const void * InternalStorage::valueOfRecordStarting(char * start) const {
  char * currentChar = start+sizeof(record_size_t);
  size_t fullNameLength = strlen(currentChar);
  return currentChar+fullNameLength+1;
}

size_t InternalStorage::overrideSizeAtPosition(char * position, record_size_t size) {
  StorageHelper::writeUnalignedShort(size, position);
  return sizeof(record_size_t);
}

size_t InternalStorage::overrideFullNameAtPosition(char * position, const char * fullName) {
  return strlcpy(position, fullName, strlen(fullName)+1) + 1;
}

size_t InternalStorage::overrideBaseNameWithExtensionAtPosition(char * position, const char * baseName, const char * extension) {
  size_t result = strlcpy(position, baseName, strlen(baseName)+1); // strlcpy copies the null terminating char
  assert(UTF8Decoder::CharSizeOfCodePoint(k_dotChar) == 1);
  *(position+result) = k_dotChar; // Replace the null terminating char with a dot
  result++;
  result += strlcpy(position+result, extension, strlen(extension)+1);
  return result+1;
}

size_t InternalStorage::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool InternalStorage::isFullNameTaken(const char * fullName, const Record * recordToExclude) {
  Record r = Record(fullName);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool InternalStorage::isBaseNameWithExtensionTaken(const char * baseName, const char * extension, Record * recordToExclude) {
  Record r = Record(baseName, extension);
  return isNameOfRecordTaken(r, recordToExclude);
}

bool InternalStorage::isNameOfRecordTaken(Record r, const Record * recordToExclude) {
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

bool InternalStorage::FullNameCompliant(const char * fullName) {
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

bool InternalStorage::FullNameHasExtension(const char * fullName, const char * extension, size_t extensionLength) {
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

char * InternalStorage::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t InternalStorage::sizeOfBaseNameAndExtension(const char * baseName, const char * extension) const {
  // +1 for the dot and +1 for the null terminating char
  return strlen(baseName)+1+strlen(extension)+1;
}

size_t InternalStorage::sizeOfRecordWithBaseNameAndExtension(const char * baseName, const char * extension, size_t dataSize) const {
  return sizeOfBaseNameAndExtension(baseName, extension) + dataSize + sizeof(record_size_t);
}

size_t InternalStorage::sizeOfRecordWithFullName(const char * fullName, size_t dataSize) const {
  size_t nameSize = strlen(fullName)+1;
  return nameSize+dataSize+sizeof(record_size_t);
}

bool InternalStorage::slideBuffer(char * position, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  memmove(position+delta, position, endBuffer()+sizeof(record_size_t)-position);
  return true;
}

InternalStorage::Record InternalStorage::privateRecordAndExtensionOfRecordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult, int baseNameLength) {
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

InternalStorage::RecordIterator & InternalStorage::RecordIterator::operator++() {
  assert(m_recordStart);
  record_size_t size = StorageHelper::unalignedShort(m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = StorageHelper::unalignedShort(nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}
