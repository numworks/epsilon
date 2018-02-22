#include "apps_container_storage.h"
#include "global_preferences.h"

AppsContainerStorage container;

void ion_main(int argc, char * argv[]) {
#if EPSILON_GETOPT
  for (int i=1; i<argc; i++) {
    if (argv[i][0] != '-' || argv[i][1] != '-') {
      continue;
    }
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
    const char * appNames[] = {"home", EPSILON_APPS_NAMES};
    for (int j = 0; j < container.numberOfApps(); j++) {
      App::Snapshot * snapshot = container.appSnapshotAtIndex(j);
      int cmp = strcmp(argv[i]+2, appNames[j]);
      if (cmp == '-') {
        snapshot->setOpt(argv[i]+2+strlen(appNames[j])+1, argv[i+1]);
        break;
      }
    }
  }
#endif
  container.run();
}
