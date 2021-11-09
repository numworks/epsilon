#include "test_mode_controller.h"
#include "../main_controller.h"
#include <assert.h>

using namespace Escher;

namespace Settings {

KDCoordinate TestModeController::nonMemoizedRowHeight(int j) {
  Escher::MessageTableCellWithChevron tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

HighlightCell * TestModeController::reusableCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells && type == 0);
  return m_cells + index;
}

bool TestModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_mainController->pushModel(m_messageTreeModel->childAtIndex(selectedRow()));
    return true;
  }
  return GenericSubController::handleEvent(event);
}

}
