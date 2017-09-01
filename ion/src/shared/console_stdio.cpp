#include <ion/console.h>
#include <stdio.h>

namespace Ion {
namespace Console {

char readChar() {
  return getchar();
}

void writeChar(char c) {
  putchar(c);
  fflush(stdout);
}

}
}
