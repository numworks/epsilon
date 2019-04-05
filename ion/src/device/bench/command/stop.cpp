#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Stop(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Ion::Timing::msleep(100);
  Ion::Power::suspend(); //Ion::Power::stop(); TODO Decomment once the method exists
}

}
}
}
}
