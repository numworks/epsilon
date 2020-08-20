#include "../../shared/usb/calculator.h"
#include "../../shared/drivers/external_flash.h"
#include "../../shared/drivers/board.h"
#include "../../shared/drivers/flash.h"
#include "../../shared/drivers/reset.h"
#include <ion/src/device/n0110/drivers/cache.h>
#include <ion.h>

using namespace Ion::Device;
using namespace Ion::Device::Regs;

void decrypt(uint8_t * signature, uint8_t * decryptedSignature) {
  // TODO : Decrypt signature with public key
  memcpy(decryptedSignature, signature, Ion::Sha256DigestBytes);
}

bool IsAuthenticated(void * pointer) {
  /* Data structure at pointer must be :
   * | code size |         code        |   signature   | */
  // Extract size and code
  size_t size = *(size_t*) pointer;
  uint8_t * code = (uint8_t *)pointer + sizeof(size_t);
  // Hash code
  uint8_t digest[Ion::Sha256DigestBytes];
  // By construction, Sha256 also hashes code size into digest
  Ion::sha256(code, size, digest);
  // Extract and Decrypt signature
  uint8_t * signature = (uint8_t *)pointer + sizeof(size_t) + size;
  uint8_t decryptedSignature[Ion::Sha256DigestBytes];
  decrypt(signature, decryptedSignature);
  // Code is authenticated if signature decrypts to digest
  return memcmp(digest, decryptedSignature, Ion::Sha256DigestBytes) == 0;
}

void ColorScreen(uint32_t color) {
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(color));
  Ion::Timing::msleep(300);
}

bool StartOfExternalFlashIsAuthenticated() {
  // TODO LEA + instead of complicated cryptographic computations, we can start by simply checking if there are only 0s.
  // void * StartOfExternalFlashAddress = reinterpret_cast<void *>(0x90000000);
  // return IsAuthenticated(StartOfExternalFlashAddress);
  return true;
}

void UpdateUpdatableBootloader() {
  if (!StartOfExternalFlashIsAuthenticated()) {
    // Nothing to update
    return;
  }

  /* To run the permanent bootloader, the device must haven been reset, so there
   * is no ongoing Write protection of the internal flash. We can simply memcpy
   * the data. */
  // TODO LEA Get values from elsewhere
  uint8_t * UpdatableBootloaderAddress = reinterpret_cast<uint8_t *>(0x08000000 + 16*1024);
  uint8_t * StartOfExternalFlashAddress = reinterpret_cast<uint8_t *>(0x90000000);
  constexpr size_t SizeOfUpdatableBootloader = (64 - 16) * 1024; // TODO LEA we could try not to copy the padding, only the real length
  for (int i = 1; i < 4; i++) {
    Ion::Device::Flash::EraseSector(i); //TODO LEA erase only what needed?
  }
  Ion::Device::Flash::WriteMemory(UpdatableBootloaderAddress, StartOfExternalFlashAddress, SizeOfUpdatableBootloader);
}

