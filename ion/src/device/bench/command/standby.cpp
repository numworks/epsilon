#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Standby(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Ion::Timing::msleep(100);
  //Ion::Power::standby(); TODO Decomment once the method exists
}

}
}
}
}
