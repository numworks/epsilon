#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Charge(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  if (Ion::Battery::isCharging()) {
    reply("CHARGE=ON");
  } else {
    reply("CHARGE=OFF");
  }
}

}
}
}
}
