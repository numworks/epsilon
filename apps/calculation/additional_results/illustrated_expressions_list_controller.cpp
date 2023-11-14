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
  if (type == k_illustrationCellType) {
    return 1;
  }
  assert(type == k_expressionCellType);
  return k_maxNumberOfRows;
}

HighlightCell* IllustratedExpressionsListController::reusableCell(int index,
                                                                  int type) {
  if (type == k_illustrationCellType) {
    assert(index == 0);
    return illustrationCell();
  }
  assert(type == k_expressionCellType);
  assert(0 <= index && index < k_maxNumberOfRows);
  return &m_cells[index];
}

KDCoordinate IllustratedExpressionsListController::nonMemoizedRowHeight(
    int row) {
  if (typeAtRow(row) == k_illustrationCellType) {
    return illustrationCell()->isVisible() ? illustrationHeight() : 0;
  }
  assert(typeAtRow(row) == k_expressionCellType);
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

void IllustratedExpressionsListController::setShowIllustration(
    bool showIllustration) {
  illustrationCell()->setVisible(showIllustration);
  m_listController.selectableListView()->resetSizeAndOffsetMemoization();
}

int IllustratedExpressionsListController::textAtIndex(char* buffer,
                                                      size_t bufferSize,
                                                      HighlightCell* cell,
                                                      int index) {
  if (index == 0) {
    // Illustration cell does not have a text
    buffer[0] = 0;
    return 0;
  }
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
