#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Keyboard(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  char result[9+Ion::Keyboard::NumberOfKeys+1] = { 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D', '=' };
  for (uint8_t i=0; i<Ion::Keyboard::NumberOfKeys; i++) {
    result[9+i] = Ion::Keyboard::keyDown((Ion::Keyboard::Key)i) ? '1' : '0';
  }
  result[9+Ion::Keyboard::NumberOfKeys] = 0;
  reply(result);
}

}
}
}
}
