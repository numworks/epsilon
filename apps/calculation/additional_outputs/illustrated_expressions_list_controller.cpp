#include "illustrated_expressions_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/symbol.h>

#include "../app.h"
#include "additionnal_result_cell.h"
#include "expressions_list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

void IllustratedExpressionsListController::didBecomeFirstResponder() {
  selectCell(showIllustration());
  ExpressionsListController::didBecomeFirstResponder();
}

void IllustratedExpressionsListController::viewWillAppear() {
  ChainedExpressionsListController::viewWillAppear();
  illustrationCell()->reload();  // compute labels
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

KDCoordinate IllustratedExpressionsListController::nonMemoizedRowHeight(
    int row) {
  if (typeAtRow(row) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  AdditionnalResultCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void IllustratedExpressionsListController::fillCellForRow(HighlightCell* cell,
                                                          int row) {
  if (typeAtRow(row) == k_illustrationCellType) {
    return;
  }
  ChainedExpressionsListController::fillCellForRow(cell,
                                                   row - showIllustration());
}

void IllustratedExpressionsListController::
    listViewDidChangeSelectionAndDidScroll(SelectableListView* l,
                                           int previousSelectedRow,
                                           KDPoint previousOffset,
                                           bool withinTemporarySelection) {
  assert(l == m_listController.selectableListView());
  if (!withinTemporarySelection && l->selectedRow() == 1 &&
      showIllustration()) {
    /* Illustration cell is not selectable so when we select row 1, scroll to
     * the top to display the illustration. */
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
  /* Make it editable to have Horiz(CodePoint("-"),CodePoint("1") ==
   * String("-1") */
  m_exactLayouts[index] = exact;
  m_approximatedLayouts[index] =
      exact.isIdenticalTo(approximated, true) ? Layout() : approximated;
};

}  // namespace Calculation
