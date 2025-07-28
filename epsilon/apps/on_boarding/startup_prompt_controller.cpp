
#include "startup_prompt_controller.h"

#include <apps/apps_container.h>

namespace OnBoarding {

bool StartupModalHandleEvent(Ion::Events::Event event) {
  if (event.isKeyPress() && event != Ion::Events::Back &&
      event != Ion::Events::OnOff) {
    App::app()->modalViewController()->dismissModal();
    AppsContainer* appsContainer = AppsContainer::sharedAppsContainer();
    if (App::app()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->switchToBuiltinApp(appsContainer->homeAppSnapshot());
    }
    return true;
  }
  return false;
};

}  // namespace OnBoarding
