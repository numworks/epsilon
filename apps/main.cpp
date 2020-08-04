#include "apps_container.h"
#include "global_preferences.h"
#include <poincare/init.h>
#include <ion/src/device/n0110/drivers/cache.h>

#define DUMMY_MAIN 1
#if DUMMY_MAIN

using namespace Ion::Device;
using namespace Ion::Device::Regs;

void ion_main(int argc, const char * const argv[]) {
  // Initialization
  Ion::Backlight::init();
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorRed);


  // CODE BOOTLOADER SECURE
  // Shutdown the LED
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low).shutdown();

  /* Quand MPU_ON_GPIO_B_MODER_ALTERNATE_FUNCTION la LED bleue ne s'allume pas
   * et le soft crash à l'initialisation de la LED. Sinon, la LED bleue
   * s'allume. */
#define MPU_ON_GPIO_B_MODER_ALTERNATE_FUNCTION 0
#if MPU_ON_GPIO_B_MODER_ALTERNATE_FUNCTION
  // MPU on Blue LED
  Cache::dmb();
  MPU.RNR()->setREGION(10);
  MPU.RBAR()->setADDR(0x40020400);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_64B);
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

  // CODE EN EXTERNAL FLASH
  // Témoin
  Ion::LED::setColor(KDColorRed);
  Ion::Timing::msleep(2000);

  // Essaie d'allumer la LED bleue
  // Restart the LED
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low).init();
  Ion::LED::setColor(KDColorBlue);

  while (1) {}
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
