#include "apps_container_storage.h"
#include "global_preferences.h"

#include "tree/test.cpp"


void ion_main(int argc, char * argv[]) {
  Test::main();
}

#if 0
void ion_main(int argc, char * argv[]) {
#if EPSILON_GETOPT
  for (int i=1; i<argc; i++) {
    if (argv[i][0] != '-' || argv[i][1] != '-') {
      continue;
    }
    /* Option should be given at run-time:
     * $ ./epsilon.elf --language fr
     */
    if (strcmp(argv[i], "--language") == 0 && argc > i+1) {
      const char * languageIdentifiers[] = {"none", "en", "fr", "es", "de", "pt"};
      const char * requestedLanguageId = argv[i+1];
      for (int i=0; i<sizeof(languageIdentifiers)/sizeof(languageIdentifiers[0]); i++) {
        if (strcmp(requestedLanguageId, languageIdentifiers[i]) == 0) {
          GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)i);
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
    for (int j = 0; j < AppsContainerStorage::sharedContainer()->numberOfApps(); j++) {
      App::Snapshot * snapshot = AppsContainerStorage::sharedContainer()->appSnapshotAtIndex(j);
      int cmp = strcmp(argv[i]+2, appNames[j]);
      if (cmp == '-') {
        snapshot->setOpt(argv[i]+2+strlen(appNames[j])+1, argv[i+1]);
        break;
      }
    }
  }
#endif
  AppsContainerStorage::sharedContainer()->run();
}
#endif
