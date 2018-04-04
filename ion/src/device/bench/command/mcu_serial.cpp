#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void MCUSerial(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  char response[11+Ion::SerialNumberLength + 1] = {'M', 'C', 'U', '_', 'S', 'E', 'R', 'I', 'A', 'L', '=', 0};
  Ion::getSerialNumber(response+11);
  reply(response);
}

}
}
}
}
