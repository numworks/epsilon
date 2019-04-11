#include <ion/battery.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/usb.h>
#include <drivers/board.h>
#include <drivers/battery.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/usb.h>
#include <drivers/wakeup.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>

namespace Ion {

namespace Device {
namespace Board {

extern Device::Board::Frequency sNormalFrequency;

}
}

namespace Power {

using namespace Device::Regs;

void configWakeUp() {
  Device::WakeUp::onPowerKeyDown();
  Device::WakeUp::onUSBPlugging();
  Device::WakeUp::onChargingEvent();
}

void stopConfiguration() {
  // This is done differently on the models
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
  Device::Board::sNormalFrequency = Device::Board::Frequency::Low;
  Device::Board::setClockFrequency(Device::Board::Frequency::Low);

#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  // Disable over-drive
  PWR.CR()->setODSWEN(false);
  while(!PWR.CSR1()->getODSWRDY()) {
  }
  PWR.CR()->setODEN(true);

  // Choose Voltage scale 3
  PWR.CR()->setVOS(PWR::CR::Voltage::Scale3);
  while (!PWR.CSR1()->getVOSRDY()) {}
#endif

  CORTEX.SCR()->setSLEEPDEEP(false);
}

void suspend(bool checkIfPowerKeyReleased) {
  bool isLEDActive = Ion::LED::getColor() != KDColorBlack;
  bool plugged = USB::isPlugged();

  if (checkIfPowerKeyReleased) {
    /* Wait until power is released to avoid restarting just after suspending */
    bool isPowerDown = true;
    while (isPowerDown) {
      Keyboard::State scan = Keyboard::scan();
      isPowerDown = scan.keyDown(Keyboard::PowerKey);
    }
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
    if (isLEDActive) {
      sleepConfiguration();
    } else {
      stopConfiguration();
    }

    // Shutdown all peripherals (except LED if active)
    Device::Board::shutdownPeripherals(isLEDActive);

    /* Wake up on:
     * - Power key
     * - Plug/Unplug USB
     * - Stop charging */
    configWakeUp();

    // Shutdown all clocks (except the ones used by LED if active)
    Device::Board::shutdownClocks(isLEDActive);

   /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
    asm("sev");
    asm("wfe");
    asm("nop");
    asm("wfe");

    /* A hardware event triggered a wake up, we determine if the device should
     * wake up. We wake up when:
     * - only the power key was down
     * - the unplugged device was plugged */
    Device::Board::initClocks();

    // Check power key
    Device::Keyboard::init();
    Keyboard::State scan = Keyboard::scan();
    Ion::Keyboard::State OnlyPowerKeyDown = Keyboard::State(Keyboard::PowerKey);

    // Check plugging state
    Device::USB::initGPIO();
    if (scan == OnlyPowerKeyDown || (!plugged && USB::isPlugged())) {
      // Wake up
      break;
    }
    plugged = USB::isPlugged();
  }

  // Reset normal frequency
  Device::Board::sNormalFrequency = Device::Board::Frequency::High;
  Device::Board::initClocks();
  Device::Board::initPeripherals();
  // Update LED according to plug and charge state
  LED::updateColorWithPlugAndCharge();
}

}
}
