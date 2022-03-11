#include "buffers_and_expressions_list_controller.h"
#include "../app.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

BuffersAndExpressionsListController::BuffersAndExpressionsListController(EditExpressionController * editExpressionController) :
  ListController(editExpressionController),
  m_numberOfExpressionCells(0),
  m_expressionCells{},
  m_bufferCells{}
{
  for (int i = 0; i < k_maxNumberOfExpressionCells; i++) {
    m_expressionCells[i].setParentResponder(m_listController.selectableTableView());
  }
}

void BuffersAndExpressionsListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, 0);
}

int BuffersAndExpressionsListController::reusableCellCount(int type) {
  assert(type == k_expressionCellType || type == k_bufferCellType);
  if (type == k_expressionCellType) {
    return m_numberOfExpressionCells;
  }
  return k_maxNumberOfBufferCells;
}

void BuffersAndExpressionsListController::viewDidDisappear() {
  ListController::viewDidDisappear();
  // Reset layout and cell memoization to avoid taking extra space in the pool
  for (int i = 0; i < k_maxNumberOfExpressionCells; i++) {
    m_expressionCells[i].setLayout(Layout());
    m_expressionCells[i].setHighlighted(false);
  }
  for (int i = 0; i < k_maxNumberOfBufferCells; i++) {
    m_bufferCells[i].setHighlighted(false);
  }
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_layouts[i] = Layout();
  }
  m_expression = Expression();
  m_numberOfExpressionCells = 0;
}

HighlightCell * BuffersAndExpressionsListController::reusableCell(int index, int type) {
  assert(type == k_expressionCellType || type == k_bufferCellType);
  if (type == k_expressionCellType) {
    return &m_expressionCells[index];
  }
  return &m_bufferCells[index];
}

KDCoordinate BuffersAndExpressionsListController::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_expressionCellType) {
    ExpressionTableCellWithMessage tempCell;
    return heightForCellAtIndex(&tempCell, index);
  }
  BufferTableCellWithMessage tempCell;
  return heightForCellAtIndex(&tempCell, index);
}

void BuffersAndExpressionsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_expressionCellType) {
    ExpressionTableCellWithMessage * myCell = static_cast<ExpressionTableCellWithMessage *>(cell);
    myCell->setLayout(layoutAtIndex(index));
    myCell->setSubLabelMessage(messageAtIndex(index));
    myCell->reloadScroll();
  } else {
    BufferTableCellWithMessage * myCell = static_cast<BufferTableCellWithMessage *>(cell);
    fillBufferCellAtIndex(myCell, index - m_numberOfExpressionCells);
    myCell->setSubLabelMessage(messageAtIndex(index));
  }
}

int BuffersAndExpressionsListController::numberOfRows() const {
  int nbOfRows = 0;
  for (size_t i = 0; i < k_maxNumberOfRows; i++) {
    if (!m_layouts[i].isUninitialized()) {
      nbOfRows++;
    }
  }
  return nbOfRows;
}

void BuffersAndExpressionsListController::setExpression(Poincare::Expression e) {
  // Reinitialize memoization
  resetMemoization();
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_layouts[i] = Layout();
  }
  m_expression = e;
  m_numberOfExpressionCells = 0;
}

Poincare::Layout BuffersAndExpressionsListController::layoutAtIndex(int index) {
  assert(!m_layouts[index].isUninitialized());
  return m_layouts[index];
}

int BuffersAndExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_layouts[index].serializeParsedExpression(buffer, bufferSize, App::app()->localContext());
}

}

