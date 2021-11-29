#include <drivers/battery.h>
#include <drivers/led.h>
#include <ion/usb.h>
#include <kernel/drivers/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace LED {

KDColor updateColorWithPlugAndCharge() {
  KDColor ledColor = getColor();
  uint8_t examMode = PersistingBytes::read(0);
  /* If exam mode is on (except press-to-test mode and IB mode), we do not update the LED
   * with the plugged/charging state */
  if (examMode == 0 || examMode == 3 || examMode == 4) {
    if (USB::isPlugged()) {
      ledColor = Battery::isCharging() ? KDColorOrange : KDColorGreen;
    } else {
      ledColor = KDColorBlack;
    }
    setColorSecure(ledColor);
  }
  return ledColor;
}

}
}
}
