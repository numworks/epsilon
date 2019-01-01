#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include "i18n.h"

class GlobalPreferences {
public:
  enum class ExamMode {
    Activate,
    Deactivate
  };
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  ExamMode examMode() const { return m_examMode; }
  void setExamMode(ExamMode examMode) { m_examMode = examMode; }
#ifdef EPSILON_BOOT_PROMPT
  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }
#endif
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);
  constexpr static int NumberOfBrightnessStates = 5;
  bool invert() const { return m_inverted; }
#if PLATFORM == simulator // ugly fix for "simulator doesn't build"
  void setinvert(bool inverted) { m_inverted = inverted; }
#elif
  void setinvert(bool inverted) { m_inverted = inverted; Ion::Display::invert(inverted); }
#endif
private:
  GlobalPreferences() :
    m_language(I18n::Language::EN),
    m_examMode(ExamMode::Deactivate),
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
  bool m_inverted = false;
};

#endif
