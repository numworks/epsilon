#include "screen_timeout_controller.h"

#include <apps/i18n.h>

namespace Settings {

bool ScreenTimeoutController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    setPreference(selectedRow());
    // AppsContainer::sharedAppsContainer()->refreshPreferences();
    Escher::StackViewController* stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

int ScreenTimeoutController::initialSelectedRow() const {
  // return (int)preferences->screenTimeoutSetting();
  return 0;
}

void ScreenTimeoutController::setPreference(int valueIndex) {
  // TODO
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
