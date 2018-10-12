#include "language_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"

namespace OnBoarding {

LanguageController::LanguageController(Responder * parentResponder, LogoController * logoController) :
  Shared::LanguageController(parentResponder, (Ion::Display::Height - I18n::NumberOfLanguages*Metric::ParameterCellHeight)/2),
  m_logoController(logoController)
{
}

void LanguageController::reinitOnBoarding() {
  resetSelection();
  app()->displayModalViewController(m_logoController, 0.5f, 0.5f);
}

bool LanguageController::handleEvent(Ion::Events::Event event) {
  if (Shared::LanguageController::handleEvent(event)) {
#if EPSILON_SOFTWARE_UPDATE_PROMPT
    AppsContainer * container = (AppsContainer *)(app()->container());
    app()->displayModalViewController(container->updatePopUpController(), 0.5f, 0.5f);
#else
    AppsContainer * appsContainer = (AppsContainer *)app()->container();
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
