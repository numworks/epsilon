#include <kernel/drivers/led.h>
#include <drivers/usb.h>
#include <kernel/drivers/battery.h>
#include <kernel/drivers/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace LED {

KDColor updateColorWithPlugAndCharge() {
  KDColor ledColor = getColor();
  if (PersistingBytes::read() == 0xFF) { // If exam mode is on, we do not update the LED with the plugged/charging state
    if (USB::isPlugged()) {
      ledColor = Battery::isCharging() ? KDColorOrange : KDColorGreen;
    } else {
      ledColor = KDColorBlack;
    }
    setColor(ledColor);
  }
  return ledColor;
}

}
}
}
