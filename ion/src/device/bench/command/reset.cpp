#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/reset.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Reset(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  /* We jump instead of calling "core()", because the '6' button is pressed on
   * the bench and calling "core" would reboot on the bootloader. */
  reply(sOK);
  Ion::Device::Reset::jump();
}

}
}
}
}
