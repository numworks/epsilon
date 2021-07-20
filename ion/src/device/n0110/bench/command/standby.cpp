#include "command.h"
#include <drivers/power_suspend.h>
#include <assert.h>

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
  /* We can use Power::standby as the OnOff key is not down in the bench
   * (standby wait until OnOff key is released before entering standby mode). */
  Power::standby();
  assert(false);
}

}
}
}
}
