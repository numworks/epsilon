#include "script.h"

namespace Code {

Script::Script(Record f) :
  Record(f)
{
}

bool Script::importationStatus() const {
  assert(!isNull());
  Data d = value();
  return (((char *)d.buffer)[0] == 1);
}

void Script::toggleImportationStatus() {
  Data d = value();
  ((char *)d.buffer)[0] = (((char *)d.buffer)[0] == 1 ? 0 : 1);
  setValue(d);
}

const char * Script::readContent() const {
  assert(!isNull());
  Data d = value();
  return (const char *)d.buffer+k_importationStatusSize;
}

Ion::Storage::Record::ErrorStatus Script::writeContent(const char * data, size_t size) {
  // TODO: could we avoid this useless allocation?
  char * buffer = new char[size+k_importationStatusSize];
  strlcpy(buffer+1, data, size);
  buffer[0] = importationStatus() ? 1 : 0;
  ErrorStatus e = setValue({.buffer= buffer, .size = size+k_importationStatusSize});
  delete[] buffer;
  return e;
}

}
