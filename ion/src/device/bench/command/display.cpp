#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/display.h>
#include <poincare/integer.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must be of the form "0xAABBCC" or "ON" or "OFF"
void Display(const char * input) {

  if (strcmp(input, sON) == 0) {
    Ion::Device::Display::init();
    reply(sOK);
    return;
  }
  if (strcmp(input, sOFF) == 0) {
    Ion::Device::Display::shutdown();
    reply(sOK);
    return;
  }
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || !isHex(input[4]) || !isHex(input[5]) || !isHex(input[6]) || !isHex(input[7]) || input[8] != NULL) {
    reply(sSyntaxError);
    return;
  }

  /* We fill the screen with a color and return OK if we read that color back everywhere. */

  KDColor c = KDColor::RGB24(hexNumber(input));

  int numberOfInvalidPixels = Ion::Display::displayUniformTilingSize10(c);

  char response[16] = {'D', 'E', 'L', 'T', 'A', '='};
  Poincare::Integer(numberOfInvalidPixels).serialize(response+6, sizeof(response)-6);
  reply(response);
}

}
}
}
}
