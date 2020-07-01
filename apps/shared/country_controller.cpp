#include "country_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include <string.h>

namespace Shared {

int CountryController::IndexOfCountry(I18n::Country country) {
  /* As we want to order the countries alphabetically in the selected language,
   * the index of a country in the table is the number of other countries that
   * go before it in alphabetical order. */
  int res = 0;
  for (int c = 0; c < I18n::NumberOfCountries; c++) {
    if (country != static_cast<I18n::Country>(c) && strcmp(I18n::translate(I18n::CountryNames[static_cast<uint8_t>(country)]), I18n::translate(I18n::CountryNames[c])) > 0) {
      res += 1;
    }
  }
  return res;
}

I18n::Country CountryController::CountryAtIndex(int i) {
  /* This method is called for each country one after the other, so we could
   * save a lot of computations by memoizing the IndexInTableOfCountry.
   * However, since the number of countries is fairly small, and the country
   * menu is unlikely to be used more than once or twice in the device's
   * lifespan, we skim on memory usage here.*/
  for (int c = 0; c < I18n::NumberOfCountries; c++) {
    I18n::Country country = static_cast<I18n::Country>(c);
    if (i == IndexOfCountry(country)) {
      return country;
    }
  }
  assert(false);
  return (I18n::Country)0;
}

CountryController::CountryController(Responder * parentResponder, KDCoordinate verticalMargin) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this)
{
  m_selectableTableView.setTopMargin(verticalMargin);
  m_selectableTableView.setBottomMargin(verticalMargin);
  for (int i = 0; i < I18n::NumberOfCountries; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void CountryController::resetSelection() {
  m_selectableTableView.deselectTable();
  selectCellAtLocation(0, IndexOfCountry(GlobalPreferences::sharedGlobalPreferences()->country()));
}


void CountryController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetSelection();
  /* FIXME : When selecting a country, changing the language, then coming back
   * to select a country, some countries' names will be cropped. We force the
   * TableView to refresh to prevent that. */
  m_selectableTableView.reloadData();
}

bool CountryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    GlobalPreferences::sharedGlobalPreferences()->setCountry(CountryAtIndex(selectedRow()));
    return true;
  }
  return false;
}

void CountryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  static_cast<MessageTableCell *>(cell)->setMessage(I18n::CountryNames[static_cast<uint8_t>(CountryAtIndex(index))]);
}

}
