#include "command.h"
#include <ion.h>
#include <ion/src/device/backlight.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Backlight(const char * input) {
  // Input must be of the form "0xAA" or "ON" or "OFF"
  if (strcmp(input, sON) == 0) {
    Ion::Backlight::Device::init();
    reply(sOK);
    return;
  }
  if (strcmp(input, sOFF) == 0) {
    Ion::Backlight::Device::shutdown();
    reply(sOK);
    return;
  }
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || input[4] != NULL) {
    reply(sSyntaxError);
    return;
  }
  uint32_t brightness = hexNumber(input+2);
  Ion::Backlight::setBrightness(brightness);
  reply(sOK);
}

}
}
}
}
