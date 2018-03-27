#include <ion.h>
#include <string.h>
#include <assert.h>

Ion::Storage storage;

namespace Ion {

Storage::Record::Record(const char * name) {
  if (name == nullptr) {
    m_nameCRC32 = 0;
    return;
  }
  size_t lenght = strlen(name);
  size_t crc32InputSize = lenght*sizeof(char)/sizeof(uint32_t)+1;
  uint32_t * crc32Input = new uint32_t[crc32InputSize];
  memset(crc32Input, 0, crc32InputSize*sizeof(uint32_t));
  strlcpy((char *)crc32Input, name, lenght+1);
  assert((crc32InputSize*sizeof(uint32_t) & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  m_nameCRC32 = Ion::crc32(crc32Input, crc32InputSize);
  delete[] crc32Input;
}

Storage::Storage() :
  m_magicHeader(Magic),
  m_buffer(),
  m_magicFooter(Magic)
{
  assert(m_magicHeader == Magic);
  assert(m_magicFooter == Magic);
  // Set the size of the first record to 0
  overrideSizeAtPosition(m_buffer, 0);
}

size_t Storage::availableSize() {
  return k_storageSize-(endBuffer()-m_buffer)-sizeof(record_size_t);
}

Storage::Record::ErrorStatus Storage::createRecord(const char * name, const void * data, size_t size) {
  if (!nameCompliant(name)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  size_t recordSize = sizeOfRecord(name, size);
  if (recordSize >= k_maxRecordSize || recordSize > availableSize()) {
   return Record::ErrorStatus::NotEnoughSpaceAvailable;
  }
  if (isNameTaken(name)) {
    return Record::ErrorStatus::NameTaken;
  }
  // Find the end of data
  char * newRecord = endBuffer();
  // Fill totalSize
  newRecord += overrideSizeAtPosition(newRecord, (record_size_t)recordSize);
  // Fill name
  newRecord += overrideNameAtPosition(newRecord, name);
  // Fill data
  newRecord += overrideValueAtPosition(newRecord, data, size);
  // Next Record is null-sized
  overrideSizeAtPosition(newRecord, 0);
  return Record::ErrorStatus::None;
}

int Storage::numberOfRecordsWithExtension(const char * extension) {
  int count = 0;
  for (char * p : *this) {
    const char * name = nameOfRecordStarting(p);
    const char * ext = name+strlen(name)-strlen(extension);
    if (strcmp(ext, extension) == 0) {
      count++;
    }
  }
  return count;
}

Storage::Record Storage::recordWithExtensionAtIndex(const char * extension, int index) {
  int currentIndex = -1;
  const char * name = nullptr;
  for (char * p : *this) {
    const char * currentName = nameOfRecordStarting(p);
    const char * currentExtension = currentName+strlen(currentName)-strlen(extension);
    if (strcmp(currentExtension, extension) == 0) {
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

Storage::Record Storage::recordNamed(const char * name) {
  for (char * p : *this) {
    const char * currentName = nameOfRecordStarting(p);
    if (strcmp(currentName, name) == 0) {
      return Record(name);
    }
  }
  return Record();
}

const char * Storage::nameOfRecord(const Record record) {
  for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      return nameOfRecordStarting(p);
    }
  }
  return nullptr;
}

Storage::Record::ErrorStatus Storage::setNameOfRecord(Record record, const char * name) {
  if (!nameCompliant(name)) {
    return Record::ErrorStatus::NonCompliantName;
  }
  if (isNameTaken(name, &record)) {
    return Record::ErrorStatus::NameTaken;
  }
  size_t nameSize = strlen(name)+1;
  for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      size_t previousNameSize = strlen(nameOfRecordStarting(p))+1;
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      size_t newRecordSize = previousRecordSize-previousNameSize+nameSize;
      if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+sizeof(record_size_t)+previousNameSize, nameSize-previousNameSize)) {
        return Record::ErrorStatus::NotEnoughSpaceAvailable;
      }
      overrideSizeAtPosition(p, newRecordSize);
      overrideNameAtPosition(p+sizeof(record_size_t), name);
      return Record::ErrorStatus::None;
    }
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

Storage::Record::Data Storage::valueOfRecord(const Record record) {
 for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      const char * name = nameOfRecordStarting(p);
      record_size_t size = sizeOfRecordStarting(p);
      const void * value = valueOfRecordStarting(p);
      return {.buffer= value, .size= size-strlen(name)-1-sizeof(record_size_t)};
    }
  }
  return {.buffer= nullptr, .size= 0};
}

Storage::Record::ErrorStatus Storage::setValueOfRecord(Record record, Record::Data data) {
  for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      const char * name = nameOfRecordStarting(p);
      size_t newRecordSize = sizeOfRecord(name, data.size);
      if (newRecordSize >= k_maxRecordSize || !slideBuffer(p+previousRecordSize, newRecordSize-previousRecordSize)) {
        return Record::ErrorStatus::NotEnoughSpaceAvailable;
      }
      record_size_t nameSize = strlen(name)+1;
      overrideSizeAtPosition(p, newRecordSize);
      overrideValueAtPosition(p+sizeof(record_size_t)+nameSize, data.buffer, data.size);
      return Record::ErrorStatus::None;
    }
  }
  return Record::ErrorStatus::RecordDoesNotExist;
}

