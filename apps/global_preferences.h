#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>

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
  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);
  bool accessibilityMagnify() const { return m_accessibilityMagnify; }
  void setAccessibilityMagnify(bool enabled) { m_accessibilityMagnify = enabled; }
  bool accessibilityInhibitMagnify() const { return m_accessibilityInhibitMagnify; }
  void setAccessibilityInhibitMagnify(bool enabled) { m_accessibilityInhibitMagnify = enabled; }
  int accessibilityMagnifyPosition() const { return m_accessibilityMagnifyPosition; }
  void setAccessibilityMagnifyPosition(int position) { m_accessibilityMagnifyPosition = position; }
  bool accessibilityInvertColors() const { return m_accessibilityInvertColors; }
  void setAccessibilityInvertColors(bool enabled) { m_accessibilityInvertColors = enabled; }
  bool accessibilityGamma() const { return m_accessibilityGamma; }
  void setAccessibilityGamma(bool enabled) { m_accessibilityGamma = enabled; }
  int accessibilityGammaRed() const { return m_accessibilityGammaRed; }
  void setAccessibilityGammaRed(int level);
  int accessibilityGammaGreen() const { return m_accessibilityGammaGreen; }
  void setAccessibilityGammaGreen(int level);
  int accessibilityGammaBlue() const { return m_accessibilityGammaBlue; }
  void setAccessibilityGammaBlue(int level);
  constexpr static int NumberOfBrightnessStates = 5;
  constexpr static int NumberOfGammaStates = 10;
private:
  GlobalPreferences() :
    m_language(I18n::Language::EN),
    m_examMode(ExamMode::Deactivate),
    m_showPopUp(true),
    m_brightnessLevel(Ion::Backlight::MaxBrightness),
    m_accessibilityMagnify(false),
    m_accessibilityInhibitMagnify(false),
    m_accessibilityMagnifyPosition(8),
    m_accessibilityInvertColors(false),
    m_accessibilityGamma(false),
    m_accessibilityGammaRed(NumberOfGammaStates/2),
    m_accessibilityGammaGreen(NumberOfGammaStates/2),
    m_accessibilityGammaBlue(NumberOfGammaStates/2) {}
  I18n::Language m_language;
  ExamMode m_examMode;
  bool m_showPopUp;
  int m_brightnessLevel;
  bool m_accessibilityMagnify;
  bool m_accessibilityInhibitMagnify;
  int m_accessibilityMagnifyPosition;
  bool m_accessibilityInvertColors;
  bool m_accessibilityGamma;
  int m_accessibilityGammaRed;
  int m_accessibilityGammaGreen;
  int m_accessibilityGammaBlue;
};

#endif
