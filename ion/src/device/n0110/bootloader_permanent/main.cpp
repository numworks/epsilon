#include "../../shared/usb/calculator.h"
#include "../../shared/drivers/reset.h"
#include <ion/src/device/n0110/drivers/cache.h>
#include <ion.h>

using namespace Ion::Device;
using namespace Ion::Device::Regs;

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



  // TODO LEA factorize
  // Unlock option bytes programming
  if (FLASH.OPTCR()->getOPTLOCK()) {
    FLASH.OPTKEYR()->set(0x08192A3B);
    FLASH.OPTKEYR()->set(0x4C5D6E7F);
  }
  assert(FLASH.OPTCR()->getOPTLOCK() == false);

//TODO LEA remove the "#if 0" to replace ST's bootloader by our permanent bootloader
#if 0
  // 1. Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register
  assert(!FLASH.SR()->getBSY());
  // 2. Write the desired option value in the FLASH_OPTCR register.
  FLASH.OPTCR1()->setBOOT_ADD1(0x0080); // Boot from Flash on ITCM interface (0x0020 0000);
  Cache::dsb();
  //3. Set the option start bit (OPTSTRT) in the FLASH_OPTCR register
  FLASH.OPTCR()->setOPTSTRT(true);
  //4. Wait for the BSY bit to be cleared.
  while (FLASH.SR()->getBSY()) {}
  FLASH.OPTCR()->setOPTLOCK(true);

  // Unlock option bytes programming
  if (FLASH.OPTCR()->getOPTLOCK()) {
    FLASH.OPTKEYR()->set(0x08192A3B);
    FLASH.OPTKEYR()->set(0x4C5D6E7F);
  }
  assert(FLASH.OPTCR()->getOPTLOCK() == false);
#endif

  // 1. Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register
  assert(!FLASH.SR()->getBSY());
  // 2. Write the desired option value in the FLASH_OPTCR register.
  FLASH.OPTCR1()->setBOOT_ADD0(0x0081); // Boot from updatable bootloader on ITCM interface (0x00204000);
  Cache::dsb();
  //3. Set the option start bit (OPTSTRT) in the FLASH_OPTCR register
  FLASH.OPTCR()->setOPTSTRT(true);
  //4. Wait for the BSY bit to be cleared.
  while (FLASH.SR()->getBSY()) {}
  FLASH.OPTCR()->setOPTLOCK(true);
  // TODO LEA end factorize



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
