#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/led.h>
#include <kandinsky.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must be of the form "XX,YY,STRING"
void Print(const char * input) {
  if (input == nullptr || !isHex(input[0]) || !isHex(input[1]) || input[2] != ',' || !isHex(input[3]) || !isHex(input[4]) || input[5] != ',') {
    reply(sKO);
    return;
  }

  char x = hexNumber(input, 2);
  char y = hexNumber(input+3, 2);

  KDContext * ctx = KDIonContext::sharedContext();
  ctx->drawString(input+6, KDPoint(x, y));

  reply(sOK);
}

}
}
}
}
