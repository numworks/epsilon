#include "simple_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* Simple list controller */

SimpleListController::SimpleListController(Responder * parentResponder) :
  ListController(parentResponder),
  m_cells{}
{
}

void SimpleListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, 0);
}

int SimpleListController::reusableCellCount(int type) {
  return k_maxNumberOfCells;
}

HighlightCell * SimpleListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate SimpleListController::rowHeight(int j) {
  Layout l = layoutAtIndex(j);
  if (l.isUninitialized()) {
    return 0;
  }
  return l.layoutSize().height() + 2 * Metric::CommonSmallMargin + Metric::CellSeparatorThickness;
}

void SimpleListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ExpressionTableCellWithPointer * myCell = static_cast<ExpressionTableCellWithPointer *>(cell);
  myCell->setLayout(layoutAtIndex(index));
  myCell->setAccessoryMessage(messageAtIndex(index));
}

}
