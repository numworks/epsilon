#include <ion.h>
#include "regs/regs.h"
#include "battery.h"
#include "device.h"
#include "display.h"
#include "keyboard.h"
#include "led.h"
#include "usb.h"
#include "wake_up.h"

void Ion::Power::suspend() {
  // Shutdown all peripherals except the LED
  Device::shutdownPeripherals();

  PWR.CR()->setLPDS(true); // Turn the regulator off. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
  CM4.SCR()->setSLEEPDEEP(true);

  WakeUp::Device::onChargingEvent();
  WakeUp::Device::onUSBPlugging();

  while (1) {
    /* Update LEDS
     * if the standby mode was stopped due to a "stop charging" event, we wait
     * a while to be sure that the plug state of the USB is up-to-date. */
    msleep(200);
    KDColor LEDColor = KDColorBlack;
    if (Battery::isCharging()) {
      LEDColor = KDColorRed;
    } else if (USB::isPlugged()) {
      LEDColor = KDColorGreen;
    } else {
      LEDColor = KDColorBlack;
    }
    if (LEDColor != KDColorBlack) {
      LED::Device::init();
    } else {
      LED::Device::shutdown();
    }
    LED::setColor(LEDColor);

    WakeUp::Device::onPowerKeyDown();

    Device::shutdownClocks();//Shutdown all except LED
    if (LEDColor == KDColorBlack) {
      Device::shutdownLEDClocks();
    }

   /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
    asm("sev");
    asm("wfe");
    msleep(1);
    asm("wfe");

    Device::initClocks();

    Keyboard::Device::init();
    Keyboard::State scan = Keyboard::scan();
    Keyboard::Device::shutdown();

    Ion::Keyboard::State OnlyPowerKeyDown = (1 << (uint8_t)Keyboard::Key::B2);
    if (scan == OnlyPowerKeyDown) {
      // Wake up
      break;
    }
  }

  Device::initClocks();

  Device::initPeripherals();
  LED::Device::init();
}
