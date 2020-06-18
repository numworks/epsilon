#include "country_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"

namespace Shared {

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
  selectCellAtLocation(0, static_cast<int>(GlobalPreferences::sharedGlobalPreferences()->country()));
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
    /* TODO : Since the order in which the countries are displayed changes with
     * the language, we need something more sophisticated
     * than (I18n::Country)selectedRow() */
    GlobalPreferences::sharedGlobalPreferences()->setCountry(static_cast<I18n::Country>(selectedRow()));
    return true;
  }
  return false;
}

void CountryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  /* TODO : We want the countries to always appear in alphabetical order. The
   * index from which the country's name is fetched must take that into
   * account.*/
  static_cast<MessageTableCell *>(cell)->setMessage(I18n::CountryNames[index]);
}

}
