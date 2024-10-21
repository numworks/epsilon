#include "screen_timeout_controller.h"

#include <apps/i18n.h>

namespace Settings {

bool ScreenTimeoutController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    setPreference(selectedRow());
    // TODO: if necessary
    // AppsContainer::sharedAppsContainer()->refreshPreferences();
    Escher::StackViewController* stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

int ScreenTimeoutController::initialSelectedRow() const {
  // TODO: replace with real preference getter
  return m_preferenceIndex;
}

void ScreenTimeoutController::setPreference(int valueIndex) {
  assert(valueIndex >= 0 && valueIndex < k_totalNumberOfCell);
  // TODO: replace with real preference setter
  m_preferenceIndex = valueIndex;
}

Escher::HighlightCell* ScreenTimeoutController::reusableCell(int index,
                                                             int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int ScreenTimeoutController::reusableCellCount(int type) const {
  return GenericSubController::reusableCellCount(type);
}

void ScreenTimeoutController::fillCellForRow(Escher::HighlightCell* cell,
                                             int row) {
  GenericSubController::fillCellForRow(cell, row);
}

KDCoordinate ScreenTimeoutController::nonMemoizedRowHeight(int row) {
  return GenericSubController::nonMemoizedRowHeight(row);
}

void ScreenTimeoutController::viewWillAppear() {
  GenericSubController::viewWillAppear();
}

}  // namespace Settings
