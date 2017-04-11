#include <ion.h>
#include "regs/regs.h"
#include "battery.h"
#include "device.h"
#include "keyboard.h"
#include "led.h"
#include "display.h"
#include "usb.h"

void Ion::Power::suspend() {
  Device::shutdownPeripherals(USB::isPlugged());

  PWR.CR()->setLPDS(true); // Turn the regulator off. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
  CM4.SCR()->setSLEEPDEEP(true);

  Device::shutdownClocks();

  msleep(300);

  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */

  Device::initStandbyClock();

  //TODO: optimize stand by consumption
  while (1) {
    Keyboard::Device::generateWakeUpEventForPowerKey();
    Battery::Device::generateWakeUpEventForChargingState();
    USB::Device::generateWakeUpEventForUSBPlug();

    asm("sev");
    asm("wfe");
    msleep(1);
    asm("wfe");

    Keyboard::Device::init();
    Keyboard::State scan = Keyboard::scan();
    Keyboard::Device::shutdown();

    // Update LEDS
    KDColor LEDColor = KDColorBlack;
    if (Battery::isCharging()) {
      LEDColor = KDColorOrange;
    } else if (Ion::USB::isPlugged()) {
      LEDColor = KDColorGreen;
    } else {
      LEDColor = KDColorBlack;
    }
    if (LEDColor == KDColorBlack) {
      LED::setColor(LEDColor);
      //TODO: understand why only shutdown the LED is inefficient
      Device::shutdownPeripherals(false);
    } else {
      LED::Device::init();
      LED::setColor(LEDColor);
    }

    Ion::Keyboard::State OnlyPowerKeyDown = (1 << (uint8_t)Keyboard::Key::B2);
    if (scan == OnlyPowerKeyDown) {
      // Wake up
      break;
    }
  }

  Device::initClocks();

  Device::initPeripherals();
}
