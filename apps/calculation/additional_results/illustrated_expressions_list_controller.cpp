#include "illustrated_expressions_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/trigonometry.h>

#include "../app.h"
#include "additional_result_cell.h"
#include "expressions_list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

void IllustratedExpressionsListController::didBecomeFirstResponder() {
  selectRow(1);
  ExpressionsListController::didBecomeFirstResponder();
}

void IllustratedExpressionsListController::viewWillAppear() {
  ChainedExpressionsListController::viewWillAppear();
  illustrationCell()->reload();  // compute labels
}

int IllustratedExpressionsListController::numberOfRows() const {
  return ChainedExpressionsListController::numberOfRows() + 1;
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
    return illustrationCell()->isVisible() ? illustrationHeight() : 0;
  }
  AdditionalResultCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void IllustratedExpressionsListController::fillCellForRow(HighlightCell* cell,
                                                          int row) {
  if (typeAtRow(row) == k_illustrationCellType) {
    return;
  }
  assert(typeAtRow(row) == k_expressionCellType);
  ChainedExpressionsListController::fillCellForRow(cell, row - 1);
}

void IllustratedExpressionsListController::
    listViewDidChangeSelectionAndDidScroll(SelectableListView* l,
                                           int previousSelectedRow,
                                           KDPoint previousOffset,
                                           bool withinTemporarySelection) {
  assert(l == m_listController.selectableListView());
  if (!withinTemporarySelection && l->selectedRow() == 1) {
    /* Illustration cell is not selectable so when we select row 1, scroll to
     * the top to display the illustration. */
    l->scrollToCell(0);
  }
}

void IllustratedExpressionsListController::setShowIllustration(
    bool showIllustration) {
  illustrationCell()->setVisible(showIllustration);
  resetSizeMemoization();
}

int IllustratedExpressionsListController::textAtIndex(char* buffer,
                                                      size_t bufferSize,
                                                      HighlightCell* cell,
                                                      int index) {
  assert(index >= 1);
  return ChainedExpressionsListController::textAtIndex(buffer, bufferSize, cell,
                                                       index - 1);
}

void IllustratedExpressionsListController::setLineAtIndex(
    int index, Expression formula, Expression expression,
    const ComputationContext& computationContext) {
  m_layouts[index] = Shared::PoincareHelpers::CreateLayout(
      formula, computationContext.context());
  Layout approximated;
  m_exactLayouts[index] = getExactLayoutFromExpression(
      expression, computationContext, &approximated);
  m_approximatedLayouts[index] = approximated;
};

}  // namespace Calculation
