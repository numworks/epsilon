#include <ion.h>
#include "regs/regs.h"
#include "device.h"
#include "keyboard.h"
#include "led.h"
#include "display.h"

void Ion::Power::suspend() {
  Device::shutdownPeripherals();

  PWR.CR()->setLPDS(true); // Turn the regulator off. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
  CM4.SCR()->setSLEEPDEEP(true);

  Keyboard::Device::generateWakeUpEventForPowerKey();

  Device::shutdownClocks();

  msleep(300);

  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("wfe");

  Device::initClocks();

  Device::initPeripherals();
}
