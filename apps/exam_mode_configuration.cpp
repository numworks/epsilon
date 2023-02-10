// SPDX-License-Identifier: CC-BY-NC-ND-4.0
// Caution: Dutch exam mode is subject to a compliance certification by a
// government agency. Distribution of a non-certified Dutch exam mode is
// illegal.

#include "exam_mode_configuration.h"

#include "global_preferences.h"

using namespace Poincare;

int ExamModeConfiguration::numberOfAvailableExamMode() {
  /* Available exam mode depends on the selected country and the active mode.
   * A user could first activate an exam mode and then change the country. */
  ExamMode examMode = Preferences::sharedPreferences->examMode();
  if (examMode == ExamMode::Mode::Standard ||
      examMode == ExamMode::Mode::Dutch || examMode == ExamMode::Mode::IBTest ||
      examMode == ExamMode::Mode::Portuguese ||
      examMode == ExamMode::Mode::English) {
    // Reactivation button
    return 1;
  }
  CountryPreferences::AvailableExamModes availableExamModes =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes();

  if (availableExamModes ==
          CountryPreferences::AvailableExamModes::PressToTestOnly ||
      examMode == ExamMode::Mode::PressToTest) {
    assert(examMode == ExamMode::Mode::Off ||
           examMode == ExamMode::Mode::PressToTest);
    // Menu shouldn't be visible
    return 0;
  }
  // Activation button(s)
  assert(examMode == ExamMode::Mode::Off);
  if (availableExamModes ==
          CountryPreferences::AvailableExamModes::StandardOnly ||
      availableExamModes ==
          CountryPreferences::AvailableExamModes::PortugueseOnly ||
      availableExamModes ==
          CountryPreferences::AvailableExamModes::EnglishOnly) {
    return 1;
  }
  if (availableExamModes ==
      CountryPreferences::AvailableExamModes::StandardAndDutch) {
    return 2;
  }
  assert(availableExamModes == CountryPreferences::AvailableExamModes::All);
  return 5;
}

bool ExamModeConfiguration::pressToTestExamModeAvailable() {
  ExamMode examMode = Preferences::sharedPreferences->examMode();
  CountryPreferences::AvailableExamModes availableExamModes =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes();
  return examMode == ExamMode::Mode::PressToTest ||
         ((availableExamModes ==
               CountryPreferences::AvailableExamModes::PressToTestOnly ||
           availableExamModes == CountryPreferences::AvailableExamModes::All) &&
          examMode == ExamMode::Mode::Off);
}

bool ExamModeConfiguration::testModeAvailable() {
  return Preferences::sharedPreferences->examMode() == ExamMode::Mode::Off &&
         GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
             CountryPreferences::AvailableExamModes::All;
}

ExamMode::Mode ExamModeConfiguration::examModeAtIndex(size_t index) {
  if (GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
      CountryPreferences::AvailableExamModes::PortugueseOnly) {
    assert(index == 0);
    return ExamMode::Mode::Portuguese;
  }
  if (GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
      CountryPreferences::AvailableExamModes::EnglishOnly) {
    assert(index == 0);
    return ExamMode::Mode::English;
  }
  ExamMode::Mode examModes[] = {ExamMode::Mode::Standard, ExamMode::Mode::Dutch,
                                ExamMode::Mode::Portuguese,
                                ExamMode::Mode::English,
                                ExamMode::Mode::IBTest};
  assert(index < sizeof(examModes) / sizeof(ExamMode::Mode));
  return examModes[index];
}

I18n::Message ExamModeConfiguration::examModeActivationMessage(size_t index) {
  ExamMode::Mode examMode = Preferences::sharedPreferences->examMode().mode();
  /* If the country has all exam mode, we specify which one will be reactivated.
   * The country might still have been updated by the user after activation. */
  bool specifyFrenchExamModeType =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
      CountryPreferences::AvailableExamModes::All;
  assert(examMode == ExamMode::Mode::Off || index == 0);
  switch (examMode) {
    case ExamMode::Mode::Standard:
      return (specifyFrenchExamModeType
                  ? I18n::Message::ReactivateFrenchExamMode
                  : I18n::Message::ReactivateExamMode);
    case ExamMode::Mode::Dutch:
      return I18n::Message::ReactivateDutchExamMode;
    case ExamMode::Mode::IBTest:
      return I18n::Message::ReactivateIBExamMode;
    case ExamMode::Mode::Portuguese:
      return I18n::Message::ReactivatePortugueseExamMode;
    case ExamMode::Mode::English:
      return I18n::Message::ReactivateEnglishExamMode;
    default:
      assert(examMode == ExamMode::Mode::Off);
      switch (examModeAtIndex(index)) {
        case ExamMode::Mode::Standard:
          return (specifyFrenchExamModeType
                      ? I18n::Message::ActivateFrenchExamMode
                      : I18n::Message::ActivateExamMode);
        case ExamMode::Mode::Dutch:
          return I18n::Message::ActivateDutchExamMode;
        case ExamMode::Mode::Portuguese:
          return I18n::Message::ActivatePortugueseExamMode;
        case ExamMode::Mode::English:
          return I18n::Message::ActivateEnglishExamMode;
        case ExamMode::Mode::IBTest:
          return I18n::Message::ActivateIBExamMode;
        default:
          assert(false);
          return I18n::Message::Default;
      }
  }
}

I18n::Message ExamModeConfiguration::examModeActivationWarningMessage(
    ExamMode mode) {
  if (mode == ExamMode::Mode::Off) {
    if (Preferences::sharedPreferences->examMode() ==
        ExamMode::Mode::PressToTest) {
      return I18n::Message::ExitPressToTestExamMode;
    }
    return I18n::Message::ExitExamMode;
  } else if (mode == ExamMode::Mode::Standard) {
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActiveExamModeMessage;
    } else {
      return I18n::Message::ActiveExamModeWithResetMessage;
    }
  } else if (mode == ExamMode::Mode::IBTest) {
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActiveIBExamModeMessage;
    } else {
      return I18n::Message::ActiveIBExamModeWithResetMessage;
    }
  } else if (mode == ExamMode::Mode::PressToTest) {
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActivePressToTestModeMessage;
    } else {
      return I18n::Message::ActivePressToTestWithResetMessage;
    }
  } else if (mode == ExamMode::Mode::Dutch) {
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActiveDutchExamModeMessage;
    } else {
      return I18n::Message::ActiveDutchExamModeWithResetMessage;
    }
  } else if (mode == ExamMode::Mode::English) {
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActiveEnglishExamModeMessage;
    } else {
      return I18n::Message::ActiveEnglishExamModeWithResetMessage;
    }
  } else {
    assert(mode == ExamMode::Mode::Portuguese);
    if (Ion::Authentication::clearanceLevel() ==
        Ion::Authentication::ClearanceLevel::NumWorks) {
      return I18n::Message::ActivePortugueseExamModeMessage;
    } else {
      return I18n::Message::ActiveExamModeWithResetMessage;
    }
  }
}
