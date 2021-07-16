#include "command.h"
#include <ion/post_and_hardware_tests.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void VBlank(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(POSTAndHardwareTests::VBlankOK() ? sOK : sKO);
}

}
}
}
}
