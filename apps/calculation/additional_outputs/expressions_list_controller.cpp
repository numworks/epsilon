#include "expressions_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* Expressions list controller */

ExpressionsListController::ExpressionsListController(Responder * parentResponder, EditExpressionController * editExpressionController) :
  ListController(parentResponder, editExpressionController),
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

void ExpressionsListController::viewDidDisappear() {
  ListController::viewDidDisappear();
  // Reset cell memoization to avoid taking extra space in the pool
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_cells[i].setLayout(Layout());
  }
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
  myCell->reloadScroll();
}

void ExpressionsListController::setExpression(Poincare::Expression e) {
  // Reinitialize memoization
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_layouts[i] = Layout();
  }
  m_expression = e;
}

Poincare::Layout ExpressionsListController::layoutAtIndex(int index) {
  if (m_layouts[index].isUninitialized()) {
    computeLayoutAtIndex(index);
  }
  assert(!m_layouts[index].isUninitialized());
  return m_layouts[index];
}

int ExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  return m_layouts[index].serializeParsedExpression(buffer, bufferSize, App::app()->localContext());
}

}
