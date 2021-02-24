#include "expressions_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* Expressions list controller */

ExpressionsListController::ExpressionsListController(EditExpressionController * editExpressionController) :
  ListController(editExpressionController),
  m_cells{}
{
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].setParentResponder(m_listController.selectableTableView());
  }
}

void ExpressionsListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, 0);
}

int ExpressionsListController::reusableCellCount(int type) {
  return k_maxNumberOfRows;
}

void ExpressionsListController::viewDidDisappear() {
  ListController::viewDidDisappear();
  // Reset layout and cell memoization to avoid taking extra space in the pool
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].setLayout(Layout());
    /* By reseting m_layouts, numberOfRow will go down to 0, and the highlighted
     * cells won't be unselected. Therefore we unselect them here. */
    m_cells[i].setHighlighted(false);
    m_layouts[i] = Layout();
  }
  m_expression = Expression();
}

HighlightCell * ExpressionsListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate ExpressionsListController::rowHeight(int j) {
  Layout l = layoutAtIndex(j);
  assert(!l.isUninitialized());
  return l.layoutSize().height() + 2 * Metric::CommonSmallMargin + Metric::CellSeparatorThickness;
}

void ExpressionsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  /* Note : To further optimize memoization space in the pool, layout
   * serialization could be memoized instead, and layout would be recomputed
   * here, when setting cell's layout. */
  ExpressionTableCellWithPointer * myCell = static_cast<ExpressionTableCellWithPointer *>(cell);
  myCell->setLayout(layoutAtIndex(index));
  myCell->setAccessoryMessage(messageAtIndex(index));
  myCell->reloadScroll();
}

int ExpressionsListController::numberOfRows() const {
  int nbOfRows = 0;
  for (size_t i = 0; i < k_maxNumberOfRows; i++) {
    if (!m_layouts[i].isUninitialized()) {
      nbOfRows++;
    }
  }
  return nbOfRows;
}

void ExpressionsListController::setExpression(Poincare::Expression e) {
  // Reinitialize memoization
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_layouts[i] = Layout();
  }
  m_expression = e;
}

Poincare::Layout ExpressionsListController::layoutAtIndex(int index) {
  assert(!m_layouts[index].isUninitialized());
  return m_layouts[index];
}

int ExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  return m_layouts[index].serializeParsedExpression(buffer, bufferSize, App::app()->localContext());
}

}
