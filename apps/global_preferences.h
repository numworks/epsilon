#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include "i18n.h"

class GlobalPreferences final {
public:
  enum class ExamMode {
    Activate,
    Desactivate
  };
  static GlobalPreferences * sharedGlobalPreferences() {
    return &globalPreferences;
  }
  I18n::Language language() const {
    return m_language;
  }
  void setLanguage(I18n::Language language) {
    m_language = language;
  }
  ExamMode examMode() const {
    return m_examMode;
  }
  void setExamMode(ExamMode examMode) {
    m_examMode = examMode;
  }
  bool showUpdatePopUp() const {
    return m_showUpdatePopUp;
  }
  void setShowUpdatePopUp(bool showUpdatePopUp) {
    m_showUpdatePopUp = showUpdatePopUp;
  }
  int brightnessLevel() const {
    return m_brightnessLevel;
  }
  void setBrightnessLevel(int brightnessLevel);
  constexpr static int NumberOfBrightnessStates = 5;
private:
  GlobalPreferences() :
    m_language(I18n::Language::EN),
    m_examMode(ExamMode::Desactivate),
    m_showUpdatePopUp(true),
    m_brightnessLevel(Ion::Backlight::MaxBrightness) {}
  I18n::Language m_language;
  ExamMode m_examMode;
  bool m_showUpdatePopUp;
  int m_brightnessLevel;
  static GlobalPreferences globalPreferences;
};

#endif
