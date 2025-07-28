#include <ion/console.h>
#include <ion/display.h>
#include <stdio.h>

#include "window.h"

namespace Ion {
namespace Console {

char readChar() {
  if (Simulator::Window::isHeadless()) {
    return getchar();
  } else {
    return 0;
  }
}

void writeChar(char c) {
  putchar(c);
  fflush(stdout);
}

bool clear() {
  // No screen is expected to be cleared on simulator
  return false;
}

void startReception() {}

bool transmissionDone() { return true; }

}  // namespace Console
}  // namespace Ion
