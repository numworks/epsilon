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

}
