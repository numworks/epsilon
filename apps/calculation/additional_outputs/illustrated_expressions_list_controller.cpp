#include "expressions_list_controller.h"
#include "scrollable_three_expressions_cell_with_message.h"
#include "illustrated_expressions_list_controller.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/symbol.h>
#include "../app.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

IllustratedExpressionsListController::IllustratedExpressionsListController(EditExpressionController * editExpressionController) :
  ExpressionsListController(editExpressionController, this)
{
}

void IllustratedExpressionsListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, showIllustration());
}

int IllustratedExpressionsListController::numberOfRows() const {
  return ExpressionsListController::numberOfRows() + showIllustration();
}

int IllustratedExpressionsListController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == k_illustrationCellType) {
    return 1;
  }
  return k_maxNumberOfRows;
}

HighlightCell * IllustratedExpressionsListController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == k_illustrationCellType) {
    return illustrationCell();
  }
  return &m_cells[index];
}

KDCoordinate IllustratedExpressionsListController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  ScrollableThreeExpressionsCellWithMessage tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

void IllustratedExpressionsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_illustrationCellType) {
    return;
  }
  ExpressionsListController::willDisplayCellForIndex(cell, index - showIllustration());
}

void IllustratedExpressionsListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (t->selectedRow() == 0 && showIllustration()) {
    t->selectCellAtLocation(0, 1);
  }
  /* But scroll to the top when we select the first
   * ScrollableThreeExpressionsCell in order display the
   * illustration cell. */
  if (t->selectedRow() == 1) {
    t->scrollToCell(0, 0);
  }
}

Poincare::VariableContext IllustratedExpressionsListController::illustratedExpressionsListContext() {
  // Create variable context containing expression for symbol
  VariableContext context = VariableContext(symbol(), App::app()->localContext());
  assert(!m_expression.isUninitialized() && !m_expression.wasErasedByException());
  context.setExpressionForSymbolAbstract(m_expression, Poincare::Symbol::Builder(symbol(), strlen(symbol())));
  return context;
}

int IllustratedExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  assert(index > 0);
  return ExpressionsListController::textAtIndex(buffer, bufferSize, index - showIllustration());
}

}
