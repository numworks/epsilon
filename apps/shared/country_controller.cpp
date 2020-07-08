#include "country_controller.h"
#include "../global_preferences.h"
#include "../apps_container.h"
#include <string.h>
#include <algorithm>

namespace Shared {

// CountryController::ContentView
constexpr int CountryController::ContentView::k_numberOfTextLines;

CountryController::ContentView::ContentView(CountryController * controller, SelectableTableViewDataSource * dataSource) :
  m_selectableTableView(controller, controller, dataSource),
  m_titleMessage(KDFont::LargeFont, I18n::Message::Country),
  m_displayTitle(true)
{
  m_titleMessage.setBackgroundColor(Palette::WallScreen);
  m_titleMessage.setAlignment(0.5f, 0.5f);
  I18n::Message textMessages[k_numberOfTextLines] = {I18n::Message::CountryWarning1, I18n::Message::CountryWarning2};
  for (int i = 0; i < k_numberOfTextLines; i++) {
    m_messageLines[i].setBackgroundColor(Palette::WallScreen);
    m_messageLines[i].setFont(KDFont::SmallFont);
    m_messageLines[i].setAlignment(0.5f, 0.5f);
    m_messageLines[i].setMessage(textMessages[i]);
  }
}

void CountryController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::WallScreen);
}

View * CountryController::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  switch (i) {
  case 0:
    return &m_selectableTableView;
  case 1:
    return &m_titleMessage;
  default:
    return &m_messageLines[i - 2];
  }
}

void CountryController::ContentView::layoutSubviews(bool force) {
  KDCoordinate origin = Metric::CommonTopMargin;
  if (m_displayTitle) {
    origin = layoutTitleSubview(force, origin) + Metric::CommonSmallMargin;
  }
  origin = layoutSubtitleSubview(force, origin) + Metric::CommonTopMargin;
  origin = layoutTableSubview(force, origin);
  assert(origin <= bounds().height());
}

KDCoordinate CountryController::ContentView::layoutTitleSubview(bool force, KDCoordinate verticalOrigin) {
  KDCoordinate titleHeight = m_titleMessage.font()->glyphSize().height();
  m_titleMessage.setFrame(KDRect(0, verticalOrigin, bounds().width(), titleHeight), force);
  return verticalOrigin + titleHeight;
}

KDCoordinate CountryController::ContentView::layoutSubtitleSubview(bool force, KDCoordinate verticalOrigin) {
  assert(k_numberOfTextLines > 0);
  KDCoordinate textHeight = m_messageLines[0].font()->glyphSize().height();
  for (int i = 0; i < k_numberOfTextLines; i++) {
    m_messageLines[i].setFrame(KDRect(0, verticalOrigin, bounds().width(), textHeight), force);
    verticalOrigin += textHeight;
  }
  return verticalOrigin;
}

KDCoordinate CountryController::ContentView::layoutTableSubview(bool force, KDCoordinate verticalOrigin) {
  KDCoordinate tableHeight = std::min<KDCoordinate>(
      bounds().height() - verticalOrigin,
      m_selectableTableView.minimalSizeForOptimalDisplay().height());
  m_selectableTableView.setFrame(KDRect(0, verticalOrigin, bounds().width(), tableHeight), force);
  return verticalOrigin + tableHeight;
}

// CountryController
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
  m_contentView(this, this)
{
  selectableTableView()->setTopMargin(0);
  selectableTableView()->setBottomMargin(verticalMargin);
  for (int i = 0; i < I18n::NumberOfCountries; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void CountryController::resetSelection() {
  selectableTableView()->deselectTable();
  selectCellAtLocation(0, IndexOfCountry(GlobalPreferences::sharedGlobalPreferences()->country()));
}


void CountryController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetSelection();
  /* FIXME : When selecting a country, changing the language, then coming back
   * to select a country, some countries' names will be cropped. We force the
   * TableView to refresh to prevent that. */
  selectableTableView()->reloadData();
}

bool CountryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* FIXME : Changing the unit format should perhaps be done in setCountry.*/
    I18n::Country country = CountryAtIndex(selectedRow());
    GlobalPreferences::sharedGlobalPreferences()->setCountry(country);
    Poincare::Preferences::sharedPreferences()->setUnitFormat(GlobalPreferences::sharedGlobalPreferences()->unitFormat());
    return true;
  }
  return false;
}

void CountryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  static_cast<MessageTableCell *>(cell)->setMessage(I18n::CountryNames[static_cast<uint8_t>(CountryAtIndex(index))]);
}

}
