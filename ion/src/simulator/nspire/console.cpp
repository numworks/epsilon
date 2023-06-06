#include <ion/console.h>
#include "main.h"
#include <kandinsky/ion_context.h>
#include <stdio.h>

namespace Ion {
namespace Console {

char readChar() {
  return 0;
}

void writeChar(char c) {
  // fxlibc conflicts with this
  #undef putchar
  KDIonContext::putchar(c);
}

bool transmissionDone() {
  return true;
}

}
}
