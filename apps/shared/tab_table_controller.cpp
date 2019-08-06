#include "tab_table_controller.h"

namespace Shared {

TabTableController::TabTableController(Responder * parentResponder) :
  ViewController(parentResponder)
{
}

void TabTableController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(selectableTableView());
}

void TabTableController::viewWillAppear() {
  selectableTableView()->reloadData();
}

void TabTableController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0,0);
  }
}

SelectableTableView * TabTableController::selectableTableView() {
  return static_cast<SelectableTableView *>(view());
}

}
