#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

class GlobalPreferences {
public:
  enum class Language {
  	French = 0,
  	English = 1,
  	Spanish = 2
  };
  GlobalPreferences();
  static GlobalPreferences * sharedGlobalPreferences();
  Language language() const;
  void setLanguage(Language language);
private:
  Language m_language;
};

#endif
