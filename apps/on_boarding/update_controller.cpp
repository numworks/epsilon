#include "update_controller.h"
#include "../apps_container.h"
#include <assert.h>

UpdateController::UpdateController() :
  MessageController(I18n::Message::UpdateAvailable, I18n::Message::UpdateMessage1, I18n::Message::UpdateMessage2, I18n::Message::UpdateMessage3, I18n::Message::UpdateMessage4)
{
}

bool UpdateController::handleEvent(Ion::Events::Event event) {
  if (MessageController::handleEvent(event)) {
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->refreshPreferences();
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

