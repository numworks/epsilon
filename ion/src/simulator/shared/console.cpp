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
  if (Simulator::Window::isHeadless()) {
    putchar(c);
    fflush(stdout);
  } else {
    KDIonContext::putchar(c);
  }
}

bool transmissionDone() {
  return true;
}

}
}
