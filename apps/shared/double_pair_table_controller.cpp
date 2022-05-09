
#include "double_pair_table_controller.h"

namespace Shared {

bool DoublePairTableController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    Escher::Container::activeApp()->setFirstResponder(tabController());
    return true;
  }
  return false;
}
void DoublePairTableController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 1);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  TabTableController::didBecomeFirstResponder();
}

}

