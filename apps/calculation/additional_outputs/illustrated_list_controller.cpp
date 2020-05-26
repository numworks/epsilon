#include "illustrated_list_controller.h"
#include <poincare/symbol.h>
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* Illustrated list controller */

IllustratedListController::IllustratedListController(Responder * parentResponder, EditExpressionController * editExpressionController) :
  ListController(parentResponder, editExpressionController, this),
  m_additionalCalculationCells{}
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
  // Reset the context as it was before displaying the IllustratedListController
  Poincare::Context * context = App::app()->localContext();
  if (m_savedExpression.isUninitialized()) {
    /* If no expression was stored in the symbol used by the
     * IllustratedListController, we delete the record we stored */
    char symbolName[3];
    size_t length = UTF8Decoder::CodePointToChars(expressionSymbol(), symbolName, 3);
    assert(length < 3);
    symbolName[length] = 0;
    const char * const extensions[2] = {"exp", "func"};
    Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(symbolName, extensions, 2).destroy();
  } else {
    Poincare::Symbol s = Poincare::Symbol::Builder(expressionSymbol());
    context->setExpressionForSymbolAbstract(m_savedExpression, s);
  }
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
  if (type == 0) {
    return 1;
  }
  return k_maxNumberOfAdditionalCalculations;
}

HighlightCell * IllustratedListController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == 0) {
    return illustrationCell();
  }
  return &m_additionalCalculationCells[index];
}

KDCoordinate IllustratedListController::rowHeight(int j) {
  if (j == 0) {
    return k_illustrationHeight;
  }
  int calculationIndex = j-1;
  if (calculationIndex >= m_calculationStore.numberOfCalculations()) {
    return 0;
  }
  Shared::ExpiringPointer<Calculation> calculation = m_calculationStore.calculationAtIndex(calculationIndex);
  constexpr bool expanded = true;
  KDCoordinate result = calculation->memoizedHeight(expanded);
  if (result < 0) {
    result = ScrollableThreeExpressionsCell::Height(calculation.pointer());
    calculation->setMemoizedHeight(expanded, result);
  }
  return result + Metric::CellSeparatorThickness;
}

int IllustratedListController::typeAtLocation(int i, int j) {
  return j == 0 ? 0 : 1;
}

void IllustratedListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == 0) {
    // TODO ?
    return;
  }
  Poincare::Context * context = App::app()->localContext();
  ScrollableThreeExpressionsCell * myCell = (ScrollableThreeExpressionsCell *)cell;
  Calculation * c = m_calculationStore.calculationAtIndex(index-1).pointer();
  myCell->setCalculation(c);
  myCell->setDisplayCenter(c->displayOutput(context) != Calculation::DisplayOutput::ApproximateOnly);
  //myCell->setHighlighted(myCell->isHighlighted()); //TODO??
}

void IllustratedListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (t->selectedRow() == 0) {
    t->selectCellAtLocation(0, 1);
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
  Poincare::Context * context = App::app()->localContext();
  Poincare::Symbol s = Poincare::Symbol::Builder(expressionSymbol());
  m_savedExpression = context->expressionForSymbolAbstract(s, false);
  context->setExpressionForSymbolAbstract(e, s);
}

int IllustratedListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  ScrollableThreeExpressionsCell * myCell = static_cast<ScrollableThreeExpressionsCell *>(m_listController.selectableTableView()->selectedCell());
  Shared::ExpiringPointer<Calculation> c =  m_calculationStore.calculationAtIndex(index-1);
  const char * text = myCell->selectedSubviewPosition() == ScrollableThreeExpressionsView::SubviewPosition::Right ? c->approximateOutputText(Calculation::NumberOfSignificantDigits::Maximal) : c->exactOutputText();
  return strlcpy(buffer, text, bufferSize);
}

}
