// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a government agency. Distribution of a non-certified Dutch exam mode is illegal.

#include "exam_mode_configuration.h"

using namespace Poincare;

int ExamModeConfiguration::numberOfAvailableExamMode() {
  /* Available exam mode depends on the selected country and the active mode.
   * A user could first activate an exam mode and then change the country. */
  GlobalPreferences::ExamMode examMode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  if (examMode == GlobalPreferences::ExamMode::Standard
      || examMode == GlobalPreferences::ExamMode::Dutch) {
    // Reactivation button
    return 1;
  }
  CountryPreferences::AvailableExamModes availableExamModes = GlobalPreferences::sharedGlobalPreferences()->availableExamModes();

  if (availableExamModes == CountryPreferences::AvailableExamModes::PressToTestOnly
      || examMode == GlobalPreferences::ExamMode::PressToTest) {
    assert(examMode == GlobalPreferences::ExamMode::Off || examMode == GlobalPreferences::ExamMode::PressToTest);
    // Menu shouldn't be visible
    return 0;
  }
  assert(examMode == GlobalPreferences::ExamMode::Off);
  if (availableExamModes == CountryPreferences::AvailableExamModes::StandardOnly) {
    // Activation button
    return 1;
  }
  assert(availableExamModes == CountryPreferences::AvailableExamModes::StandardAndDutch);
  // Activation buttons
  return 2;
}

bool ExamModeConfiguration::pressToTestExamModeAvailable() {
  GlobalPreferences::ExamMode examMode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  CountryPreferences::AvailableExamModes availableExamModes = GlobalPreferences::sharedGlobalPreferences()->availableExamModes();
  return examMode == GlobalPreferences::ExamMode::PressToTest
         || (availableExamModes == CountryPreferences::AvailableExamModes::PressToTestOnly
             && examMode == GlobalPreferences::ExamMode::Off);
}

GlobalPreferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  return index == 0 ? GlobalPreferences::ExamMode::Standard : GlobalPreferences::ExamMode::Dutch;
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  return index == 0 ? I18n::Message::ActivateExamMode : I18n::Message::ActivateDutchExamMode;
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default, I18n::Message::Default};
    return warnings[line];
  } else if (mode == GlobalPreferences::ExamMode::Standard) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3, I18n::Message::Default};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveExamModeWithResetMessage4};
      return warnings[line];
    }
  } else if (mode == GlobalPreferences::ExamMode::PressToTest) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActivePressToTestModeMessage1, I18n::Message::ActivePressToTestModeMessage2, I18n::Message::ActivePressToTestModeMessage3, I18n::Message::Default};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActivePressToTestWithResetMessage1, I18n::Message::ActivePressToTestWithResetMessage2, I18n::Message::ActivePressToTestWithResetMessage3, I18n::Message::ActivePressToTestWithResetMessage4};
      return warnings[line];
    }
  }
  assert(mode == GlobalPreferences::ExamMode::Dutch);
  if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
    I18n::Message warnings[] = {I18n::Message::ActiveDutchExamModeMessage1, I18n::Message::ActiveDutchExamModeMessage2, I18n::Message::ActiveDutchExamModeMessage3, I18n::Message::ActiveDutchExamModeMessage4};
    return warnings[line];
  } else {
    I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveDutchExamModeWithResetMessage4};
    return warnings[line];
  }
}

I18n::Message ExamModeConfiguration::forbiddenAppMessage(GlobalPreferences::ExamMode mode, int line) {
  if (mode == GlobalPreferences::ExamMode::PressToTest) {
    I18n::Message messages[] = {I18n::Message::ForbiddenAppInPressToTestMode1, I18n::Message::ForbiddenAppInPressToTestMode2};
    return messages[line];
  }
  assert(mode != GlobalPreferences::ExamMode::Off);
  I18n::Message messages[] = {I18n::Message::ForbidenAppInExamMode1, I18n::Message::ForbidenAppInExamMode2};
  return messages[line];
}

KDColor ExamModeConfiguration::examModeColor(GlobalPreferences::ExamMode mode) {
  /* The Dutch exam mode LED is supposed to be orange but we can only make
   * blink "pure" colors: with RGB leds on or off (as the PWM is used for
   * blinking). The closest "pure" color is Yellow. Moreover, Orange LED is
   * already used when the battery is charging. Using yellow, we can assert
   * that the yellow LED only means that Dutch exam mode is on and avoid
   * confusing states when the battery is charging and states when the Dutch
   * exam mode is on. */
  // TODO Emilie: what color do we forbid with unofficial userland?
  assert(mode == GlobalPreferences::ExamMode::Dutch || mode == GlobalPreferences::ExamMode::Standard);
  return mode == GlobalPreferences::ExamMode::Dutch ? KDColorYellow : KDColorRed;
}

bool ExamModeConfiguration::appIsForbidden(I18n::Message appName) {
  GlobalPreferences::ExamMode mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  return (appName == I18n::Message::CodeApp
          && mode == GlobalPreferences::ExamMode::Dutch)
         || (appName == I18n::Message::SolverApp
             && GlobalPreferences::sharedGlobalPreferences()->equationSolverIsForbidden());
}

static bool isPrimeFactorization(Expression expression) {
  /* A prime factorization can only be built with integers, powers of integers,
   * and a multiplication. */
  return !expression.hasExpression([](const Expression e, const void *) {
    return !(e.type() == ExpressionNode::Type::BasedInteger
          || e.type() == ExpressionNode::Type::Multiplication
          || (e.type() == ExpressionNode::Type::Power
           && e.childAtIndex(0).type() == ExpressionNode::Type::BasedInteger
           && e.childAtIndex(1).type() == ExpressionNode::Type::BasedInteger));
  }, nullptr);
}

bool ExamModeConfiguration::exactExpressionIsForbidden(Expression e) {
  if (GlobalPreferences::sharedGlobalPreferences()->examMode() != GlobalPreferences::ExamMode::Dutch) {
    return false;
  }
  bool isFraction = e.type() == ExpressionNode::Type::Division && e.childAtIndex(0).isNumber() && e.childAtIndex(1).isNumber();
  return !(e.isNumber() || isFraction || isPrimeFactorization(e));
}

bool ExamModeConfiguration::additionalResultsAreForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Dutch;
}

bool ExamModeConfiguration::inequalityGraphingIsForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->inequalityGraphingIsForbidden();
}

bool ExamModeConfiguration::implicitPlotsAreForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->implicitPlotsAreForbidden();
}

bool ExamModeConfiguration::statsDiagnosticsAreForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->statsDiagnosticsAreForbidden();
}

bool ExamModeConfiguration::vectorsAreForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->vectorsAreForbidden();
}

bool ExamModeConfiguration::basedLogarithmIsForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->basedLogarithmIsForbidden();
}

bool ExamModeConfiguration::sumIsForbidden() {
  return GlobalPreferences::sharedGlobalPreferences()->sumIsForbidden();
}
