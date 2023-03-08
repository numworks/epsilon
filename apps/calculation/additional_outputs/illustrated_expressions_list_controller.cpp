#include "illustrated_expressions_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/symbol.h>

#include "../app.h"
#include "expressions_list_controller.h"
#include "scrollable_three_expressions_cell_with_message.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

void IllustratedExpressionsListController::didBecomeFirstResponder() {
  selectCell(showIllustration());
  ListController::didBecomeFirstResponder();
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

HighlightCell* IllustratedExpressionsListController::reusableCell(int index,
                                                                  int type) {
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

void IllustratedExpressionsListController::willDisplayCellForIndex(
    HighlightCell* cell, int index) {
  if (typeAtIndex(index) == k_illustrationCellType) {
    return;
  }
  ChainedExpressionsListController::willDisplayCellForIndex(
      cell, index - showIllustration());
}

void IllustratedExpressionsListController::listViewDidChangeSelection(
    SelectableListView* l, int previousSelectedRow,
    bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (l->selectedRow() == 0 && showIllustration()) {
    l->selectRow(1);
  }
}

void IllustratedExpressionsListController::
    listViewDidChangeSelectionAndDidScroll(SelectableListView* l,
                                           int previousSelectedRow,
                                           bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* But scroll to the top when we select the first
   * ScrollableThreeExpressionsCell in order display the
   * illustration cell. */
  if (l->selectedRow() == 1 && showIllustration()) {
    l->scrollToCell(0);
  }
}

int IllustratedExpressionsListController::textAtIndex(char* buffer,
                                                      size_t bufferSize,
                                                      HighlightCell* cell,
                                                      int index) {
  assert(index >= showIllustration());
  return ChainedExpressionsListController::textAtIndex(
      buffer, bufferSize, cell, index - showIllustration());
}

void IllustratedExpressionsListController::setLineAtIndex(
    int index, Poincare::Expression formula, Poincare::Expression expression,
    Poincare::Context* context, Poincare::Preferences* preferences) {
  m_layouts[index] = Shared::PoincareHelpers::CreateLayout(formula, context);
  Layout exact = getLayoutFromExpression(expression, context, preferences);
  Layout approximated = getLayoutFromExpression(
      expression.approximate<double>(context, preferences->complexFormat(),
                                     preferences->angleUnit()),
      context, preferences);
  // Make it editable to have Horiz(CodePoint("-"),CodePoint("1") ==
  // String("-1")
  m_exactLayouts[index] = exact;
  m_approximatedLayouts[index] =
      exact.isIdenticalTo(approximated, true) ? Layout() : approximated;
};

}  // namespace Calculation
