#include <escher/file.h>
#include <escher/file_system.h>
#include <string.h>
#include <assert.h>

File::File(size_t * size, char * name, Type type, char * body) :
  m_body(body),
  m_size(size),
  m_name(name),
  m_type(type)
{
}

bool File::isNull() const {
  if (m_type == Type::Null) {
    assert(m_size == nullptr);
    return true;
  }
  return false;
}


const char * File::name() const {
  return m_name;
}

File::ErrorStatus File::rename(const char * newName) {
  if (FileSystem::sharedFileSystem()->isNameTaken(newName, m_type)) {
    return ErrorStatus::NameTaken;
  }
  if (strlen(newName) >= k_nameSize) {
    return ErrorStatus::NameTooLong;
  }
  strlcpy(m_name, newName, k_nameSize);
  return ErrorStatus::None;
}

const char * File::read() const {
  return m_body;
}

File::ErrorStatus File::write(const char * data, size_t size) {
  int deltaSize = (int)size - (int)bodySize();
  // TODO: if this fails because deltaSize is too big, return an error?
  if (FileSystem::sharedFileSystem()->moveNextFile(start(), deltaSize)) {
    *m_size += deltaSize;
    strlcpy(m_body, data, size);
    return ErrorStatus::None;
  }
  return ErrorStatus::NoEnoughSpaceAvailable;
}

File::Type File::type() {
  return m_type;
}

void File::remove() {
  FileSystem::sharedFileSystem()->moveNextFile(start(), -*(m_size));
}

char * File::start() {
  return m_name - k_typeSize - k_sizeSize;
}

size_t File::bodySize() const {
  return *m_size - k_nameSize - k_typeSize - k_sizeSize;
}
