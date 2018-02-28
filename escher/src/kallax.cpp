#include <escher/kallax.h>
#include <string.h>
#include <assert.h>

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

#ifndef FORCE_LINK
#define FORCE_LINK
#endif

Kallax f;
constexpr KallaxInfo HEADER_SECTION FORCE_LINK kallax_infos((uint32_t)(&f));

Kallax::Kallax() :
  m_dataHeader(Magic),
  m_data(),
  m_dataFooter(Magic)
{
  size_t * p = (size_t *)m_data;
  p[0] = 0;
}

Kallax * Kallax::sharedKallax() {
  return &f;
}

int Kallax::numberOfRecordOfType(Record::Type type) {
  assert(m_dataHeader == Magic);
  assert(m_dataFooter == Magic);
  int count = 0;
  char * currentPointer = m_data;
  size_t size = sizeOfRecordStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfRecordStarting(currentPointer) == type) {
      count++;
    }
    currentPointer += size;
    size = sizeOfRecordStarting(currentPointer);
  }
  return count;
}

Record Kallax::recordOfTypeAtIndex(Record::Type type, int index) {
  int currentIndex = -1;
  char * currentPointer = m_data;
  size_t size = sizeOfRecordStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfRecordStarting(currentPointer) == type) {
      currentIndex++;
      if (currentIndex == index) {
        break;
      }
    }
    currentPointer += size;
    size = sizeOfRecordStarting(currentPointer);
  }
  return Record(sizeAddressOfRecordStarting(currentPointer), nameOfRecordStarting(currentPointer), type, bodyOfRecordStarting(currentPointer));
}

Record Kallax::getRecord(Record::Type type, const char * name) {
  for (int i = 0; i < numberOfRecordOfType(type); i++) {
    Record currentRecord = recordOfTypeAtIndex(type, i);
    if (strcmp(currentRecord.name(), name) == 0) {
      return currentRecord;
    }
  }
  return Record();
}

Record Kallax::addRecord(const char * name, Record::Type type, const char * body) {
  // assert name is short enough and there is enough space to add the record
  assert(strlen(name) < Record::k_nameSize);
  assert(availableSize() >= sizeOfRecordWithBody(body));
  // Find the end of data
  char * currentPointer = m_data;
  size_t size = sizeOfRecordStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    currentPointer += size;
    size = sizeOfRecordStarting(currentPointer);
  }
  size_t recordSize = sizeOfRecordWithBody(body);
  // Fill totalSize
  *((size_t *)currentPointer) = recordSize;
  // Fill type
  *(currentPointer+Record::k_sizeSize) = (uint8_t)type;
  // Fill name
  strlcpy(currentPointer+Record::k_sizeSize+Record::k_typeSize, name, Record::k_nameSize);
  // Fill body
  strlcpy(currentPointer+Record::k_sizeSize+Record::k_typeSize+Record::k_nameSize, body, strlen(body)+1);
  char * nextPointer = currentPointer + recordSize;
  *((size_t *)nextPointer) = 0;
  return Record(sizeAddressOfRecordStarting(currentPointer), nameOfRecordStarting(currentPointer), type, bodyOfRecordStarting(currentPointer));
}

char * Kallax::lastUsedData() {
  size_t usedSize = 0;
  char * currentPointer = m_data;
  size_t size = sizeOfRecordStarting(currentPointer);
  while (size != 0  && currentPointer < m_data + k_totalSize) {
    usedSize += size;
    currentPointer += size;
    size = sizeOfRecordStarting(currentPointer);
  }
  return currentPointer + Record::k_sizeSize;
}


size_t Kallax::availableSize() {
  return k_totalSize-(lastUsedData()-m_data);
}

bool Kallax::isNameTaken(const char * name, Record::Type type) {
  char * currentPointer = m_data;
  size_t size = sizeOfRecordStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfRecordStarting(currentPointer) == type && strcmp(nameOfRecordStarting(currentPointer), name) == 0) {
      return true;
    }
    currentPointer += size;
    size = sizeOfRecordStarting(currentPointer);
  }
  return false;
}

bool Kallax::moveNextRecord(char * start, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  char * nextRecord = start + sizeOfRecordStarting(start);
  memmove(nextRecord+delta, nextRecord, lastUsedData()-nextRecord);
  return true;
}

size_t * Kallax::sizeAddressOfRecordStarting(char * start) const {
  return (size_t *)start;
}

size_t Kallax::sizeOfRecordStarting(char * start) const {
  if (start >= m_data + k_totalSize) {
    return 0;
  }
  return *(sizeAddressOfRecordStarting(start));
}

Record::Type Kallax::typeOfRecordStarting(char * start) const {
   return (Record::Type)*((uint8_t *)start+Record::k_sizeSize);
}

char * Kallax::nameOfRecordStarting(char * start) {
  return start+Record::k_sizeSize+Record::k_typeSize;
}

char * Kallax::bodyOfRecordStarting(char * start) {
  return start+Record::k_sizeSize+Record::k_typeSize+Record::k_nameSize;
}

size_t Kallax::sizeOfRecordWithBody(const char * body) const {
  return Record::k_sizeSize+Record::k_typeSize+Record::k_nameSize+strlen(body)+1;
}
