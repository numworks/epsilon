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
      m_contentView(&m_selectableListView) {}

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
  selectCell(initialSelectedRow());
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
  switch (rules) {
    case ExamMode::Ruleset::PressToTest:
      // Menu shouldn't be visible
      return 0;
    case ExamMode::Ruleset::Standard:
    case ExamMode::Ruleset::Dutch:
    case ExamMode::Ruleset::IBTest:
    case ExamMode::Ruleset::Portuguese:
    case ExamMode::Ruleset::English:
      // Reactivation button
      return 1;
    default:
      assert(rules == ExamMode::Ruleset::Off);
  }
  CountryPreferences::AvailableExamModes availableExamModes =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes();
  // Activation button(s)
  switch (availableExamModes) {
    case CountryPreferences::AvailableExamModes::StandardOnly:
    case CountryPreferences::AvailableExamModes::PortugueseOnly:
    case CountryPreferences::AvailableExamModes::EnglishOnly:
      return 1;
    case CountryPreferences::AvailableExamModes::StandardAndDutch:
      // Standard and Dutch
      return 2;
    case CountryPreferences::AvailableExamModes::AmericanAll:
      // Menu shouldn't be visible
      return 0;
    default:
      assert(availableExamModes == CountryPreferences::AvailableExamModes::All);
  }
  // All exam modes
  return k_maxNumberOfCells;
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
  CountryPreferences::AvailableExamModes availableExamModes =
      GlobalPreferences::sharedGlobalPreferences->availableExamModes();
  switch (availableExamModes) {
    case CountryPreferences::AvailableExamModes::PortugueseOnly:
      assert(index == 0);
      return ExamMode::Ruleset::Portuguese;
    case CountryPreferences::AvailableExamModes::EnglishOnly:
      assert(index == 0);
      return ExamMode::Ruleset::English;
    default:
      assert(availableExamModes ==
                 CountryPreferences::AvailableExamModes::StandardOnly ||
             availableExamModes ==
                 CountryPreferences::AvailableExamModes::StandardAndDutch ||
             availableExamModes == CountryPreferences::AvailableExamModes::All);
      constexpr ExamMode::Ruleset modes[] = {
          ExamMode::Ruleset::Standard, ExamMode::Ruleset::Dutch,
          ExamMode::Ruleset::Portuguese, ExamMode::Ruleset::English,
          ExamMode::Ruleset::IBTest};
      // -2 for Off and Press-to-test rulesets
      static_assert(
          sizeof(modes) / sizeof(ExamMode::Ruleset) ==
              static_cast<size_t>(ExamMode::Ruleset::NumberOfRulesets) - 2,
          "modes must contain each available exam mode");
      assert(index < sizeof(modes) / sizeof(modes[0]));
      return modes[index];
  }
}

I18n::Message ExamModeController::examModeActivationMessage(
    size_t index) const {
  constexpr size_t numberOfModes =
      static_cast<size_t>(ExamMode::Ruleset::NumberOfRulesets);
  constexpr size_t messagesPerMode = 2;
  constexpr I18n::Message messages[] = {
      // Off, used to specify french exam mode type
      I18n::Message::ActivateFrenchExamMode,
      I18n::Message::ReactivateFrenchExamMode,
      // Standard
      I18n::Message::ActivateExamMode,
      I18n::Message::ReactivateExamMode,
      // Dutch
      I18n::Message::ActivateDutchExamMode,
      I18n::Message::ReactivateDutchExamMode,
      // IBTest
      I18n::Message::ActivateIBExamMode,
      I18n::Message::ReactivateIBExamMode,
      // PressToTest, unused // TODO : move it add the end of the ruleset list
      I18n::Message::Default,
      I18n::Message::Default,
      // Portuguese
      I18n::Message::ActivatePortugueseExamMode,
      I18n::Message::ReactivatePortugueseExamMode,
      // English
      I18n::Message::ActivateEnglishExamMode,
      I18n::Message::ReactivateEnglishExamMode,
  };
  static_assert(sizeof(messages) / sizeof(I18n::Message) ==
                    numberOfModes * messagesPerMode,
                "messages size is invalid");

  ExamMode::Ruleset examMode =
      Preferences::sharedPreferences->examMode().ruleset();
  bool isReactivation = (examMode != ExamMode::Ruleset::Off);
  assert(!isReactivation || index == 0);
  // Exam mode is either the selected ruleset or the already activated one.
  examMode = isReactivation ? examMode : examModeRulesetAtIndex(index);
  assert(examMode != ExamMode::Ruleset::Off &&
         examMode != ExamMode::Ruleset::PressToTest &&
         GlobalPreferences::sharedGlobalPreferences->availableExamModes() !=
             CountryPreferences::AvailableExamModes::AmericanAll);
  size_t messageIndex =
      static_cast<size_t>(examMode) * messagesPerMode + isReactivation;
  if (examMode == ExamMode::Ruleset::Standard &&
      GlobalPreferences::sharedGlobalPreferences->availableExamModes() ==
          CountryPreferences::AvailableExamModes::All) {
    // Specify french exam mode type
    messageIndex -= messagesPerMode;
  }
  assert(messageIndex >= 0 && messageIndex < numberOfModes * messagesPerMode);
  return messages[messageIndex];
}

}  // namespace Settings
