#include "console.h"
#include <ion/console.h>
#include <kandinsky/ion_context.h>

namespace Ion {
namespace Console {

char readChar() {
  return 0;
}

void writeChar(char c) {
  KDIonContext::putchar(c);
}

bool transmissionDone() {
  return true;
}

}
}

namespace Ion {
namespace Device {
namespace Console {

void init() {
}

void shutdown() {
}

bool peerConnected() {
  return false;
}

}
}
}
