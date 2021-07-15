#include "command.h"
#include <ion/usb.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void USBPlugged(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  if (Ion::USB::isPlugged()) {
    reply(sON);
  } else {
    reply(sOFF);
  }
}

}
}
}
}