void Storage::destroyRecord(Record record) {
  for (char * p : *this) {
    Record currentRecord(nameOfRecordStarting(p));
    if (record == currentRecord) {
      record_size_t previousRecordSize = sizeOfRecordStarting(p);
      slideBuffer(p+previousRecordSize, -previousRecordSize);
    }
  }
}

Storage::record_size_t Storage::sizeOfRecordStarting(char * start) const {
  return *((record_size_t *)start);
}

const char * Storage::nameOfRecordStarting(char * start) const {
  return start+sizeof(record_size_t);
}

const void * Storage::valueOfRecordStarting(char * start) const {
  char * currentChar = start+sizeof(record_size_t);
  while (*currentChar != 0) {
    currentChar++;
  }
  return currentChar+1;
}

size_t Storage::overrideSizeAtPosition(char * position, record_size_t size) {
  *((record_size_t *)position) = size;
  return sizeof(record_size_t);
}

size_t Storage::overrideNameAtPosition(char * position, const char * name) {
  return strlcpy(position, name, strlen(name)+1)+1;
}

size_t Storage::overrideValueAtPosition(char * position, const void * data, record_size_t size) {
  memcpy(position, data, size);
  return size;
}

bool Storage::isNameTaken(const char * name, Record * recordToExclude) {
  Record r = Record(name);
  if (r == Record()) {
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

bool Storage::nameCompliant(const char * name) const {
  /* The name format is [a-z0-9_]*(\.)?[a-z0-9_]+ */
  bool dot = false;
  const char * currentChar = name;
  while (*currentChar != 0) {
    if (*currentChar == '.') {
      if (dot) {
        return false;
      } else {
        dot = true;
      }
    }
    if ((*currentChar >= 'a' && *currentChar <= 'z') || *currentChar == '_' || (*currentChar >= '0' && *currentChar <= '9') || *currentChar == '.') {
      currentChar++;
      continue;
    }
    return false;
  }
  return name != currentChar;
}

char * Storage::endBuffer() {
  char * currentBuffer = m_buffer;
  for (char * p : *this) {
    currentBuffer += sizeOfRecordStarting(p);
  }
  return currentBuffer;
}

size_t Storage::sizeOfRecord(const char * name, size_t dataSize) const {
  size_t nameSize = strlen(name)+1;
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
  record_size_t size = *((record_size_t *)m_recordStart);
  char * nextRecord = m_recordStart+size;
  record_size_t newRecordSize = *((record_size_t *)nextRecord);
  m_recordStart = (newRecordSize == 0 ? nullptr : nextRecord);
  return *this;
}

}
