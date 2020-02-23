// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a government agency. Distribution of a non-certified Dutch exam mode is illegal.

#include "exam_mode_configuration.h"

constexpr Shared::SettingsMessageTree ExamModeConfiguration::s_modelExamChildren[2] = {Shared::SettingsMessageTree(I18n::Message::ActivateExamMode), Shared::SettingsMessageTree(I18n::Message::ActivateDutchExamMode)};

int ExamModeConfiguration::numberOfAvailableExamMode() {
  if (GlobalPreferences::sharedGlobalPreferences()->language() != I18n::Language::EN || GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    return 1;
  }
  return 2;
}

GlobalPreferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  return index == 0 ? GlobalPreferences::ExamMode::Standard : GlobalPreferences::ExamMode::Dutch;
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  return index == 0 ? I18n::Message::ActivateExamMode : I18n::Message::ActivateDutchExamMode;
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default};
    return warnings[line];
  } else if (mode == GlobalPreferences::ExamMode::Standard) {
    I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3};
    return warnings[line];
  }
  assert(mode == GlobalPreferences::ExamMode::Dutch);
  I18n::Message warnings[] = {I18n::Message::ActiveDutchExamModeMessage1, I18n::Message::ActiveDutchExamModeMessage2, I18n::Message::ActiveDutchExamModeMessage3};
  return warnings[line];
}

KDColor ExamModeConfiguration::examModeColor(GlobalPreferences::ExamMode mode) {
  /* The Dutch exam mode LED is supposed to be orange but we can only make
   * blink "pure" colors: with RGB leds on or off (as the PWM is used for
   * blinking). The closest "pure" color is Yellow. Moreover, Orange LED is
   * already used when the battery is charging. Using yellow, we can assert
   * that the yellow LED only means that Dutch exam mode is on and avoid
   * confusing states when the battery is charging and states when the Dutch
   * exam mode is on. */
  return mode == GlobalPreferences::ExamMode::Dutch ? KDColorYellow : KDColorRed;
}

bool ExamModeConfiguration::appIsForbiddenInExamMode(I18n::Message appName, GlobalPreferences::ExamMode mode) {
  return appName == I18n::Message::CodeApp && mode == GlobalPreferences::ExamMode::Dutch;
}

bool ExamModeConfiguration::exactExpressionsAreForbidden(GlobalPreferences::ExamMode mode) {
  return mode == GlobalPreferences::ExamMode::Dutch;
}
