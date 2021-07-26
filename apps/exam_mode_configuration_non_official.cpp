#include "exam_mode_configuration.h"

constexpr Shared::SettingsMessageTree ExamModeConfiguration::s_modelExamChildren[] = {Shared::SettingsMessageTree(I18n::Message::ActivateExamMode), Shared::SettingsMessageTree(I18n::Message::Default)};

int ExamModeConfiguration::numberOfAvailableExamMode() {
  return 1;
}

GlobalPreferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  return GlobalPreferences::ExamMode::Standard;
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  return I18n::Message::ActivateExamMode;
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default, I18n::Message::Default};
    return warnings[line];
  }
  assert(mode == GlobalPreferences::ExamMode::Standard);
  if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
    I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3, I18n::Message::Default};
    return warnings[line];
  } else {
    I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveExamModeWithResetMessage4};
    return warnings[line];
  }
}

KDColor ExamModeConfiguration::examModeColor(GlobalPreferences::ExamMode mode) {
  assert(mode == GlobalPreferences::ExamMode::Standard);
  return KDColorRed;
}

bool ExamModeConfiguration::appIsForbiddenInExamMode(I18n::Message appName, GlobalPreferences::ExamMode mode) {
  return false;
}

bool ExamModeConfiguration::exactExpressionIsForbidden(GlobalPreferences::ExamMode mode, Poincare::Expression e) {
  return false;
}

bool ExamModeConfiguration::additionalResultsAreForbidden(GlobalPreferences::ExamMode mode) {
  return false;
}
