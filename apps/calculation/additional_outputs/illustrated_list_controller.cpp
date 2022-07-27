#include "illustrated_list_controller.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/symbol.h>
#include "../app.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

/* Illustrated list controller */

IllustratedListController::IllustratedListController(EditExpressionController * editExpressionController) :
  ListController(editExpressionController, this),
  m_calculationStore(m_calculationStoreBuffer, k_calculationStoreBufferSize)
{
  for (int i = 0; i < k_maxNumberOfAdditionalCalculations; i++) {
    m_additionalCalculationCells[i].setParentResponder(m_listController.selectableTableView());
  }
}

void IllustratedListController::didEnterResponderChain(Responder * previousFirstResponder) {
  // Select the left subview on all cells and reinitialize scroll
  for (int i = 0; i < k_maxNumberOfAdditionalCalculations; i++) {
    m_additionalCalculationCells[i].reinitSelection();
  }
  selectCellAtLocation(0, 1);
}

void IllustratedListController::viewDidDisappear() {
  ListController::viewDidDisappear();
  m_expression = Expression();
  // Reset cell memoization to avoid taking extra space in the pool
  for (int i = 0; i < k_maxNumberOfAdditionalCalculations; i++) {
     m_additionalCalculationCells[i].resetMemoization();
   }
}

int IllustratedListController::numberOfRows() const {
  return m_calculationStore.numberOfCalculations() + 1;
}

int IllustratedListController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == k_illustrationCellType) {
    return 1;
  }
  return k_maxNumberOfAdditionalCalculations;
}

HighlightCell * IllustratedListController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == k_illustrationCellType) {
    return illustrationCell();
  }
  return &m_additionalCalculationCells[index];
}

KDCoordinate IllustratedListController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  int calculationIndex = j-1;
  if (calculationIndex >= m_calculationStore.numberOfCalculations()) {
    return 0;
  }
  ScrollableThreeExpressionsCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

void IllustratedListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_illustrationCellType) {
    return;
  }
  VariableContext context = illustratedListContext();

  ScrollableThreeExpressionsCell * myCell = static_cast<ScrollableThreeExpressionsCell *>(cell);
  Calculation * c = m_calculationStore.calculationAtIndex(index-1).pointer();
  myCell->setCalculation(c, &context);
  myCell->setDisplayCenter(c->displayOutput(&context) != Calculation::DisplayOutput::ApproximateOnly);
}

void IllustratedListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (t->selectedRow() == 0) {
    t->selectRow(1);
  }
}

void IllustratedListController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* But scroll to the top when we select the first
   * ScrollableThreeExpressionsCell in order display the
   * illustration cell. */
  if (t->selectedRow() == 1) {
    t->scrollToCell(0, 0);
  }
}

void IllustratedListController::setExpression(Poincare::Expression e) {
  m_calculationStore.deleteAll();
  m_expression = e.clone();
}

Poincare::VariableContext IllustratedListController::illustratedListContext() {
  // Create variable context containing expression for symbol
  VariableContext context = VariableContext(symbol(), App::app()->localContext());
  assert(!m_expression.isUninitialized() && !m_expression.wasErasedByException());
  context.setExpressionForSymbolAbstract(m_expression, Poincare::Symbol::Builder(symbol(), strlen(symbol())));
  return context;
}

int IllustratedListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  ScrollableThreeExpressionsCell * myCell = static_cast<ScrollableThreeExpressionsCell *>(m_listController.selectableTableView()->selectedCell());
  Shared::ExpiringPointer<Calculation> c = m_calculationStore.calculationAtIndex(index-1);
  const char * text = myCell->selectedSubviewPosition() == ScrollableThreeExpressionsView::SubviewPosition::Right ? c->approximateOutputText(Calculation::NumberOfSignificantDigits::Maximal) : c->exactOutputText();
  return strlcpy(buffer, text, bufferSize);
}

}
