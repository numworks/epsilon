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

bool Script::nameCompliant(const char * name) {
  /* The name format is [a-z0-9_\.]+ */
  const char * currentChar = name;
  while (*currentChar != 0) {
    if ((*currentChar >= 'a' && *currentChar <= 'z') || *currentChar == '_' || (*currentChar >= '0' && *currentChar <= '9') || *currentChar == '.') {
      currentChar++;
      continue;
    }
    return false;
  }
  return name != currentChar;
}

}
