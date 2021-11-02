#include "pop_up_controller.h"
#include "../apps_container.h"

namespace HardwareTest {

PopUpController::PopUpController() :
  Shared::PopUpController(
    Escher::Invocation(
      [](void * context, void * sender) {
        AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
        appsContainer->switchToBuiltinApp(appsContainer->hardwareTestAppSnapshot());
        return true;
      },
      this
    ),
    {
      I18n::Message::HardwareTestLaunch1,
      I18n::Message::HardwareTestLaunch2,
      I18n::Message::HardwareTestLaunch3,
      I18n::Message::HardwareTestLaunch4
    }
  )
{
}

}
