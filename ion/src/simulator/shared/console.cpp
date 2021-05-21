#include <ion/console.h>
#include "window.h"
#include <kandinsky/ion_context.h>
#include <stdio.h>

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

bool transmissionDone() {
  return true;
}

}
}
