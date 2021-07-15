#include "command.h"
#include <drivers/battery.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Charge(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  if (Battery::isCharging()) {
    reply("CHARGE=ON");
  } else {
    reply("CHARGE=OFF");
  }
}

}
}
}
}
