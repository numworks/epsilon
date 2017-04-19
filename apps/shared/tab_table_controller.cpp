#include "tab_table_controller.h"

namespace Shared {

TabTableController::TabTableController(Responder * parentResponder, TableViewDataSource * dataSource, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators) :
  DynamicViewController(parentResponder),
  m_dataSource(dataSource),
  m_topMargin(topMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin),
  m_delegate(delegate),
  m_showIndicators(showIndicators)
{
}

void TabTableController::didBecomeFirstResponder() {
  app()->setFirstResponder(selectableTableView());
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
  return (SelectableTableView *)view();
}

View * TabTableController::createView() {
  return new SelectableTableView(this, m_dataSource, 0, 0, m_topMargin, m_rightMargin, m_bottomMargin, m_leftMargin, m_delegate, m_showIndicators, true, Palette::WallScreenDark);
}


}

