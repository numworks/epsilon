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
  return (SelectableTableView *)view();
}

View * TabTableController::loadView() {
  return new SelectableTableView(this, m_dataSource, 0, 0,  Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, this, nullptr, true, true, Palette::WallScreenDark);
}

void TabTableController::unloadView(View * view) {
  delete view;
}

}

