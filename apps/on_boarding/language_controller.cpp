#include "language_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include <escher/scroll_view_indicator.h>
#include <algorithm>

namespace OnBoarding {

LanguageController::LanguageController(Responder * parentResponder) :
  Shared::LanguageController(
    parentResponder,
    std::max(static_cast<int>(Metric::CommonLeftMargin),
      (Ion::Display::Height - I18n::NumberOfLanguages*Metric::ParameterCellHeight)/2))
{
  static_cast<ScrollViewIndicator * >(m_selectableTableView.decorator()->indicatorAtIndex(1))->setMargin(
      std::max(static_cast<int>(Metric::CommonLeftMargin),
        (Ion::Display::Height - I18n::NumberOfLanguages*Metric::ParameterCellHeight)/2));
}

bool LanguageController::handleEvent(Ion::Events::Event event) {
  if (Shared::LanguageController::handleEvent(event)) {
    AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
    if (appsContainer->promptController()) {
      Container::activeApp()->displayModalViewController(appsContainer->promptController(), 0.5f, 0.5f);
    } else {
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  if (event == Ion::Events::Back) {
    return true;
  }
  return false;
}

}
