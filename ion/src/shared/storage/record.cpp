#include <ion.h>
#if ION_STORAGE_LOG
#include<iostream>
#endif

namespace Ion {

namespace Storage {

Record::Name Record::CreateRecordNameFromFullName(const char * fullName) {
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

Record::Name Record::CreateRecordNameFromBaseNameAndExtension(const char * baseName, const char * extension) {
  return Name({baseName, strlen(baseName), extension});
}

size_t Record::SizeOfName(Record::Name name) {
  // +1 for the dot and +1 for the null terminating char.
  return name.baseNameLength + 1 + strlen(name.extension) + 1;
}

bool Record::NameIsEmpty(Name name) {
  return name.baseName == nullptr || name.extension == nullptr;
}

Record::Record(Record::Name name) {
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

Record::Record(const char * fullName) : Record(CreateRecordNameFromFullName(fullName)) {}

Record::Record(const char * baseName, const char * extension) : Record(CreateRecordNameFromBaseNameAndExtension(baseName, extension)) {}


#if ION_STORAGE_LOG

void Record::log() {
  std::cout << "Name: " << fullName() << std::endl;
  std::cout << "        Value (" << value().size << "): " << (char *)value().buffer << "\n\n" << std::endl;
}
#endif

uint32_t Record::checksum() {
  uint32_t crc32Results[2];
  crc32Results[0] = m_fullNameCRC32;
  Data data = value();
  crc32Results[1] = Ion::crc32Byte((const uint8_t *)data.buffer, data.size);
  return Ion::crc32Word(crc32Results, 2);
}


Record::Name Record::name() const {
  return Storage::FileSystem::sharedFileSystem()->nameOfRecord(*this);
}

const char * Record::fullName() const {
  // The baseName points towards the start of the fullName
  return Storage::FileSystem::sharedFileSystem()->nameOfRecord(*this).baseName;
}

Record::Data Record::value() const {
  return Storage::FileSystem::sharedFileSystem()->valueOfRecord(*this);
}

Record::ErrorStatus Record::setValue(Data data) {
  return Storage::FileSystem::sharedFileSystem()->setValueOfRecord(*this, data);
}

void Record::destroy() {
  return Storage::FileSystem::sharedFileSystem()->destroyRecord(*this);
}

Record::ErrorStatus Record::SetBaseNameWithExtension(Record * record, const char * baseName, const char * extension) {
  Name name = CreateRecordNameFromBaseNameAndExtension(baseName, extension);
  return Storage::FileSystem::sharedFileSystem()->setNameOfRecord(record, name);
}

Record::ErrorStatus Record::SetFullName(Record * record, const char * fullName) {
  Name name = CreateRecordNameFromFullName(fullName);
  return Storage::FileSystem::sharedFileSystem()->setNameOfRecord(record, name);
}

}

}
