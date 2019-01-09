#include "command.h"

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Exit(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply(sOK);
}

}
}
}
}
