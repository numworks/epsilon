#include "illustrated_expressions_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"
#include "additional_result_cell.h"
#include "expressions_list_controller.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

void IllustratedExpressionsListController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::BecameFirst) {
    selectRow(1);
  }
  ChainedExpressionsListController::handleResponderChainEvent(event);
}

void IllustratedExpressionsListController::viewWillAppear() {
  ChainedExpressionsListController::viewWillAppear();
  illustrationCell()->reload();  // compute labels
}

int IllustratedExpressionsListController::numberOfRows() const {
  return ChainedExpressionsListController::numberOfRows() + 1;
}

int IllustratedExpressionsListController::reusableCellCount(int type) const {
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

Layout IllustratedExpressionsListController::layoutAtIndex(HighlightCell* cell,
                                                           int index) {
  if (index == 0) {
    // Illustration cell does not have a text
    return Layout();
  }
  return ChainedExpressionsListController::layoutAtIndex(cell, index - 1);
}

// Create layout for formula, simplified expression and approximated expression.
void IllustratedExpressionsListController::setLineAtIndex(
    int index, const UserExpression formula, const UserExpression expression,
    const Internal::ProjectionContext* ctx) {
  m_layouts[index] =
      Shared::PoincareHelpers::CreateLayout(formula, ctx->m_context);
  m_exactLayouts[index] = GetExactLayoutFromExpression(
      expression, ctx, &(m_approximatedLayouts[index]),
      &(m_isStrictlyEqual[index]));
}

}  // namespace Calculation
