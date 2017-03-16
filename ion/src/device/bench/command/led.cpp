#include "command.h"
#include <ion.h>
#include <ion/src/device/led.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must be of the form "0xAABBCC" or "ON" or "OFF"
void LED(const char * input) {
  if (strcmp(input, sON) == 0) {
    Ion::LED::Device::init();
    Ion::Console::writeLine(sOK);
    return;
  }
  if (strcmp(input, sOFF) == 0) {
    Ion::LED::Device::shutdown();
    Ion::Console::writeLine(sOK);
    return;
  }
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || !isHex(input[4]) || !isHex(input[5]) || !isHex(input[6]) || !isHex(input[7]) || input[8] != NULL) {
    Ion::Console::writeLine(sSyntaxError);
    return;
  }
  uint32_t hexColor = hexNumber(input+2);
  KDColor ledColor = KDColor::RGB24(hexColor);
  Ion::LED::setColor(ledColor);
  Ion::Console::writeLine(sOK);
}

}
}
}
}
