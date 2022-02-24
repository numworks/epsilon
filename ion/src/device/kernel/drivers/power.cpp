#include <drivers/reset.h>
#include <ion/src/shared/events.h>
#include <ion/src/shared/keyboard_queue.h>
#include <ion/usb.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/led.h>
#include <kernel/drivers/power.h>
#include <kernel/warning_display.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>
#include <regs/regs.h>
#include <shared/drivers/battery.h>
#include <shared/drivers/usb_privileged.h>

namespace Ion {
namespace Device {
namespace Power {

using namespace Regs;

bool sStandbyModeActivated = true;

void selectStandbyMode(bool standbyMode) {
  sStandbyModeActivated = standbyMode;
}

void suspend(bool checkIfOnOffKeyReleased) {
  /* Pause the stalling timer to avoid having the spinner activate during the
   * shutdown sequence.
   * The timer will resume during the next call to getEvent, when the OnOff
   * event is processed. */
  Events::pauseStallingTimer();

  if (sStandbyModeActivated) {
    return standby();
  }
  bool isLEDActive = LED::getColor() != KDColorBlack;
  bool plugged = Ion::USB::isPlugged();

  if (checkIfOnOffKeyReleased) {
    waitUntilOnOffKeyReleased();
  }

  /* First, shutdown all peripherals except LED. Indeed, the charging pin state
   * might change when we shutdown peripherals that draw current. This also
   * shutdown all interruptions. */
  Board::shutdownPeripherals(true);

  while (1) {
    // Update LED color according to plug and charge state
    Battery::initGPIO();
    USB::initGPIO();
    LED::init();
    isLEDActive = LED::updateColorWithPlugAndCharge() != KDColorBlack;

    // Configure low-power mode
    if (isLEDActive) {
      Power::sleepConfiguration();
    } else {
      Power::stopConfiguration();
    }

    // Shutdown all peripherals (except LED if active)
    Board::shutdownPeripherals(isLEDActive);

    /* Wake up on:
     * - Power key
     * - Plug/Unplug USB
     * - Stop charging */
    Power::configWakeUp();

    // Shutdown clocks (except LED and external flash)
    Board::shutdownPeripheralsClocks(isLEDActive);
    // Trampoline to bootloader suspend
    bootloaderSuspend();

    /* A hardware event triggered a wake up, we determine if the device should
     * wake up. We wake up when:
     * - only the power key was down
     * - the unplugged device was plugged
     * - the battery stopped charging */
    CORTEX.SCR()->setSLEEPDEEP(false);
    if (!isLEDActive) {
      // When LED are off, the system STOPs which switchs off the PLL
      Board::initSystemClocks();
    }
    Board::initPeripheralsClocks();

    // Check power key
    Keyboard::init(false, false); // Don't activate interruptions which would override wake-up configurations
    Keyboard::State scan = Keyboard::scan();
    Keyboard::State OnlyOnOffKeyDown = Keyboard::State(Keyboard::Key::OnOff);

    // Check plugging state
    USB::initGPIO();
    if (scan == OnlyOnOffKeyDown || (!plugged && Ion::USB::isPlugged())) {
      // Wake up
      break;
    } else {
      /* The wake up event can be an unplug event or a battery charging event.
       * In both cases, we want to update static observed states like
       * sLastUSBPlugged or sLastBatteryCharging. */
      Ion::Events::getPlatformEvent();
    }
    plugged = Ion::USB::isPlugged();
  }

  // Reset normal frequency
  Board::setStandardFrequency(Board::Frequency::High);
  Board::initPeripherals(false);
  // Update LED according to plug and charge state
  LED::updateColorWithPlugAndCharge();
  /* If the USB has been unplugged while sleeping, the USB should have been
   * soft disabled but as part of the USB peripheral was asleep, this could
   * not be done before. */
  if (Ion::USB::isPlugged()) {
    Ion::USB::disable();
  }

  if (Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::ThirdParty) {
    WarningDisplay::unauthenticatedUserland();
  }

  /* Flush the keyboard queue to avoid handling artifacts state at wake-up.
   * The very next event is the OnOffEvent to notify the userland that a
   * switchOnOff has happened. */
  Keyboard::Queue::sharedQueue()->flush();
  Keyboard::Queue::sharedQueue()->push(Ion::Keyboard::State(Ion::Keyboard::Key::OnOff));
}

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  while (Keyboard::scan().keyDown(Keyboard::Key::OnOff)) {
    Ion::Timing::msleep(10); // Debouncing
  }
  /* Special case: Power::suspend waits for the release of the OnOff key. We
   * update sKeysSeenUp accordingly. */
  Ion::Events::resetKeyboardState();
}

}
}
}
