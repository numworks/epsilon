#include "accessibility_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <apps/i18n.h>

using namespace Escher;

namespace Settings {

AccessibilityController::AccessibilityController(Responder* parentResponder)
    : GenericSubController(parentResponder) {}

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
