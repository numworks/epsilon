#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include "i18n.h"

class GlobalPreferences {
public:
  enum class ExamMode {
    Activate,
    Desactivate
  };
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const;
  void setLanguage(I18n::Language language);
  ExamMode examMode() const;
  void setExamMode(ExamMode examMode);
#ifdef EPSILON_BOOT_PROMPT
  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp);
#endif
  int brightnessLevel() const;
  void setBrightnessLevel(int brightnessLevel);
  constexpr static int NumberOfBrightnessStates = 5;
private:
  GlobalPreferences() :
    m_language(I18n::Language::EN),
    m_examMode(ExamMode::Desactivate),
#ifdef EPSILON_BOOT_PROMPT
    m_showPopUp(true),
#endif
    m_brightnessLevel(Ion::Backlight::MaxBrightness) {}
  I18n::Language m_language;
  ExamMode m_examMode;
#ifdef EPSILON_BOOT_PROMPT
  bool m_showPopUp;
#endif
  int m_brightnessLevel;
};

#endif
