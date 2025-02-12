#include "exam_pop_up_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <ion/usb.h>

#include <array>

#include "apps_container.h"

using namespace Escher;
using namespace Poincare;

ExamPopUpController::ExamPopUpController()
    : Shared::MessagePopUpController(
          Invocation::Builder<ExamPopUpController>(
              [](ExamPopUpController* controller, void* sender) {
                return controller->handleButton();
              },
              this),
          I18n::Message::Default) {}

void ExamPopUpController::setTargetExamMode(ExamMode mode) {
  m_targetExamMode = mode;
  setContentMessage(activationWarningMessage());
}

void ExamPopUpController::viewWillAppear() {
  if (!m_targetExamMode.isActive()) {
    /* Activate the DFU when the pop-up to turn off the exam mode is displayed.
     * This is to allow exiting exam mode from a DFU request, as the exam mode
     * would otherwise disable the "Calculator connected" screen. */
    AppsContainer::sharedAppsContainer()->setDFUBetweenEvents(true);
  }
}

void ExamPopUpController::viewDidDisappear() {
  AppsContainer::sharedAppsContainer()->setDFUBetweenEvents(false);
  if (!m_targetExamMode.isActive()) {
    Ion::USB::clearEnumerationInterrupt();
  }
}

I18n::Message ExamPopUpController::activationWarningMessage() const {
  constexpr size_t messagesPerMode = 2;
  constexpr I18n::Message messages[] = {
      // Off
      I18n::Message::ExitExamMode,
      I18n::Message::ExitPressToTestExamMode,
      // Standard
      I18n::Message::ActiveExamModeMessage,
      I18n::Message::ActiveExamModeWithResetMessage,
      // Dutch
      I18n::Message::ActiveDutchExamModeMessage,
      I18n::Message::ActiveDutchOrEnglishExamModeWithResetMessage,
      // IBTest
      I18n::Message::ActiveUSAExamModeMessage,
      I18n::Message::ActiveTxPaScIbExamModeWithResetMessage,
      // PressToTest
      I18n::Message::ActivePressToTestModeMessage,
      I18n::Message::ActivePressToTestWithResetMessage,
      // Portuguese
      I18n::Message::ActivePortugueseExamModeMessage,
      I18n::Message::ActiveExamModeWithResetMessage,
      // English
      I18n::Message::ActiveEnglishExamModeMessage,
      I18n::Message::ActiveDutchOrEnglishExamModeWithResetMessage,
      // STAAR
      I18n::Message::ActiveUSAExamModeMessage,
      I18n::Message::ActiveTxPaScIbExamModeWithResetMessage,
      // Pennsylvania
      I18n::Message::ActiveUSAExamModeMessage,
      I18n::Message::ActiveTxPaScIbExamModeWithResetMessage,
      // SouthCarolina
      I18n::Message::ActiveUSAExamModeMessage,
      I18n::Message::ActiveTxPaScIbExamModeWithResetMessage,
      // NorthCarolina
      I18n::Message::ActiveUSAExamModeMessage,
      I18n::Message::ActiveTxPaScIbExamModeWithResetMessage,
  };
  static_assert(
      std::size(messages) == Ion::ExamMode::k_numberOfModes * messagesPerMode,
      "messages size is invalid");
  ExamMode::Ruleset rules = m_targetExamMode.ruleset();
  size_t index = static_cast<size_t>(rules) * messagesPerMode;
  index += (rules == ExamMode::Ruleset::Off &&
            Preferences::SharedPreferences()->examMode().ruleset() ==
                ExamMode::Ruleset::PressToTest) ||
           Ion::Authentication::clearanceLevel() !=
               Ion::Authentication::ClearanceLevel::NumWorks;
  assert(index < Ion::ExamMode::k_numberOfModes * messagesPerMode);
  return messages[index];
}

bool ExamPopUpController::handleButton() const {
  /* Warning : By unplugging before confirmation, the examMode may then be
   *           deactivated while unplugged. */
  AppsContainer::sharedAppsContainer()->setExamMode(
      m_targetExamMode, Preferences::SharedPreferences()->examMode());
  return true;
}
