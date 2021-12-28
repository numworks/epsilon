// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a government agency. Distribution of a non-certified Dutch exam mode is illegal.

#include "exam_mode_configuration.h"
#include "global_preferences.h"

using namespace Poincare;

int ExamModeConfiguration::numberOfAvailableExamMode() {
  /* Available exam mode depends on the selected country and the active mode.
   * A user could first activate an exam mode and then change the country. */
  Preferences::ExamMode examMode = Preferences::sharedPreferences()->examMode();
  if (examMode == Preferences::ExamMode::Standard
      || examMode == Preferences::ExamMode::Dutch
      || examMode == Preferences::ExamMode::IBTest
      || examMode == Preferences::ExamMode::Portuguese) {
    // Reactivation button
    return 1;
  }
  CountryPreferences::AvailableExamModes availableExamModes = GlobalPreferences::sharedGlobalPreferences()->availableExamModes();

  if (availableExamModes == CountryPreferences::AvailableExamModes::PressToTestOnly
      || examMode == Preferences::ExamMode::PressToTest) {
    assert(examMode == Preferences::ExamMode::Off || examMode == Preferences::ExamMode::PressToTest);
    // Menu shouldn't be visible
    return 0;
  }
  // Activation button(s)
  assert(examMode == Preferences::ExamMode::Off);
  if (availableExamModes == CountryPreferences::AvailableExamModes::StandardOnly || availableExamModes == CountryPreferences::AvailableExamModes::PortugueseOnly) {
    return 1;
  }
  if (availableExamModes == CountryPreferences::AvailableExamModes::StandardAndDutch) {
    return 2;
  }
  assert(availableExamModes == CountryPreferences::AvailableExamModes::All);
  return 4;
}

bool ExamModeConfiguration::pressToTestExamModeAvailable() {
  Preferences::ExamMode examMode = Preferences::sharedPreferences()->examMode();
  CountryPreferences::AvailableExamModes availableExamModes = GlobalPreferences::sharedGlobalPreferences()->availableExamModes();
  return examMode == Preferences::ExamMode::PressToTest
         || ((availableExamModes == CountryPreferences::AvailableExamModes::PressToTestOnly
              || availableExamModes == CountryPreferences::AvailableExamModes::All)
             && examMode == Preferences::ExamMode::Off);
}

bool ExamModeConfiguration::testModeAvailable() {
  return Preferences::sharedPreferences()->examMode()
             == Preferences::ExamMode::Off
         && GlobalPreferences::sharedGlobalPreferences()->availableExamModes()
                == CountryPreferences::AvailableExamModes::All;
}

