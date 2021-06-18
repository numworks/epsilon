#include "localization_controller.h"
#include <apps/apps_container.h>
#include <apps/global_preferences.h>

namespace Shared {

// ContentView
constexpr int LocalizationController::ContentView::k_numberOfCountryWarningLines;

LocalizationController::ContentView::ContentView(LocalizationController * controller, SelectableTableViewDataSource * dataSource) :
  m_controller(controller),
  m_selectableTableView(controller, controller, dataSource),
  m_countryTitleMessage(KDFont::LargeFont, I18n::Message::Country),
  m_borderView(Palette::BackgroundApps)
{
  m_countryTitleMessage.setBackgroundColor(Palette::WallScreen);
  m_countryTitleMessage.setAlignment(0.5f, 0.5f);
  assert(k_numberOfCountryWarningLines == 2); // textMessages is not overflowed
  I18n::Message textMessages[k_numberOfCountryWarningLines] = {I18n::Message::CountryWarning1, I18n::Message::CountryWarning2};
  for (int i = 0; i < k_numberOfCountryWarningLines; i++) {
    m_countryWarningLines[i].setBackgroundColor(Palette::WallScreen);
    m_countryWarningLines[i].setFont(KDFont::SmallFont);
    m_countryWarningLines[i].setAlignment(0.5f, 0.5f);
    m_countryWarningLines[i].setMessage(textMessages[i]);
  }
}

int LocalizationController::ContentView::numberOfSubviews() const {
  return 1 + m_controller->shouldDisplayTitle() + (k_numberOfCountryWarningLines + 1) * m_controller->shouldDisplayWarning();
}

View * LocalizationController::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  /* This relies on the fact that the title is never displayed without the warning. */
  assert((!m_controller->shouldDisplayTitle()) || m_controller->shouldDisplayWarning());
  switch (i) {
  case 0:
    return &m_selectableTableView;
  case 3:
    return &m_borderView;
  case 4:
    return &m_countryTitleMessage;
  default:
    return &m_countryWarningLines[i-1];
  }
}

void LocalizationController::ContentView::modeHasChanged() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

void LocalizationController::ContentView::layoutSubviews(bool force) {
  KDCoordinate origin = 0;
  if (m_controller->shouldDisplayTitle()) {
    origin = layoutTitleSubview(force, Metric::CommonTopMargin + origin);
  }
  if (m_controller->shouldDisplayWarning()) {
    origin = layoutWarningSubview(force, Metric::CommonTopMargin + origin) + Metric::CommonTopMargin;
  }
  origin = layoutTableSubview(force, origin);
  assert(origin <= bounds().height());
}

KDCoordinate LocalizationController::ContentView::layoutTitleSubview(bool force, KDCoordinate verticalOrigin) {
  KDCoordinate titleHeight = m_countryTitleMessage.font()->glyphSize().height();
  m_countryTitleMessage.setFrame(KDRect(0, verticalOrigin, bounds().width(), titleHeight), force);
  return verticalOrigin + titleHeight;
}

KDCoordinate LocalizationController::ContentView::layoutWarningSubview(bool force, KDCoordinate verticalOrigin) {
  assert(k_numberOfCountryWarningLines > 0);
  KDCoordinate textHeight = m_countryWarningLines[0].font()->glyphSize().height();
  for (int i = 0; i < k_numberOfCountryWarningLines; i++) {
    m_countryWarningLines[i].setFrame(KDRect(0, verticalOrigin, bounds().width(), textHeight), force);
    verticalOrigin += textHeight;
  }
  return verticalOrigin;
}

KDCoordinate LocalizationController::ContentView::layoutTableSubview(bool force, KDCoordinate verticalOrigin) {
  KDCoordinate tableHeight = std::min<KDCoordinate>(
      bounds().height() - verticalOrigin,
      m_selectableTableView.minimalSizeForOptimalDisplay().height());
  KDCoordinate tableHeightSansMargin = tableHeight - m_selectableTableView.bottomMargin();

  if (m_controller->shouldDisplayWarning()) {
    /* If the top cell is cut, bot not enough to hide part of the text, it will
     * appear squashed. To prevent that, we increase the top margin slightly,
     * so that the top cell will be cropped in the middle. */
    KDCoordinate rowHeight = m_controller->cellHeight() + Metric::CellSeparatorThickness;
    KDCoordinate incompleteCellHeight = tableHeightSansMargin - (tableHeightSansMargin / rowHeight) * rowHeight;
    KDCoordinate offset = std::max(0, incompleteCellHeight - rowHeight / 2);
    tableHeight -= offset;
    verticalOrigin += offset;

    m_borderView.setFrame(KDRect(Metric::CommonLeftMargin, verticalOrigin, bounds().width() - Metric::CommonLeftMargin - Metric::CommonRightMargin, Metric::CellSeparatorThickness), force);
  }
  m_selectableTableView.setFrame(KDRect(0, verticalOrigin, bounds().width(), tableHeight), force);
  return verticalOrigin + tableHeight;
}

// LocalizationController
constexpr int LocalizationController::k_numberOfCells;

int LocalizationController::IndexOfCountry(I18n::Country country) {
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

I18n::Country LocalizationController::CountryAtIndex(int i) {
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

LocalizationController::LocalizationController(Responder * parentResponder, KDCoordinate verticalMargin, LocalizationController::Mode mode) :
  ViewController(parentResponder),
  m_contentView(this, this),
  m_mode(mode)
{
  selectableTableView()->setTopMargin((shouldDisplayWarning()) ? 0 : verticalMargin);
  selectableTableView()->setBottomMargin(verticalMargin);
  for (int i = 0; i < k_numberOfCells; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void LocalizationController::resetSelection() {
  selectableTableView()->deselectTable();
  selectCellAtLocation(0, indexOfCellToSelectOnReset());
}

void LocalizationController::setMode(LocalizationController::Mode mode) {
  selectableTableView()->deselectTable();
  m_mode = mode;
  selectableTableView()->setTopMargin((shouldDisplayWarning()) ? 0 : selectableTableView()->bottomMargin());
  m_contentView.modeHasChanged();
}

int LocalizationController::indexOfCellToSelectOnReset() const {
  assert(mode() == Mode::Language);
  return static_cast<int>(GlobalPreferences::sharedGlobalPreferences()->language());
}

const char * LocalizationController::title() {
  if (mode() == Mode::Language) {
    return I18n::translate(I18n::Message::Language);
  }
  assert(mode() == Mode::Country);
  return I18n::translate(I18n::Message::Country);
}

void LocalizationController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetSelection();
  selectableTableView()->reloadData();
}

bool LocalizationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (mode() == Mode::Language) {
      GlobalPreferences::sharedGlobalPreferences()->setLanguage(static_cast<I18n::Language>(selectedRow()));
      AppsContainer::sharedAppsContainer()->reloadTitleBarView();
    } else {
      assert(mode() == Mode::Country);
      GlobalPreferences::sharedGlobalPreferences()->setCountry(CountryAtIndex(selectedRow()));
    }
    return true;
  }
  return false;
}

void LocalizationController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (mode() == Mode::Language) {
    static_cast<MessageTableCell *>(cell)->setMessage(I18n::LanguageNames[index]);
    return;
  }
  assert(mode() == Mode::Country);
  static_cast<MessageTableCell *>(cell)->setMessage(I18n::CountryNames[static_cast<uint8_t>(CountryAtIndex(index))]);
}
}
