#include "tab_table_controller.h"

using namespace Escher;

namespace Shared {

TabTableController::TabTableController(Responder* parentResponder)
    : ViewController(parentResponder) {}

void TabTableController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(selectableTableView());
}

void TabTableController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectableTableView()->reloadData();
}

void TabTableController::willExitResponderChain(Responder* nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0, 0);
  }
}

}  // namespace Shared
