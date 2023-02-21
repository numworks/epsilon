#include "exam_mode_controller.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <assert.h>

#include <cmath>

#include "../../apps_container.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Settings {

ExamModeController::ExamModeController(Responder *parentResponder)
    : GenericSubController(parentResponder),
      m_contentView(&m_selectableTableView) {}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(examMode());
    return true;
  }
  return GenericSubController::handleEvent(event);
}

void ExamModeController::didEnterResponderChain(
    Responder *previousFirstResponder) {
  /* When a pop-up is dismissed, the exam mode status might have changed. We
   * reload the selection as the number of rows might have also changed. We
   * force to reload the entire data because they might have changed. */
  selectCellAtLocation(0, initialSelectedRow());
  m_contentView.reload();
  // We add a message when the mode exam is on
  if (Preferences::sharedPreferences->examMode().isActive()) {
    I18n::Message deactivateMessages[] = {I18n::Message::ToDeactivateExamMode1,
                                          I18n::Message::ToDeactivateExamMode2,
                                          I18n::Message::ToDeactivateExamMode3};
    m_contentView.setMessages(deactivateMessages,
                              k_numberOfDeactivationMessageLines);
  } else {
    m_contentView.setMessages(nullptr, 0);
  }
}

int ExamModeController::numberOfRows() const {
  /* Available exam mode depends on the selected country and the active mode.
   * A user could first activate an exam mode and then change the country. */
  ExamMode::Ruleset rules =
      Preferences::sharedPreferences->examMode().ruleset();
  if (rules == ExamMode::Ruleset::Standard ||
      rules == ExamMode::Ruleset::Dutch || rules == ExamMode::Ruleset::IBTest ||
      rules == ExamMode::Ruleset::Portuguese ||
      rules == ExamMode::Ruleset::English) {
    // Reactivation button
    return 1;
  }
  CountryPreferences::AvailableExamModes availableExamModes =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes();

  if (availableExamModes ==
          CountryPreferences::AvailableExamModes::PressToTestOnly ||
      rules == ExamMode::Ruleset::PressToTest) {
    assert(rules == ExamMode::Ruleset::Off ||
           rules == ExamMode::Ruleset::PressToTest);
    // Menu shouldn't be visible
    return 0;
  }
  // Activation button(s)
  assert(rules == ExamMode::Ruleset::Off);
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

HighlightCell *ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_maxNumberOfCells);
  return &m_cell[index];
}

int ExamModeController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell *cell,
                                                 int index) {
  MessageTableCell *myCell = static_cast<MessageTableCell *>(cell);
  myCell->setMessage(examModeActivationMessage(index));
}

int ExamModeController::initialSelectedRow() const {
  int row = selectedRow();
  if (row >= numberOfRows()) {
    return numberOfRows() - 1;
  } else if (row < 0) {
    return 0;
  }
  return row;
}

ExamMode ExamModeController::examMode() {
  ExamMode mode(examModeRulesetAtIndex(selectedRow()));
  if (Preferences::sharedPreferences->examMode().isActive()) {
    // If the exam mode is already on, this re-activate the same exam mode
    mode = Preferences::sharedPreferences->examMode();
  }
  return mode;
}

ExamMode::Ruleset ExamModeController::examModeRulesetAtIndex(
    size_t index) const {
  switch (GlobalPreferences::sharedGlobalPreferences->availableExamModes()) {
    case CountryPreferences::AvailableExamModes::PortugueseOnly:
      assert(index == 0);
      return ExamMode::Ruleset::Portuguese;
    case CountryPreferences::AvailableExamModes::EnglishOnly:
      assert(index == 0);
      return ExamMode::Ruleset::English;
    default:
      ExamMode::Ruleset modes[] = {
          ExamMode::Ruleset::Standard, ExamMode::Ruleset::Dutch,
          ExamMode::Ruleset::Portuguese, ExamMode::Ruleset::English,
          ExamMode::Ruleset::IBTest};
      assert(index < sizeof(modes) / sizeof(modes[0]));
      return modes[index];
  }
}

I18n::Message ExamModeController::examModeActivationMessage(
    size_t index) const {
  ExamMode::Ruleset examMode =
      Preferences::sharedPreferences->examMode().ruleset();
  /* If the country has all exam mode, we specify which one will be reactivated.
   * The country might still have been updated by the user after activation. */
  bool specifyFrenchExamModeType =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
      CountryPreferences::AvailableExamModes::All;
  assert(examMode == ExamMode::Ruleset::Off || index == 0);
  switch (examMode) {
    case ExamMode::Ruleset::Standard:
      return (specifyFrenchExamModeType
                  ? I18n::Message::ReactivateFrenchExamMode
                  : I18n::Message::ReactivateExamMode);
    case ExamMode::Ruleset::Dutch:
      return I18n::Message::ReactivateDutchExamMode;
    case ExamMode::Ruleset::IBTest:
      return I18n::Message::ReactivateIBExamMode;
    case ExamMode::Ruleset::Portuguese:
      return I18n::Message::ReactivatePortugueseExamMode;
    case ExamMode::Ruleset::English:
      return I18n::Message::ReactivateEnglishExamMode;
    default:
      assert(examMode == ExamMode::Ruleset::Off);
      switch (examModeRulesetAtIndex(index)) {
        case ExamMode::Ruleset::Standard:
          return (specifyFrenchExamModeType
                      ? I18n::Message::ActivateFrenchExamMode
                      : I18n::Message::ActivateExamMode);
        case ExamMode::Ruleset::Dutch:
          return I18n::Message::ActivateDutchExamMode;
        case ExamMode::Ruleset::Portuguese:
          return I18n::Message::ActivatePortugueseExamMode;
        case ExamMode::Ruleset::English:
          return I18n::Message::ActivateEnglishExamMode;
        case ExamMode::Ruleset::IBTest:
          return I18n::Message::ActivateIBExamMode;
        default:
          assert(false);
          return I18n::Message::Default;
      }
  }
}

}  // namespace Settings
