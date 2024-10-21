#include "screen_timeout_controller.h"

#include <apps/i18n.h>

using namespace Shared;
using namespace Escher;

namespace Settings {

bool ScreenTimeoutController::handleEvent(Ion::Events::Event event) {
  return GenericSubController::handleEvent(event);
}

int ScreenTimeoutController::initialSelectedRow() const {
  // return (int)preferences->screenTimeoutSetting();
  return 0;
}

HighlightCell* ScreenTimeoutController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int ScreenTimeoutController::reusableCellCount(int type) const {
  return k_totalNumberOfCell;
}

void ScreenTimeoutController::fillCellForRow(HighlightCell* cell, int row) {
  GenericSubController::fillCellForRow(cell, row);
}

KDCoordinate ScreenTimeoutController::nonMemoizedRowHeight(int row) {
  MenuCell<MessageTextView, LayoutView> tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void ScreenTimeoutController::viewWillAppear() {
  GenericSubController::viewWillAppear();
}

}  // namespace Settings
