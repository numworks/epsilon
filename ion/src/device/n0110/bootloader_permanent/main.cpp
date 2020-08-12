#include "../../shared/usb/calculator.h"
#include "../../shared/drivers/reset.h"
#include <ion.h>

void updateUpdatableBootloader() {
}

void ColorScreen(uint32_t color) {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(color));
  Ion::Timing::msleep(300);
}

void ion_main(int argc, const char * const argv[]) {
  // TODO LEA Initialize display with message ?
  Ion::Backlight::init();
  ColorScreen(0xFF0000);

  /* Step 1. Initialize the option bytes if needed:
   * - First, set BOOT1_ADDRESS to the permanent bootloader and BOOT0_ADDRESS to
   *   the updatable bootloader
   * - Set the RDP to 2 (TODO LEA, 1 for now) */

  /* Step 2. Process DFU requests on external flash only. If there is a reset
   * with BOOT pin to 1, this acts as the "ST bootloader". */

  // TODO LEA Do we need to disable/enable all the time?
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
      Ion::Timing::msleep(10);
    }
    if (Ion::Device::USB::Calculator::PollWithoutReset()) {
      break;
    }
    Ion::USB::disable();
  }

  /* Step 3. Update the updatable bootloader if needed and if authenticated. */
  updateUpdatableBootloader();

  /* Step 4. Reset. Always jump to the internal flash, no matter the reset
   * address the dfu transaction asked for. TODO LEA */
  Ion::Device::Reset::coreWhilePlugged();
}
