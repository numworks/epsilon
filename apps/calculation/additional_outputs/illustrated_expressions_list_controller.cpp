#include "expressions_list_controller.h"
#include "scrollable_three_expressions_cell_with_message.h"
#include "illustrated_expressions_list_controller.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/symbol.h>
#include <apps/shared/poincare_helpers.h>
#include "../app.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

void IllustratedExpressionsListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectCellAtLocation(0, showIllustration());
}

int IllustratedExpressionsListController::numberOfRows() const {
  return ChainedExpressionsListController::numberOfRows() + showIllustration();
}

int IllustratedExpressionsListController::reusableCellCount(int type) {
  assert(type == k_illustrationCellType || type == k_expressionCellType);
  if (type == k_illustrationCellType) {
    return 1;
  }
  return k_maxNumberOfRows;
}

HighlightCell * IllustratedExpressionsListController::reusableCell(int index, int type) {
  assert(type == k_illustrationCellType || type == k_expressionCellType);
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
  ChainedExpressionsListController::willDisplayCellForIndex(cell, index - showIllustration());
}

void IllustratedExpressionsListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (t->selectedRow() == 0 && showIllustration()) {
    t->selectRow(1);
  }
}

void IllustratedExpressionsListController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* But scroll to the top when we select the first
   * ScrollableThreeExpressionsCell in order display the
   * illustration cell. */
  if (t->selectedRow() == 1 && showIllustration()) {
    t->scrollToCell(0, 0);
  }
}

int IllustratedExpressionsListController::textAtIndex(char * buffer, size_t bufferSize, int index) {
  assert(index >= showIllustration());
  return ChainedExpressionsListController::textAtIndex(buffer, bufferSize, index - showIllustration());
}

void IllustratedExpressionsListController::appendLine(int index, Poincare::Expression formula, Poincare::Expression expression, Poincare::Context * context, Poincare::Preferences * preferences) {
  m_layouts[index] = Shared::PoincareHelpers::CreateLayout(formula, context);
  Layout exact = getLayoutFromExpression(expression, context, preferences);
  Layout approximated = getLayoutFromExpression(expression.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit()), context, preferences);
  // Make it editable to have Horiz(CodePoint("-"),CodePoint("1") == String("-1")
  m_exactLayouts[index] = exact.isIdenticalTo(approximated, true) ? Layout() : exact;
  m_approximatedLayouts[index] = approximated;
  index++;
};

}
