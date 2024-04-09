#include <escher/init.h>
#include <poincare/include/poincare.h>
#include <poincare/init.h>

#include "apps_container.h"
#include "global_preferences.h"
#include "init.h"

#define DUMMY_MAIN 0
#if DUMMY_MAIN

#include <ion.h>

char digit_to_char(int n) { return '0' + (n % 10); }

void ion_main(int argc, const char* const argv[]) {
  Ion::LED::setColor(KDColorBlack);
  Ion::Display::pushRectUniform(KDRectScreen, KDColorWhite);

  constexpr int bufsiz = 32;
  char txtbuf[bufsiz];

  while (1) {
    float volt = Ion::Battery::voltage();
    txtbuf[0] = digit_to_char(static_cast<int>(volt));
    txtbuf[1] = '.';
    txtbuf[2] = digit_to_char(static_cast<int>(volt * 10));
    txtbuf[3] = digit_to_char(static_cast<int>(volt * 100));
    txtbuf[4] = '\0';
    Ion::Console::writeLine(txtbuf);

    Ion::Timing::msleep(1000);
  }
}

#else

void ion_main(int argc, const char *const argv[]) {
  // Initialize Poincare::Pool::sharedPool
  Poincare::Init();
  Poincare::Internal::Init();
  Escher::Init();
  Apps::Init();

#if EPSILON_GETOPT
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' || argv[i][1] != '-') {
      continue;
    }
    /* Option should be given at run-time:
     * $ ./epsilon.elf --language fr */
    if (strcmp(argv[i], "--language") == 0 && argc > i + 1) {
      const char *requestedLanguageId = argv[i + 1];
      for (int j = 0; j < I18n::NumberOfLanguages; j++) {
        if (strcmp(requestedLanguageId, I18n::LanguageISO6391Codes[j]) == 0) {
          GlobalPreferences::SharedGlobalPreferences()->setLanguage(
              (I18n::Language)j);
          GlobalPreferences::SharedGlobalPreferences()->setCountry(
              I18n::DefaultCountryForLanguage[j]);
          break;
        }
      }
      continue;
    }

    const char *appNames[] = {"home", EPSILON_APPS_NAMES};

    /* Option to open a given app at run-time:
     * $ ./epsilon.elf --open-app code */
    if (strcmp(argv[i], "--open-app") == 0 && argc > i + 1) {
      const char *requestedAppName = argv[i + 1];
      for (int j = 0;
           j < AppsContainer::sharedAppsContainer()->numberOfBuiltinApps();
           j++) {
        if (strcmp(requestedAppName, appNames[j]) == 0) {
          Escher::App::Snapshot *snapshot =
              AppsContainer::sharedAppsContainer()->appSnapshotAtIndex(j);
          AppsContainer::sharedAppsContainer()->setInitialAppSnapshot(snapshot);
          break;
        }
      }
      continue;
    }

    /* Option should be given at run-time:
     * $ ./epsilon.elf --[app_name]-[option] [arguments]
     * For example:
     * $ make -j8 PLATFORM=emscripten EPSILON_APPS=code
     * $ ./epsilon.elf --code-script hello_world.py:print("hello")
     * --code-lock-on-console
     */
    for (int j = 0;
         j < AppsContainer::sharedAppsContainer()->numberOfBuiltinApps(); j++) {
      int cmp = strcmp(argv[i] + 2, appNames[j]);
      if (cmp == '-') {
        Escher::App::Snapshot *snapshot =
            AppsContainer::sharedAppsContainer()->appSnapshotAtIndex(j);
        snapshot->setOpt(argv[i] + 2 + strlen(appNames[j]) + 1, argv[i + 1]);
        break;
      }
    }
  }
#endif

  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  volatile int stackTop;
  Ion::setStackStart((void *)(&stackTop));

  AppsContainer::sharedAppsContainer()->run();

  Poincare::Internal::Shutdown();
}

#endif
