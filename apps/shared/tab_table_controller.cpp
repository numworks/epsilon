#include "tab_table_controller.h"

namespace Shared {

TabTableController::TabTableController(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, dataSource, 0, topMargin, rightMargin, bottomMargin, leftMargin,
    delegate, showIndicators, true, Palette::WallScreenDark))
{
}

View * TabTableController::view() {
  return &m_selectableTableView;
}

void TabTableController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

void TabTableController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

void TabTableController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

}

