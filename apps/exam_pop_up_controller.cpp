#include "exam_pop_up_controller.h"
#include "apps_container.h"
#include "exam_mode_configuration.h"
#include <assert.h>

ExamPopUpController::ExamPopUpController(ExamPopUpControllerDelegate * delegate) :
  PopUpController(
    k_numberOfLines,
    Invocation(
      [](void * context, void * sender) {
        ExamPopUpController * controller = (ExamPopUpController *)context;
        GlobalPreferences::ExamMode mode = controller->targetExamMode();
        assert(mode != GlobalPreferences::ExamMode::Unknown);
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
        return true;
      }, this)
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
