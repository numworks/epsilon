#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Suspend(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
  Ion::msleep(100);
  Ion::Power::suspend();
}

}
}
}
}
