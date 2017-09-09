#include "update_controller.h"
#include "../apps_container.h"
#include <assert.h>

namespace OnBoarding {

static const I18n::Message *sOnBoardingMessages[] = {&I18n::Common::UpdateAvailable, &I18n::Common::UpdateMessage1, &I18n::Common::UpdateMessage2, &I18n::Common::UpdateMessage3, &I18n::Common::UpdateMessage4};

static KDColor sOnBoardingColors[] = {KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::YellowDark};

UpdateController::UpdateController() :
  MessageController(sOnBoardingMessages, sOnBoardingColors)
{
}

bool UpdateController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff) {
    app()->dismissModalViewController();
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    if (appsContainer->activeApp()->snapshot() == appsContainer->onBoardingAppSnapshot()) {
      appsContainer->refreshPreferences();
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

}