void ion_main(int argc, const char * const argv[]) {
  // TODO LEA Initialize display with message ?
  Ion::Backlight::init();

  /* Step 1. Initialize the option bytes if needed:
   * - First, set BOOT1_ADDRESS to the permanent bootloader and BOOT0_ADDRESS to
   *   the updatable bootloader
   * - Set the RDP to 2 (TODO LEA, 1 for now) */



  // TODO LEA factorize
  // TODO LEA remove the "#if 0" to replace ST's bootloader by our permanent bootloader
#if 0
  // Set the reset address when the 6 key is pressed
  constexpr uint16_t Boot0Address = 0x0081; // TODO LEA Compute from 0x00200000 + fetch 0x00200000 elsewhere
  if (FLASH.OPTCR1()->getBOOT_ADD1() != Boot1Address) {
    // Unlock option bytes programming
    if (FLASH.OPTCR()->getOPTLOCK()) {
      FLASH.OPTKEYR()->set(0x08192A3B);
      FLASH.OPTKEYR()->set(0x4C5D6E7F);
    }
    assert(FLASH.OPTCR()->getOPTLOCK() == false);

    // 1. Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register
    assert(!FLASH.SR()->getBSY());
    // 2. Write the desired option value in the FLASH_OPTCR register.
    FLASH.OPTCR1()->setBOOT_ADD1(Boot1Address); // Boot from the permanent bootloader on ITCM interface (0x00200000)
    Cache::dsb();
    //3. Set the option start bit (OPTSTRT) in the FLASH_OPTCR register
    FLASH.OPTCR()->setOPTSTRT(true);
    //4. Wait for the BSY bit to be cleared.
    while (FLASH.SR()->getBSY()) {}
    FLASH.OPTCR()->setOPTLOCK(true);
  }
//#endif

  constexpr uint16_t Boot0Address = 0x0081; // TODO LEA Compute from 0x00204000 + fetch 0x00204000 elsewhere
  if (FLASH.OPTCR1()->getBOOT_ADD0() != Boot0Address) {
    // Set the reset address when the 6 key is not pressed
    if (FLASH.OPTCR()->getOPTLOCK()) {
      FLASH.OPTKEYR()->set(0x08192A3B);
      FLASH.OPTKEYR()->set(0x4C5D6E7F);
    }
    assert(FLASH.OPTCR()->getOPTLOCK() == false);
    assert(!FLASH.SR()->getBSY());
    FLASH.OPTCR1()->setBOOT_ADD0(Boot0Address); // Boot from updatable bootloader on ITCM interface (0x00204000);
    Cache::dsb();
    FLASH.OPTCR()->setOPTSTRT(true);
    while (FLASH.SR()->getBSY()) {}
    FLASH.OPTCR()->setOPTLOCK(true);
  }
  // TODO LEA end factorize


  // TODO LEA remove #if 0
//#if 0
  constexpr FLASH::OPTCR::RDP RDPLevel = FLASH::OPTCR::RDP::Level1;
  if (FLASH.OPTCR()->getRDP() != RDPLevel) {
    // Set the RDP to Level 2
    if (FLASH.OPTCR()->getOPTLOCK()) {
      FLASH.OPTKEYR()->set(0x08192A3B);
      FLASH.OPTKEYR()->set(0x4C5D6E7F);
    }
    assert(FLASH.OPTCR()->getOPTLOCK() == false);
    assert(!FLASH.SR()->getBSY());
    FLASH.OPTCR()->setRDP(FLASH::OPTCR::RDP::Level1); // TODO LEA put to level 2 for production
    Cache::dsb();
    FLASH.OPTCR()->setOPTSTRT(true);
    while (FLASH.SR()->getBSY()) {}
    FLASH.OPTCR()->setOPTLOCK(true);
  }
#endif

  /* Step 2. Process DFU requests on external flash only. If there is a reset
   * with BOOT pin to 1, this acts as the "ST bootloader". */

  ColorScreen(0x00FF00);

  // TODO LEA Do we need to disable/enable all the time?
  while (true) {
    Ion::USB::enable();
    while (!Ion::USB::isEnumerated()) {
      Ion::Timing::msleep(10);
    }
    if (Ion::Device::USB::Calculator::PollWithoutReset()) {
      Ion::USB::disable();
      break;
    }
    Ion::USB::disable();
  }

  /* Step 3. Update the updatable bootloader if needed and if authenticated. */
  UpdateUpdatableBootloader();
  ColorScreen(0x0000FF);
  Ion::Timing::msleep(2000);

  /* Step 4. Reset. Always jump to the internal flash, no matter the reset
   * address the dfu transaction asked for. TODO LEA */
  Ion::Device::ExternalFlash::shutdown(); //TODO LEA Remove?
  Ion::Device::Reset::coreWhilePlugged();
}
