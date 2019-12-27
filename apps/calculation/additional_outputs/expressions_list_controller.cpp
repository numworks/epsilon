#include "expressions_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* Expressions list controller */

ExpressionsListController::ExpressionsListController(Responder * parentResponder) :
  ListController(parentResponder),
  m_cells{}
{
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_cells[i].setParentResponder(m_listController.selectableTableView());
  }
}

void ExpressionsListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, 0);
}

int ExpressionsListController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

HighlightCell * ExpressionsListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate ExpressionsListController::rowHeight(int j) {
  Layout l = layoutAtIndex(j);
  if (l.isUninitialized()) {
    return 0;
  }
  return l.layoutSize().height() + 2 * Metric::CommonSmallMargin + Metric::CellSeparatorThickness;
}

void ExpressionsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ExpressionTableCellWithPointer * myCell = static_cast<ExpressionTableCellWithPointer *>(cell);
  myCell->setLayout(layoutAtIndex(index));
  myCell->setAccessoryMessage(messageAtIndex(index));
}

}
