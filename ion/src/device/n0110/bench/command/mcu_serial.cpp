#include "command.h"
#include <ion.h>
#include <drivers/serial_number.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void MCUSerial(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  char response[11 + k_serialNumberLength + 1] = {'M', 'C', 'U', '_', 'S', 'E', 'R', 'I', 'A', 'L', '=', 0};
  SerialNumber::copy(response + 11);
  reply(response);
}

}
}
}
}
