#include "exam_pop_up_controller.h"
#include "apps_container.h"
#include "exam_mode_configuration.h"
#include <assert.h>
#include <apps/i18n.h>

using namespace Escher;

ExamPopUpController::ExamPopUpController(ExamPopUpControllerDelegate * delegate) :
  PopUpController(
    k_numberOfLines,
    Invocation(
      [](void * context, void * sender) {
        ExamPopUpController * controller = (ExamPopUpController *)context;
        GlobalPreferences::ExamMode mode = controller->targetExamMode();
        assert(mode != GlobalPreferences::ExamMode::Unknown);
        assert(mode == GlobalPreferences::ExamMode::PressToTest || controller->targetPressToTestParams().value == 0);
        GlobalPreferences::sharedGlobalPreferences()->setPressToTestParams(controller->targetPressToTestParams());
        GlobalPreferences::sharedGlobalPreferences()->setExamMode(mode);
        AppsContainer * container = AppsContainer::sharedAppsContainer();
        if (mode == GlobalPreferences::ExamMode::Off) {
          Ion::LED::setColor(KDColorBlack);
          Ion::LED::updateColorWithPlugAndCharge();
        } else {
          container->activateExamMode(mode);
        }
        container->refreshPreferences();
        Container::activeApp()->dismissModalViewController();
        /* TODO Hugo : Fix issues with menus that would be no longer up-to-date:
         *     - Go back to Home Menu ? */
        return true;
      }, this),
    I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel
  ),
  m_targetExamMode(GlobalPreferences::ExamMode::Unknown),
  m_delegate(delegate)
{
}

void ExamPopUpController::setTargetExamMode(GlobalPreferences::ExamMode mode) {
  m_targetExamMode = mode;
  for (int i = 0; i < k_numberOfLines; i++) {
    m_contentView.setMessage(i, ExamModeConfiguration::examModeActivationWarningMessage(mode, i));
  }
}

void ExamPopUpController::viewDidDisappear() {
  if (m_targetExamMode == GlobalPreferences::ExamMode::Off) {
    m_delegate->examDeactivatingPopUpIsDismissed();
  }
}

bool ExamPopUpController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::USBEnumeration) {
    Container::activeApp()->dismissModalViewController();
    return false;
  }
  return PopUpController::handleEvent(event);
}
