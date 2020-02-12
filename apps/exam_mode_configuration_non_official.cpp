#include "exam_mode_configuration.h"

constexpr Settings::SettingsMessageTree ExamModeConfiguration::s_modelExamChildren[] = {Settings::SettingsMessageTree(I18n::Message::ActivateExamMode), Settings::SettingsMessageTree(I18n::Message::Default)};

int ExamModeConfiguration::numberOfAvailableExamMode() {
  return 3;
}

GlobalPreferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  return GlobalPreferences::ExamMode::Standard;
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  return I18n::Message::ActivateExamMode;
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default};
    return warnings[line];
  }
  assert(mode == GlobalPreferences::ExamMode::Standard);
  I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3};
  return warnings[line];
}

KDColor ExamModeConfiguration::examModeColor(GlobalPreferences::ExamMode mode) {
  assert(mode == GlobalPreferences::ExamMode::Standard);
  return KDColorRed;
}

bool ExamModeConfiguration::appIsForbiddenInExamMode(I18n::Message appName, GlobalPreferences::ExamMode mode) {
  return false;
}

bool ExamModeConfiguration::exactExpressionsAreForbidden(GlobalPreferences::ExamMode mode) {
  return false;
}
