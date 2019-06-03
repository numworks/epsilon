#include "command.h"
#include <apps/shared/post_and_hardware_tests.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void LCDData(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  /* TODO: Find better place for LCDDataOK than
   * apps/shared/post_and_hardware_tests. Problem is it needs Kandinsky so Ion
   * might not be the best place. Maybe move the bench out of Ion? */
  reply(Shared::POSTAndHardwareTests::LCDDataOK() ? sOK : sKO);
}

}
}
}
}
