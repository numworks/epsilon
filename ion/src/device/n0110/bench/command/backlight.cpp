#include "command.h"
#include <drivers/backlight.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Backlight(const char * input) {
  // Input must be of the form "0xAA" or "ON" or "OFF"
  if (strcmp(input, sON) == 0) {
    Backlight::init();
    reply(sOK);
    return;
  }
  if (strcmp(input, sOFF) == 0) {
    Backlight::shutdown();
    reply(sOK);
    return;
  }
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || input[4] != NULL) {
    reply(sSyntaxError);
    return;
  }
  uint32_t brightness = hexNumber(input+2);
  Backlight::setBrightness(brightness);
  reply(sOK);
}

}
}
}
}
