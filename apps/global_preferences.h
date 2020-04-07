#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>

class GlobalPreferences {
public:
  enum class ExamMode : int8_t {
    Unknown = -1,
    Off = 0,
    Standard = 1,
    NoSym = 2,
    NoSymNoText = 3,
    Dutch = 4,
  };
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  bool isInExamMode() const { return (int8_t)examMode() > 0; }
  bool isInExamModeSymbolic() const { return !((int8_t)examMode() > 1); }
  ExamMode examMode() const;
  ExamMode tempExamMode() const;
  void setExamMode(ExamMode examMode);
  void setTempExamMode(ExamMode examMode);
  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);
  const KDFont * font() const { return m_font; }
  void setFont(const KDFont * font) { m_font = font; }
  constexpr static int NumberOfBrightnessStates = 15;
private:
  static_assert(I18n::NumberOfLanguages > 0, "I18n::NumberOfLanguages is not superior to 0"); // There should already have be an error when processing an empty EPSILON_I18N flag
  GlobalPreferences() :
    m_language((I18n::Language)0),
    m_examMode(ExamMode::Unknown),
    m_tempExamMode(ExamMode::Standard),
    m_showPopUp(true),
    m_brightnessLevel(Ion::Backlight::MaxBrightness),
    m_font(KDFont::LargeFont) {}
  I18n::Language m_language;
  static_assert((int8_t)GlobalPreferences::ExamMode::Off == 0, "GlobalPreferences::isInExamMode() is not right");
  static_assert((int8_t)GlobalPreferences::ExamMode::Unknown < 0, "GlobalPreferences::isInExamMode() is not right");
  mutable ExamMode m_examMode;
  mutable ExamMode m_tempExamMode;
  bool m_showPopUp;
  int m_brightnessLevel;
  const KDFont * m_font;
};

#endif
