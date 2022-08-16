#include "chained_expressions_list_controller.h"
#include "../app.h"
#include "apps/calculation/additional_outputs/expressions_list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

ChainedExpressionsListController::ChainedExpressionsListController(EditExpressionController * editExpressionController,  Escher::SelectableTableViewDelegate * delegate) :
  ExpressionsListController(editExpressionController, delegate)
{
}

int ChainedExpressionsListController::reusableCellCount(int type) {
  return k_maxNumberOfRows + (m_tail ? m_tail->reusableCellCount(type) : 0);
}

HighlightCell * ChainedExpressionsListController::reusableCell(int index, int type) {
  int numberOfOwnedCells = ExpressionsListController::reusableCellCount(type);
  if (index >= numberOfOwnedCells) {
    return m_tail->reusableCell(index - numberOfOwnedCells, type);
  }
  return &m_cells[index];
}

KDCoordinate ChainedExpressionsListController::nonMemoizedRowHeight(int index) {
  int numberOfOwnedCells = ExpressionsListController::numberOfRows();
  if (index >= numberOfOwnedCells) {
    return m_tail->nonMemoizedRowHeight(index - numberOfOwnedCells);
  }
  return ExpressionsListController::nonMemoizedRowHeight(index);
}

void ChainedExpressionsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int numberOfOwnedCells = ExpressionsListController::numberOfRows();
  if (index >= numberOfOwnedCells) {
    return m_tail->willDisplayCellForIndex(cell, index - numberOfOwnedCells);
  }
  return ExpressionsListController::willDisplayCellForIndex(cell, index);
}

int ChainedExpressionsListController::numberOfRows() const {
  return ExpressionsListController::numberOfRows() + (m_tail ? m_tail->numberOfRows() : 0);
}

void ChainedExpressionsListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  m_tail->setExpression(e);
}

int ChainedExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  int numberOfOwnedCells = ExpressionsListController::numberOfRows();
  if (index >= numberOfOwnedCells) {
    return m_tail->textAtIndex(buffer, bufferSize, index - numberOfOwnedCells);
  }
  return ExpressionsListController::textAtIndex(buffer, bufferSize, index);
}

}
