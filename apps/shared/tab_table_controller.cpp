#include "tab_table_controller.h"

using namespace Escher;

namespace Shared {

TabTableController::TabTableController(Responder* parentResponder)
    : ViewController(parentResponder) {}

void TabTableController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectableTableView()->reloadData();
}

void TabTableController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(selectableTableView());
  } else if (event.type == ResponderChainEventType::WillExit) {
    if (event.nextFirstResponder == tabController()) {
      assert(tabController() != nullptr);
      selectableTableView()->deselectTable();
      selectableTableView()->scrollToCell(0, 0);
    }
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

}  // namespace Shared