Preferences::ExamMode ExamModeConfiguration::examModeAtIndex(int index) {
  if (GlobalPreferences::sharedGlobalPreferences()->availableExamModes() == CountryPreferences::AvailableExamModes::PortugueseOnly) {
    assert(index == 0);
    return Preferences::ExamMode::Portuguese;
  }
  Preferences::ExamMode examModes[] = {Preferences::ExamMode::Standard, Preferences::ExamMode::Dutch, Preferences::ExamMode::Portuguese, Preferences::ExamMode::IBTest};
  assert(index >= 0 && index < sizeof(examModes)/sizeof(Preferences::ExamMode));
  return examModes[index];
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(int index) {
  Preferences::ExamMode examMode = Preferences::sharedPreferences()->examMode();
  /* If the country has all exam mode, we specify which one will be reactivated.
   * The country might still have been updated by the user after activation. */
  bool specifyFrenchExamModeType = GlobalPreferences::sharedGlobalPreferences()->availableExamModes() == CountryPreferences::AvailableExamModes::All;
  assert(examMode == Preferences::ExamMode::Off || index == 0);
  switch (examMode) {
    case Preferences::ExamMode::Standard:
      return (specifyFrenchExamModeType ? I18n::Message::ReactivateFrenchExamMode : I18n::Message::ReactivateExamMode);
    case Preferences::ExamMode::Dutch:
      return I18n::Message::ReactivateDutchExamMode;
    case Preferences::ExamMode::IBTest:
      return I18n::Message::ReactivateIBExamMode;
    case Preferences::ExamMode::Portuguese:
      return I18n::Message::ReactivatePortugueseExamMode;
    default:
      assert(examMode == Preferences::ExamMode::Off);
      switch (examModeAtIndex(index)) {
      case Preferences::ExamMode::Standard:
        return (specifyFrenchExamModeType ? I18n::Message::ActivateFrenchExamMode : I18n::Message::ActivateExamMode);
      case Preferences::ExamMode::Dutch:
        return I18n::Message::ActivateDutchExamMode;
      case Preferences::ExamMode::Portuguese:
        return I18n::Message::ActivatePortugueseExamMode;
      case Preferences::ExamMode::IBTest:
        return I18n::Message::ActivateIBExamMode;
      default:
        assert(false);
        return I18n::Message::Default;
      }
  }
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(Preferences::ExamMode mode, int line) {
  /* FIXME Streamline this function by merging the message using \n */
  if (mode == Preferences::ExamMode::Off) {
    I18n::Message warnings[] = {I18n::Message::ExitExamMode1, I18n::Message::ExitExamMode2, I18n::Message::Default, I18n::Message::Default};
    return warnings[line];
  } else if (mode == Preferences::ExamMode::Standard) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeMessage1, I18n::Message::ActiveExamModeMessage2, I18n::Message::ActiveExamModeMessage3, I18n::Message::Default};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveExamModeWithResetMessage4};
      return warnings[line];
    }
  } else if (mode == Preferences::ExamMode::IBTest) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActiveIBExamModeMessage1, I18n::Message::ActiveIBExamModeMessage2, I18n::Message::ActiveIBExamModeMessage3, I18n::Message::ActiveIBExamModeMessage4};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActiveIBExamModeWithResetMessage1, I18n::Message::ActiveIBExamModeWithResetMessage2, I18n::Message::ActiveIBExamModeWithResetMessage3, I18n::Message::ActiveIBExamModeWithResetMessage4};
      return warnings[line];
    }
  } else if (mode == Preferences::ExamMode::PressToTest) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActivePressToTestModeMessage1, I18n::Message::ActivePressToTestModeMessage2, I18n::Message::ActivePressToTestModeMessage3, I18n::Message::Default};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActivePressToTestWithResetMessage1, I18n::Message::ActivePressToTestWithResetMessage2, I18n::Message::ActivePressToTestWithResetMessage3, I18n::Message::ActivePressToTestWithResetMessage4};
      return warnings[line];
    }
  } else if (mode == Preferences::ExamMode::Dutch) {
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActiveDutchExamModeMessage1, I18n::Message::ActiveDutchExamModeMessage2, I18n::Message::ActiveDutchExamModeMessage3, I18n::Message::ActiveDutchExamModeMessage4};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveDutchExamModeWithResetMessage4};
      return warnings[line];
    }
  } else {
    assert(mode == Preferences::ExamMode::Portuguese);
    if (Ion::Authentication::clearanceLevel() == Ion::Authentication::ClearanceLevel::NumWorks) {
      I18n::Message warnings[] = {I18n::Message::ActivePortugueseExamModeMessage1, I18n::Message::ActivePortugueseExamModeMessage2, I18n::Message::ActivePortugueseExamModeMessage3, I18n::Message::Default};
      return warnings[line];
    } else {
      I18n::Message warnings[] = {I18n::Message::ActiveExamModeWithResetMessage1, I18n::Message::ActiveExamModeWithResetMessage2, I18n::Message::ActiveExamModeWithResetMessage3, I18n::Message::ActiveDutchExamModeWithResetMessage4};
      return warnings[line];
    }
  }
}

