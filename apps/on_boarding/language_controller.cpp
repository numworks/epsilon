#include "language_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"

namespace OnBoarding {

LanguageController::LanguageController(Responder * parentResponder, LogoController * logoController, UpdateController * updateController) :
  Shared::LanguageController(parentResponder),
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  m_updateController(updateController),
#endif
  m_logoController(logoController)
{
}

void LanguageController::reinitOnBoarding() {
  resetSelection();
  app()->displayModalViewController(m_logoController, 0.5f, 0.5f);
}

bool LanguageController::handleEvent(Ion::Events::Event event) {
  if (Shared::LanguageController::handleEvent(event)) {
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
    app()->displayModalViewController(m_updateController, 0.5f, 0.5f);
#else
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
    appsContainer->refreshPreferences();
    appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
#endif
    return true;
  }
  if (event == Ion::Events::Back) {
    return true;
  }
  return false;
}

}
