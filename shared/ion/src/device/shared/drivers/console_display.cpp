#include <ion/console.h>
#include <ion/display.h>

namespace Ion {
namespace Console {

char readChar() { return 0; }

void writeChar(char c) { Ion::Display::Context::Putchar(c); }

bool clear() {
  Ion::Display::Context::Clear();
  return true;
}

bool transmissionDone() { return true; }

}  // namespace Console
}  // namespace Ion
