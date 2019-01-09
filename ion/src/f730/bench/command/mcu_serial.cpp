#include "command.h"
#include <ion.h>
#include "../../device.h"

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void MCUSerial(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  char response[11 + Ion::Device::SerialNumberLength + 1] = {'M', 'C', 'U', '_', 'S', 'E', 'R', 'I', 'A', 'L', '=', 0};
  Ion::Device::copySerialNumber(response + 11);
  reply(response);
}

}
}
}
}
