#include <ion/battery.h>
#include "battery.h"

namespace Ion {
namespace Battery {

bool isCharging() {
  return !Device::ChargingGPIO.IDR()->get(Device::ChargingPin);
}

}
}

namespace Ion {
namespace Battery {
namespace Device {

void init() {
  /* The BAT_CHRG pin is connected to the Li-Po charging IC. That pin uses an
   * open-drain output. Open-drain output are either connected to ground or left
   * floating. To interact with such an output, our input must therefore be
   * pulled up. */
  ChargingGPIO.MODER()->setMode(ChargingPin, GPIO::MODER::Mode::Input);
  ChargingGPIO.PUPDR()->setPull(ChargingPin, GPIO::PUPDR::Pull::Up);
}

}
}
}

/*
 *
 * RAIN = External input impedance should be <= to
 * (k-0.5)/(fADC * CADC * ln(2^(n+2)) - RADC
 *
 * n = resolution = 12
 * k = number of sampling periods
 */
