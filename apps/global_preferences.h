#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include "i18n.h"

class GlobalPreferences {
public:
  GlobalPreferences();
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const;
  void setLanguage(I18n::Language language);
private:
  I18n::Language m_language;
};

#endif
