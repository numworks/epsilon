#include "exam_pop_up_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <ion/usb.h>

#include "apps_container.h"
#include "exam_mode_configuration.h"

using namespace Escher;

ExamPopUpController::ExamPopUpController()
    : Shared::MessagePopUpController(
          Invocation::Builder<ExamPopUpController>(
              [](ExamPopUpController* controller, void* sender) {
                Poincare::ExamMode mode = controller->targetExamMode();
                Poincare::ExamMode previousMode =
                    Poincare::Preferences::sharedPreferences->examMode();
                if (Ion::Authentication::clearanceLevel() !=
                    Ion::Authentication::ClearanceLevel::NumWorks) {
                  Ion::Reset::core();
                }
                Poincare::Preferences::sharedPreferences->setExamMode(mode);
                AppsContainer* container = AppsContainer::sharedAppsContainer();
                if (!mode.isActive()) {
                  if (previousMode.mode() ==
                      Poincare::ExamMode::Mode::PressToTest) {
                    Ion::Reset::core();
                    return true;
                  }
                  Ion::LED::setColor(KDColorBlack);
                  Ion::LED::updateColorWithPlugAndCharge();
                } else {
                  container->activateExamMode(mode);
                }
                container->refreshPreferences();
                Container::activeApp()->modalViewController()->dismissModal();
                if (container->activeApp()->snapshot() !=
                    container->onBoardingAppSnapshot()) {
                  container->switchToBuiltinApp(container->homeAppSnapshot());
                }
                /* Warning : By unplugging before confirmation, the examMode may
                 * be deactivated within menus that depend on the examMode
                 * status (such as PressToTest settings menu after having
                 * changed the country). */
                return true;
              },
              this),
          I18n::Message::Default) {}

void ExamPopUpController::setTargetExamMode(Poincare::ExamMode mode) {
  m_targetExamMode = mode;
  setContentMessage(
      ExamModeConfiguration::examModeActivationWarningMessage(mode));
}

void ExamPopUpController::viewDidDisappear() {
  if (!m_targetExamMode.isActive()) {
    Ion::USB::clearEnumerationInterrupt();
  }
}

bool ExamPopUpController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::USBEnumeration) {
    Container::activeApp()->modalViewController()->dismissModal();
    return false;
  }
  return MessagePopUpController::handleEvent(event);
}
