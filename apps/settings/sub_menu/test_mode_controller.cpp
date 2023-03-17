#include "test_mode_controller.h"

#include <assert.h>

#include "../main_controller.h"

using namespace Escher;

namespace Settings {

KDCoordinate TestModeController::nonMemoizedRowHeight(int j) {
  MenuCell<MessageTextView, EmptyCellWidget, ChevronView> tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

HighlightCell* TestModeController::reusableCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells && type == 0);
  return m_cells + index;
}

bool TestModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    m_mainController->pushModel(
        m_messageTreeModel->childAtIndex(selectedRow()));
    return true;
  }
  return GenericSubController::handleEvent(event);
}

void TestModeController::didBecomeFirstResponder() {
  /* After activating an exam mode and going back, this controller shouldn't be
   * available anymore. Skip it and go back to the main settings menu. */
  if (!m_mainController->hasTestModeCell()) {
    stackController()->pop();
  } else {
    GenericSubController::didBecomeFirstResponder();
  }
}

}  // namespace Settings
