#include <ion.h>
#include "display.h"
#include "events_keyboard.h"
#include "../../../apps/global_preferences.h"

int main(int argc, char * argv[]) {
  Ion::Display::Emscripten::init();
  Ion::Events::Emscripten::init();

  if (argc == 2) {
    const char * languageIdentifiers[] = {"none", "en", "fr", "es", "de", "pt"};
    const char * requestedLanguageId = argv[1];
    for (int i=0; i<sizeof(languageIdentifiers)/sizeof(languageIdentifiers[0]); i++) {
      if (strcmp(requestedLanguageId, languageIdentifiers[i]) == 0) {
        GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)i);
        break;
      }
    }
  }
  ion_main(argc, argv);
  return 0;
}

void Ion::msleep(long ms) {
}
