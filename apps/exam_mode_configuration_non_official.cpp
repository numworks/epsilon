#include "exam_mode_configuration.h"

using namespace Poincare;

constexpr Shared::SettingsMessageTree s_examModeMode[] = {Shared::SettingsMessageTree(I18n::Message::ExamModeModeStandard), Shared::SettingsMessageTree(I18n::Message::ExamModeModeNoSym), Shared::SettingsMessageTree(I18n::Message::ExamModeModeNoSymNoText)};
#if LEDS_CHOICE
constexpr Shared::SettingsMessageTree s_ledColorChildren[] = {Shared::SettingsMessageTree(I18n::Message::ColorRed), Shared::SettingsMessageTree(I18n::Message::ColorWhite), Shared::SettingsMessageTree(I18n::Message::ColorGreen), Shared::SettingsMessageTree(I18n::Message::ColorBlue), Shared::SettingsMessageTree(I18n::Message::ColorYellow), Shared::SettingsMessageTree(I18n::Message::ColorPurple), Shared::SettingsMessageTree(I18n::Message::ColorOrange)};
constexpr Shared::SettingsMessageTree ExamModeConfiguration::s_modelExamChildren[] = {Shared::SettingsMessageTree(I18n::Message::LEDColor, s_ledColorChildren), Shared::SettingsMessageTree(I18n::Message::ExamModeMode, s_examModeMode), Shared::SettingsMessageTree(I18n::Message::ActivateExamMode)};
#else
constexpr Shared::SettingsMessageTree ExamModeConfiguration::s_modelExamChildren[] = {Shared::SettingsMessageTree(I18n::Message::ExamModeMode, s_examModeMode), Shared::SettingsMessageTree(I18n::Message::ActivateExamMode)};
#endif

int ExamModeConfiguration::numberOfAvailableExamMode() {
#if LEDS_CHOICE
  return 3;
#else
  return 2;
#endif
}

GlobalPreferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  return (s_modelExamChildren[index].label() == I18n::Message::ExamModeModeStandard) ? GlobalPreferences::ExamMode::Standard : GlobalPreferences::ExamMode::NoSym;
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  return I18n::Message::ActivateExamMode;
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default};
    return warnings[line];
  }
  assert(mode == GlobalPreferences::ExamMode::Standard || mode == GlobalPreferences::ExamMode::NoSym || mode == GlobalPreferences::ExamMode::NoSymNoText);
  I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3};
  return warnings[line];
}

KDColor ExamModeConfiguration::examModeColor(GlobalPreferences::ExamMode mode) {
  assert(mode == GlobalPreferences::ExamMode::Standard || mode == GlobalPreferences::ExamMode::NoSym || mode == GlobalPreferences::ExamMode::NoSymNoText);
  Preferences * preferences = Preferences::sharedPreferences();

  switch((int) preferences->colorOfLED()) {
    case 1:
      return KDColorWhite;
    case 2:
      return KDColorGreen;
    case 3:
      return KDColorBlue;
    case 4:
      return KDColorYellow;
    case 5:
      return KDColorPurple;
    case 6:
      return KDColorOrange;
    default:
      return KDColorRed;
  }
}

bool ExamModeConfiguration::appIsForbiddenInExamMode(App::Descriptor::ExaminationLevel appExaminationLevel, GlobalPreferences::ExamMode mode) {
  if (mode == GlobalPreferences::ExamMode::NoSymNoText) {
    return appExaminationLevel == App::Descriptor::ExaminationLevel::Basic;
  }
  return false;
}

bool ExamModeConfiguration::exactExpressionsAreForbidden(GlobalPreferences::ExamMode mode) {
  return mode == GlobalPreferences::ExamMode::NoSymNoText ? true : false;
}
