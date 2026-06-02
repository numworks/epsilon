#include "accessibility_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <escher/metric.h>
#include <escher/theme/color_picker.h>

using namespace Escher;

namespace Settings {

AccessibilityController::ContentView::ContentView(SelectableListView* listView)
    : m_listView(listView), m_hintMessage(I18n::Message::MagnifyHint) {
  m_hintMessage.setBackgroundColor(ThemeComponent::WallScreen);
  m_hintMessage.setFont(KDFont::Size::Small);
  m_hintMessage.setTextColor(ThemeComponent::TextGrayDark);
  m_hintMessage.setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
}

void AccessibilityController::ContentView::drawRect(KDContext* ctx,
                                                    KDRect rect) const {
  ctx->fillRect(bounds(), ColorPicker::Resolve(ThemeComponent::WallScreen));
}

View* AccessibilityController::ContentView::subviewAtIndex(int i) {
  if (i == 0) {
    return m_listView;
  }
  return &m_hintMessage;
}

void AccessibilityController::ContentView::layoutSubviews(bool force) {
  KDCoordinate hintHeight =
      m_hintMessage.minimalSizeForOptimalDisplay().height() +
      Metric::CommonMargins.top();
  /* Give m_listView the entire frame and set m_hintMessage frame over it at the
   * bottom. */
  setChildFrame(m_listView, bounds(), force);
  setChildFrame(
      &m_hintMessage,
      KDRect(0, bounds().height() - hintHeight, bounds().width(), hintHeight),
      force);
  /* Assert that there are enough space visually for both subviews, to avoid one
   * being drawn over the other. This  must be done after m_listView has been
   * given a width. */
  assert(bounds().height() >=
         m_listView->minimalSizeForOptimalDisplay().height() + hintHeight);
}

AccessibilityController::AccessibilityController(Responder* parentResponder)
    : GenericSubController(parentResponder),
      m_contentView(selectableListView()) {}

bool AccessibilityController::handleEvent(Ion::Events::Event event) {
  int index = selectedRow();
  assert(index >= 0 && index < k_totalNumberOfCell);
  assert(k_totalNumberOfCell == numberOfRows());

  AbstractMenuCell* cell =
      static_cast<AbstractMenuCell*>(m_selectableListView.cell(index));
  if (!cell->canBeActivatedByEvent(event)) {
    return GenericSubController::handleEvent(event);
  }

  GlobalPreferences* prefs = GlobalPreferences::SharedGlobalPreferences();
  if (index == k_contrastRow) {
    prefs->setTheme(prefs->theme() == Escher::Theme::Dark
                        ? Escher::Theme::Light
                        : Escher::Theme::Dark);
    AppsContainer::sharedAppsContainer()->redrawWindow(true);
  } else {
    assert(index == k_magnifyRow);
    prefs->setAllowFocus(!prefs->allowFocus());
  }
  m_selectableListView.reloadSelectedCell();
  return true;
}

HighlightCell* AccessibilityController::reusableCell(int index, int type) {
  assert(index < reusableCellCount(type));
  return &m_switchCells[index];
}

int AccessibilityController::reusableCellCount(int type) const {
  assert(type == 0);
  return k_totalNumberOfCell;
}

KDCoordinate AccessibilityController::nonMemoizedRowHeight(int row) {
  SwitchCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void AccessibilityController::fillCellForRow(HighlightCell* cell, int row) {
  SwitchCell* switchCell = static_cast<SwitchCell*>(cell);
  switchCell->label()->setMessage(
      m_messageTreeModel->childAtIndex(row)->label());
  bool state = (row == k_contrastRow)
                   ? (SharedGlobalPrefs().theme() == Escher::Theme::Dark)
                   : SharedGlobalPrefs().allowFocus();
  switchCell->accessory()->setState(state);
}

}  // namespace Settings
