#include "tab_table_controller.h"

namespace Shared {

TabTableController::TabTableController(Responder * parentResponder, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin, SelectableTableViewDelegate * delegate, bool showIndicators) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, topMargin, rightMargin, bottomMargin, leftMargin,
    delegate, showIndicators, true, Palette::WallScreenDark))
{
}

View * TabTableController::view() {
  return &m_selectableTableView;
}

void TabTableController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

KDCoordinate TabTableController::cumulatedWidthFromIndex(int i) {
  return i*columnWidth(0);
}

KDCoordinate TabTableController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int TabTableController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / columnWidth(0);
}

int TabTableController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY-1) / rowHeight(0);
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

