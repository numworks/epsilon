#include "apps_container.h"
#include "global_preferences.h"
#include <poincare/init.h>
#include <ion/src/device/n0110/drivers/cache.h>

#define DUMMY_MAIN 1
#if DUMMY_MAIN

using namespace Ion::Device;
using namespace Ion::Device::Regs;

extern "C" {
extern void switch_to_unpriviledged();
}

void ion_main(int argc, const char * const argv[]) {
  // ----------------------- CODE BOOTLOADER SECURE ---------------------------
#if 0
  while (1) {
    Ion::Keyboard::State state = Ion::Keyboard::scan();
    if (state.keyDown(Ion::Keyboard::Key::OK)) {
      /* ACTIVATE RPD */
      // Unlock option bytes programming
      if (FLASH.OPTCR()->getOPTLOCK()) {
        FLASH.OPTKEYR()->set(0x08192A3B);
        FLASH.OPTKEYR()->set(0x4C5D6E7F);
      }
      assert(FLASH.OPTCR()->getOPTLOCK() == false);

      // 1. Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register
      assert(!FLASH.SR()->getBSY());
      // 2. Write the desired option value in the FLASH_OPTCR register.
      FLASH.OPTCR()->setRDP(FLASH::OPTCR::RDP::Level1);
      Cache::dsb();
      //3. Set the option start bit (OPTSTRT) in the FLASH_OPTCR register
      FLASH.OPTCR()->setOPTSTRT(true);
      //4. Wait for the BSY bit to be cleared.
      while (FLASH.SR()->getBSY()) {}

      // Lock option bytes programming
      FLASH.OPTCR()->setOPTLOCK(true);
      Ion::LED::setColor(KDColorRed);
      break;
    }
    if (state.keyDown(Ion::Keyboard::Key::Back)) {
      /* DESACTIVATE RPD */
      // Unlock option bytes programming
      if (FLASH.OPTCR()->getOPTLOCK()) {
        FLASH.OPTKEYR()->set(0x08192A3B);
        FLASH.OPTKEYR()->set(0x4C5D6E7F);
      }
      //assert(FLASH.OPTCR()->getOPTLOCK() == false);

      // 1. Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register
      //assert(!FLASH.SR()->getBSY());
      // 2. Write the desired option value in the FLASH_OPTCR register.
      FLASH.OPTCR()->setRDP(FLASH::OPTCR::RDP::Level0);
      Cache::dsb();
      //3. Set the option start bit (OPTSTRT) in the FLASH_OPTCR register
      FLASH.OPTCR()->setOPTSTRT(true);
      //4. Wait for the BSY bit to be cleared.
      while (FLASH.SR()->getBSY()) {}

      // Lock option bytes programming
      FLASH.OPTCR()->setOPTLOCK(true);
      Ion::LED::setColor(KDColorGreen);
      break;
    }
  }

  // Passer en mode DFU pour voir si on peut flasher un truc en flash externe
  Ion::USB::enable();
  Ion::USB::DFU();
  // Lire la flash externe et le recopier en flash interne ?
  while (1) {}
#endif

  /* MPU */
  // Shutdown the LED
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low).shutdown();

#define MPU_ON_GPIO_B_MODER_ALTERNATE_FUNCTION 0
#if MPU_ON_GPIO_B_MODER_ALTERNATE_FUNCTION
  // MPU on Blue LED
  Cache::dmb();
  MPU.RNR()->setREGION(7);
  /* We want to forbid access to the 2 first 32-bit registers of GPIOB (MODER &
   * TYPER) but the smallest MPU region size is 32-byte long. So we offset the
   * MPU region by 6 bytes - nothing seems to there? */
  MPU.RBAR()->setADDR(0x40020400-6);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32B);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);
  Cache::dsb();
  Cache::isb();
#endif

  /* Unprivileged mode */
#define SWITCH_TO_UNPRIVILEDGED 0
#if SWITCH_TO_UNPRIVILEDGED
  switch_to_unpriviledged();
  Cache::isb();
#endif

  // ----------------------- CODE THIRD PARTY ---------------------------

  // Initialize Poincare::TreePool::sharedPool
  Poincare::Init();
  AppsContainer::sharedAppsContainer()->run();
}

#else

void ion_main(int argc, const char * const argv[]) {
  // Initialize Poincare::TreePool::sharedPool
  Poincare::Init();

#if EPSILON_GETOPT
  for (int i=1; i<argc; i++) {
    if (argv[i][0] != '-' || argv[i][1] != '-') {
      continue;
    }
    /* Option should be given at run-time:
     * $ ./epsilon.elf --language fr
     */
    if (strcmp(argv[i], "--language") == 0 && argc > i+1) {
      const char * requestedLanguageId = argv[i+1];
      if (strcmp(requestedLanguageId, "none") == 0) {
        continue;
      }
      for (int j = 0; j < I18n::NumberOfLanguages; j++) {
        if (strcmp(requestedLanguageId, I18n::LanguageISO6391Codes[j]) == 0) {
          GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)j);
          GlobalPreferences::sharedGlobalPreferences()->setCountry(I18n::DefaultCountryForLanguage[j]);
          break;
        }
      }
      continue;
    }
    /* Option should be given at run-time:
     * $ ./epsilon.elf --[app_name]-[option] [arguments]
     * For example:
     * $ make -j8 PLATFORM=emscripten EPSILON_APPS=code
     * $ ./epsilon.elf --code-script hello_world.py:print("hello") --code-lock-on-console
     */
    const char * appNames[] = {"home", EPSILON_APPS_NAMES};
    for (int j = 0; j < AppsContainer::sharedAppsContainer()->numberOfApps(); j++) {
      App::Snapshot * snapshot = AppsContainer::sharedAppsContainer()->appSnapshotAtIndex(j);
      int cmp = strcmp(argv[i]+2, appNames[j]);
      if (cmp == '-') {
        snapshot->setOpt(argv[i]+2+strlen(appNames[j])+1, argv[i+1]);
        break;
      }
    }
  }
#endif

#if !PLATFORM_DEVICE
  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  volatile int stackTop;
  Ion::setStackStart((void *)(&stackTop));
#endif

  AppsContainer::sharedAppsContainer()->run();
}

#endif
