#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>

class GlobalPreferences {
public:
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  bool examMode() const;
  void setExamMode(bool activateExamMode);
  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);
  constexpr static int NumberOfBrightnessStates = 15;
private:
  GlobalPreferences() :
    m_language(I18n::Language::EN),
    m_examMode(ExamMode::Unknown),
    m_showPopUp(true),
    m_brightnessLevel(Ion::Backlight::MaxBrightness) {}
  I18n::Language m_language;
  enum class ExamMode : uint8_t {
    Deactivate = 0,
    Activate = 1,
    Unknown = 2
  };
  static_assert((uint8_t)GlobalPreferences::ExamMode::Deactivate == 0, "GlobalPreferences::setExamMode and examMode() are not right");
  static_assert((uint8_t)GlobalPreferences::ExamMode::Activate == 1, "GlobalPreferences::setExamMode and examMode() are not right");
  mutable ExamMode m_examMode;
  bool m_showPopUp;
  int m_brightnessLevel;
};

#endif
