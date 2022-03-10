#include <ion/battery.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/rtc.h>
#include <ion/usb.h>
#include <drivers/board.h>
#include <drivers/battery.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/power.h>
#include <drivers/usb.h>
#include <drivers/reset.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>
#include "events_keyboard_platform.h"

namespace Ion {

namespace Power {

void suspend(bool checkIfOnOffKeyReleased) {
  bool isLEDActive = Ion::LED::getColor() != KDColorBlack;
  bool plugged = USB::isPlugged();

  if (checkIfOnOffKeyReleased) {
    Device::Power::waitUntilOnOffKeyReleased();
  }

  /* First, shutdown all peripherals except LED. Indeed, the charging pin state
   * might change when we shutdown peripherals that draw current. */
  Device::Board::shutdownPeripherals(true);

  while (1) {
    // Update LED color according to plug and charge state
    Device::Battery::initGPIO();
    Device::USB::initGPIO();
    Device::LED::init();
    isLEDActive = LED::updateColorWithPlugAndCharge() != KDColorBlack;

    // Configure low-power mode
    if (isLEDActive || Ion::RTC::mode() == Ion::RTC::Mode::HSE) {
      Device::Power::sleepConfiguration();
    } else {
      Device::Power::stopConfiguration();
    }

    // Shutdown all peripherals (except LED if active)
    Device::Board::shutdownPeripherals(isLEDActive);

    /* Wake up on:
     * - Power key
     * - Plug/Unplug USB
     * - Stop charging */
    Device::Power::configWakeUp();

    Device::Power::internalFlashSuspend(isLEDActive);

    // Check power key
    Device::Keyboard::init();
    Keyboard::State scan = Keyboard::scan();
    Ion::Keyboard::State OnlyOnOffKeyDown = Keyboard::State(Keyboard::Key::OnOff);

    // Check plugging state
    Device::USB::initGPIO();
    if (scan == OnlyOnOffKeyDown || (!plugged && USB::isPlugged())) {
      // Wake up
      break;
    } else {
      /* The wake up event can be an unplug event or a battery charging event.
       * In both cases, we want to update static observed states like
       * sLastUSBPlugged or sLastBatteryCharging. */
      Events::getPlatformEvent();
    }
    plugged = USB::isPlugged();
  }

  // Reset normal frequency
  Device::Board::setStandardFrequency(Device::Board::Frequency::High);
  Device::Board::initPeripherals(true);
  // Update LED according to plug and charge state
  LED::updateColorWithPlugAndCharge();
  /* If the USB has been unplugged while sleeping, the USB should have been
   * soft disabled but as part of the USB peripheral was asleep, this could
   * not be done before. */
  if (USB::isPlugged()) {
    USB::disable();
  }
}

}

}

namespace Ion {
namespace Device {
namespace Power {

// Public Power methods
using namespace Device::Regs;

void stopConfiguration() {
  PWR.CR()->setMRUDS(true); // Main regulator in Low Voltage and Flash memory in Deep Sleep mode when the device is in Stop mode
  PWR.CR()->setLPUDS(true); // Low-power regulator in under-drive mode if LPDS bit is set and Flash memory in power-down when the device is in Stop under-drive mode
  PWR.CR()->setLPDS(true); // Low-power Voltage regulator on. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  PWR.CR()->setUDEN(PWR::CR::UnderDrive::Enable);
#endif

  CORTEX.SCR()->setSLEEPDEEP(true);
}

void sleepConfiguration() {
  // Decrease HCLK frequency
  Device::Board::setStandardFrequency(Device::Board::Frequency::Low);
  Device::Board::setClockFrequency(Device::Board::standardFrequency());

#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  // Disable over-drive
  PWR.CR()->setODSWEN(false);
  while(!PWR.CSR()->getODSWRDY()) {
  }
  PWR.CR()->setODEN(true);
#endif

  CORTEX.SCR()->setSLEEPDEEP(false);
}

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  bool isPowerDown = true;
  while (isPowerDown) {
    Keyboard::State scan = Keyboard::scan();
    isPowerDown = scan.keyDown(Keyboard::Key::OnOff);
  }
  Timing::msleep(100);
}

}
}
}