I18n::Message ExamModeConfiguration::forbiddenAppMessage(Preferences::ExamMode mode, int line) {
  if (mode == Preferences::ExamMode::PressToTest) {
    I18n::Message messages[] = {I18n::Message::ForbiddenAppInPressToTestMode1, I18n::Message::ForbiddenAppInPressToTestMode2};
    return messages[line];
  }
  assert(mode != Preferences::ExamMode::Off);
  I18n::Message messages[] = {I18n::Message::ForbidenAppInExamMode1, I18n::Message::ForbidenAppInExamMode2};
  return messages[line];
}

KDColor ExamModeConfiguration::examModeColor(Preferences::ExamMode mode) {
  /* The Dutch exam mode LED is supposed to be orange but we can only make
   * blink "pure" colors: with RGB leds on or off (as the PWM is used for
   * blinking). The closest "pure" color is Yellow. Moreover, Orange LED is
   * already used when the battery is charging. Using yellow, we can assert
   * that the yellow LED only means that Dutch exam mode is on and avoid
   * confusing states when the battery is charging and states when the Dutch
   * exam mode is on. */
  switch (mode) {
  case Preferences::ExamMode::Standard:
    return KDColorRed;
  case Preferences::ExamMode::Dutch:
    return KDColorYellow;
  case Preferences::ExamMode::Portuguese:
    return KDColorGreen;
  default:
    return KDColorBlack;
  }
}

bool ExamModeConfiguration::appIsForbidden(I18n::Message appName) {
  Preferences::ExamMode mode = Preferences::sharedPreferences()->examMode();
  return (appName == I18n::Message::CodeApp
          && mode == Preferences::ExamMode::Dutch)
         || (appName == I18n::Message::SolverApp
             && Preferences::sharedPreferences()->equationSolverIsForbidden());
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
  if (Preferences::sharedPreferences()->examMode() != Preferences::ExamMode::Dutch) {
    return false;
  }
  bool isFraction = e.type() == ExpressionNode::Type::Division && e.childAtIndex(0).isNumber() && e.childAtIndex(1).isNumber();
  return !(e.isNumber() || isFraction || isPrimeFactorization(e));
}

bool ExamModeConfiguration::additionalResultsAreForbidden() {
  Preferences::ExamMode mode = Preferences::sharedPreferences()->examMode();
  return mode == Preferences::ExamMode::Dutch || mode == Preferences::ExamMode::IBTest;
}

bool ExamModeConfiguration::lineDetailsAreForbidden() {
  return Preferences::sharedPreferences()->examMode() == Preferences::ExamMode::IBTest;
}

bool ExamModeConfiguration::testsGraphResultsAreForbidden() {
  return Preferences::sharedPreferences()->examMode() == Preferences::ExamMode::IBTest;
}

bool ExamModeConfiguration::inequalityGraphingIsForbidden() {
  return Preferences::sharedPreferences()->inequalityGraphingIsForbidden();
}

bool ExamModeConfiguration::implicitPlotsAreForbidden() {
  return Preferences::sharedPreferences()->implicitPlotsAreForbidden();
}

bool ExamModeConfiguration::statsDiagnosticsAreForbidden() {
  return Preferences::sharedPreferences()->statsDiagnosticsAreForbidden();
}

bool ExamModeConfiguration::vectorNormIsForbidden() {
  return Preferences::sharedPreferences()->vectorNormIsForbidden();
}

bool ExamModeConfiguration::vectorProductsAreForbidden() {
  return Preferences::sharedPreferences()->vectorProductsAreForbidden();
}

bool ExamModeConfiguration::basedLogarithmIsForbidden() {
  return Preferences::sharedPreferences()->basedLogarithmIsForbidden();
}

bool ExamModeConfiguration::sumIsForbidden() {
  return Preferences::sharedPreferences()->sumIsForbidden();
}

bool ExamModeConfiguration::unitsAreForbidden() {
  Preferences::ExamMode mode = Preferences::sharedPreferences()->examMode();
  return mode == Preferences::ExamMode::Dutch || mode == Preferences::ExamMode::IBTest;
}
