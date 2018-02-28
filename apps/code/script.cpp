#include "script.h"

namespace Code {

Script::Script(Record f) :
  Record(f)
{
}

bool Script::importationStatus() const {
  assert(!isNull());
  const char * body = read();
  return (body[0] == 1);
}

void Script::toggleImportationStatus() {
  assert(bodySize() >= 1);
  m_body[0] = importationStatus() ? 0 : 1;
}

const char * Script::readContent() const {
  const char * body = read();
  return body+k_importationStatusSize;
}

Script::ErrorStatus Script::writeContent(const char * data, size_t size) {
  int deltaSize = (int)size+k_importationStatusSize - (int)bodySize();
  if (Ion::Kallax::sharedKallax()->moveNextRecord(start(), deltaSize)) {
    *m_size += deltaSize;
    strlcpy(m_body+k_importationStatusSize, data, size);
    return ErrorStatus::None;
  } else {
    return ErrorStatus::NoEnoughSpaceAvailable;
  }
}

}
