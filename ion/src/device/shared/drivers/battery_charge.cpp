#include "battery.h"
#include <drivers/config/battery.h>

namespace Ion {
namespace Device {
namespace Battery {

using namespace Regs;

bool isCharging() {
  return !Config::ChargingPin.group().IDR()->get(Config::ChargingPin.pin());
}

}
}
}
