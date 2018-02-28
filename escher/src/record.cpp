#include <escher/record.h>
#include <escher/kallax.h>
#include <string.h>
#include <assert.h>

Record::Record(size_t * size, char * name, Type type, char * body) :
  m_body(body),
  m_size(size),
  m_name(name),
  m_type(type)
{
}

bool Record::isNull() const {
  if (m_type == Type::Null) {
    assert(m_size == nullptr);
    return true;
  }
  return false;
}


const char * Record::name() const {
  return m_name;
}

Record::ErrorStatus Record::rename(const char * newName) {
  if (Kallax::sharedKallax()->isNameTaken(newName, m_type)) {
    return ErrorStatus::NameTaken;
  }
  if (strlen(newName) >= k_nameSize) {
    return ErrorStatus::NameTooLong;
  }
  strlcpy(m_name, newName, k_nameSize);
  return ErrorStatus::None;
}

const char * Record::read() const {
  return m_body;
}

Record::ErrorStatus Record::write(const char * data, size_t size) {
  int deltaSize = (int)size - (int)bodySize();
  // TODO: if this fails because deltaSize is too big, return an error?
  if (Kallax::sharedKallax()->moveNextRecord(start(), deltaSize)) {
    *m_size += deltaSize;
    strlcpy(m_body, data, size);
    return ErrorStatus::None;
  }
  return ErrorStatus::NoEnoughSpaceAvailable;
}

Record::Type Record::type() {
  return m_type;
}

void Record::remove() {
  Kallax::sharedKallax()->moveNextRecord(start(), -*(m_size));
}

char * Record::start() {
  return m_name - k_typeSize - k_sizeSize;
}

size_t Record::bodySize() const {
  return *m_size - k_nameSize - k_typeSize - k_sizeSize;
}
