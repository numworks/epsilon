#include "global_preferences.h"

static GlobalPreferences s_globalPreferences;

GlobalPreferences::GlobalPreferences() :
  m_language(Language::French)
{
}

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  return &s_globalPreferences;
}

GlobalPreferences::Language GlobalPreferences::language() const {
  return m_language;
}

void GlobalPreferences::setLanguage(Language language) {
  if (language != m_language) {
    m_language = language;
  }
}
