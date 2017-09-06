#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include "i18n.h"

class GlobalPreferences {
public:
  enum class ExamMode {
    Activate,
    Desactivate
  };
  GlobalPreferences();
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const;
  void setLanguage(I18n::Language language);
  ExamMode examMode() const;
  void setExamMode(ExamMode examMode);
  bool showUpdatePopUp() const;
  void setShowUpdatePopUp(bool showUpdatePopUp);
  int brightnessLevel() const;
  void setBrightnessLevel(int brightnessLevel);
  constexpr static int NumberOfBrightnessStates = 5;
private:
  I18n::Language m_language;
  ExamMode m_examMode;
  bool m_showUpdatePopUp;
  int m_brightnessLevel;
};

#endif
