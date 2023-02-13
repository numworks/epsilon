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
  ExamMode mode = examModeAtIndex(selectedRow());
  if (Preferences::sharedPreferences->examMode().isActive()) {
    // If the exam mode is already on, this re-activate the same exam mode
    mode = Preferences::sharedPreferences->examMode();
  }
  return mode;
}

ExamMode::Mode ExamModeController::examModeAtIndex(size_t index) const {
  switch (GlobalPreferences::sharedGlobalPreferences->availableExamModes()) {
    case CountryPreferences::AvailableExamModes::PortugueseOnly:
      assert(index == 0);
      return ExamMode::Mode::Portuguese;
    case CountryPreferences::AvailableExamModes::EnglishOnly:
      assert(index == 0);
      return ExamMode::Mode::English;
    default:
      ExamMode::Mode modes[] = {ExamMode::Mode::Standard, ExamMode::Mode::Dutch,
                                ExamMode::Mode::Portuguese,
                                ExamMode::Mode::English,
                                ExamMode::Mode::IBTest};
      assert(index < sizeof(modes) / sizeof(modes[0]));
      return modes[index];
  }
}

I18n::Message ExamModeController::examModeActivationMessage(
    size_t index) const {
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

}  // namespace Settings
