#include "command.h"
#include <ion.h>
#include <ion/src/device/display.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must be of the form "0xAABBCC" or "ON" or "OFF"
void Display(const char * input) {

  if (strcmp(input, sON) == 0) {
    Ion::Display::Device::init();
    reply(sOK);
    return;
  }
  if (strcmp(input, sOFF) == 0) {
    Ion::Display::Device::shutdown();
    reply(sOK);
    return;
  }
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || !isHex(input[4]) || !isHex(input[5]) || !isHex(input[6]) || !isHex(input[7]) || input[8] != NULL) {
    reply(sSyntaxError);
    return;
  }

  /* We fill the screen with a color and return OK if we read that color back everywhere. */

  KDColor c = KDColor::RGB24(hexNumber(input));

  constexpr int stampHeight = 10;
  constexpr int stampWidth = 10;
  static_assert(Ion::Display::Width % stampWidth == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % stampHeight == 0, "Stamps must tesselate the display");
  static_assert(stampHeight % 2 == 0 || stampWidth % 2 == 0, "Even number of XOR needed.");

  KDColor stamp[stampWidth*stampHeight];
  for (int i=0;i<stampWidth*stampHeight; i++) {
    stamp[i] = c;
  }

  for (int i=0; i<Ion::Display::Width/stampWidth; i++) {
    for (int j=0; j<Ion::Display::Height/stampHeight; j++) {
      Ion::Display::pushRect(KDRect(i*stampWidth, j*stampHeight, stampWidth, stampHeight), stamp);
    }
  }

  for (int i=0; i<Ion::Display::Width/stampWidth; i++) {
    for (int j=0; j<Ion::Display::Height/stampHeight; j++) {
      for (int i=0;i<stampWidth*stampHeight; i++) {
        stamp[i] = KDColorBlack;
      }
      Ion::Display::pullRect(KDRect(i*stampWidth, j*stampHeight, stampWidth, stampHeight), stamp);
      for (int i=0;i<stampWidth*stampHeight; i++) {
        if (stamp[i] != c) {
          reply(sKO);
          return;
        }
      }
    }
  }

  reply(sOK);
}

}
}
}
}
