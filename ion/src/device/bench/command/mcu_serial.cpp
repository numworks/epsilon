#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/serial_number.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void MCUSerial(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  char response[11 + Ion::Device::SerialNumber::Length + 1] = {'M', 'C', 'U', '_', 'S', 'E', 'R', 'I', 'A', 'L', '=', 0};
  Ion::Device::SerialNumber::copy(response + 11);
  reply(response);
}

}
}
}
}
