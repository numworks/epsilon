#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/reset.h>

namespace Ion {
namespace Power {

void standby() {
  waitUntilOnOffKeyReleased();
  standbyConfiguration();
  Device::Board::shutdown();
  enterLowPowerMode();
  Device::Reset::core();
}

}
}
