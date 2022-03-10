#include <drivers/power.h>
#include <drivers/board.h>
#include <drivers/reset.h>
#include <drivers/wakeup.h>

namespace Ion {
namespace Power {

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  /* N0100 cannot use standby mode because the power key is not correctly wired
   * to a wake up pin in order to trigger a wake up event. If the device is in
   * standby, we won't be able to wake it up. Instead, we use the stop mode (as
   * it is the low power mode consuming less after standby mode). */
  Device::Power::stopConfiguration();
  Device::Board::shutdownPeripherals();
  Device::WakeUp::onOnOffKeyDown();
  Device::Power::internalFlashStandby();
}

}
}

namespace Ion {
namespace Device {
namespace Power {

void configWakeUp() {
  Device::WakeUp::onOnOffKeyDown();
  Device::WakeUp::onUSBPlugging();
}

void __attribute__((noinline)) internalFlashSuspend(bool isLEDActive) {
  // Shutdown all clocks (except the ones used by LED if active)
  Device::Board::shutdownClocks(isLEDActive);

  Device::Power::enterLowPowerMode();

  /* A hardware event triggered a wake up, we determine if the device should
   * wake up. We wake up when:
   * - only the power key was down
   * - the unplugged device was plugged
   * - the battery stopped charging */
  Device::Board::initClocks();
}

void __attribute__((noinline)) internalFlashStandby() {
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  Device::Reset::coreWhilePlugged();
}

void enterLowPowerMode() {
  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("nop");
  asm("wfe");
}

}
}
}
