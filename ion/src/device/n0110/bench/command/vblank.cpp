#include "command.h"
#include <apps/shared/post_and_hardware_tests.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void VBlank(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(Shared::POSTAndHardwareTests::VBlankOK() ? sOK : sKO);
}

}
}
}
}
