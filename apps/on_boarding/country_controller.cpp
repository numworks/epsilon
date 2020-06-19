#include "country_controller.h"
#include "../apps_container.h"
#include <escher/scroll_view_indicator.h>
#include <algorithm>

namespace OnBoarding {

CountryController::CountryController(Responder * parentResponder) :
  Shared::CountryController(
    parentResponder,
    std::max(
      static_cast<int>(Metric::CommonLeftMargin),
      (Ion::Display::Height - I18n::NumberOfCountries*Metric::ParameterCellHeight)/2))
{
  static_cast<ScrollViewIndicator *>(m_selectableTableView.decorator()->indicatorAtIndex(1))->setMargin(
    std::max(
      static_cast<int>(Metric::CommonLeftMargin),
      (Ion::Display::Height - I18n::NumberOfCountries*Metric::ParameterCellHeight)/2));
}

void CountryController::resetSelection() {
  m_selectableTableView.deselectTable();
  /* The base ::CountryController behaviour is to highlight the previously
   * chosen country. On boarding, we want the highlighted cell to be the first
   * alphabetically, but with the default behaviour, it would be Canada, as it
   * is the country of value 0. */
  selectCellAtLocation(0, 0);
}

bool CountryController::handleEvent(Ion::Events::Event event) {
  if (Shared::CountryController::handleEvent(event)) {
    AppsContainer * appsContainer = AppsContainer::sharedAppsContainer();
    if (appsContainer->promptController()) {
      Container::activeApp()->displayModalViewController(appsContainer->promptController(), 0.5f, 0.5f);
    } else {
      appsContainer->switchTo(appsContainer->appSnapshotAtIndex(0));
    }
    return true;
  }
  return false;
}

}
