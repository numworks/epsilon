#include <ion/console.h>
#include <omg/print.h>

#include <ostream>

namespace std {

ostream& ostream::operator<<(const char* str) {
  Ion::Console::writeLine(str, false);
  return *this;
}

ostream& ostream::operator<<(int i) {
  constexpr char bufferSize = 32;
  char buffer[bufferSize];
  if (i < 0) {
    Ion::Console::writeChar('-');
  }
  int p = OMG::Print::IntLeft(i, buffer, bufferSize);
  buffer[p] = 0;
  Ion::Console::writeLine(buffer, false);
  return *this;
}

}  // namespace std
