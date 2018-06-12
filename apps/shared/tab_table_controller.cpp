#include "tab_table_controller.h"

namespace Shared {

TabTableController::TabTableController(Responder * parentResponder, TableViewDataSource * dataSource) :
  DynamicViewController(parentResponder),
  m_dataSource(dataSource)
{
}

void TabTableController::didBecomeFirstResponder() {
  app()->setFirstResponder(selectableTableView());
}

void TabTableController::viewWillAppear() {
  DynamicViewController::viewWillAppear();
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

View * TabTableController::loadView() {
  SelectableTableView * selectableTableView = new SelectableTableView(this, m_dataSource, this);
  selectableTableView->setBackgroundColor(Palette::WallScreenDark);
  selectableTableView->setVerticalCellOverlap(0);
  return selectableTableView;
}

void TabTableController::unloadView(View * view) {
  delete view;
}

}

