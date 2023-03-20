#include "expressions_list_controller.h"

#include "../app.h"
#include "additionnal_result_cell.h"

using namespace Poincare;
using namespace Escher;

namespace Calculation {

/* Expressions list controller */

ExpressionsListController::ExpressionsListController(
    EditExpressionController *editExpressionController,
    bool highlightWholeCells, Escher::SelectableListViewDelegate *delegate)
    : ListController(editExpressionController, delegate) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].label()->setParentResponder(
        m_listController.selectableListView());
    m_cells[i].setHighlightWholeCell(highlightWholeCells);
  }
}

void ExpressionsListController::didBecomeFirstResponder() {
  selectCell(0);
  ListController::didBecomeFirstResponder();
}

int ExpressionsListController::reusableCellCount(int type) {
  return k_maxNumberOfRows;
}

void ExpressionsListController::viewDidDisappear() {
  ListController::viewDidDisappear();
  // Reset layout and cell memoization to avoid taking extra space in the pool
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].label()->resetMemoization();
    /* By reseting m_layouts, numberOfRow will go down to 0, and the highlighted
     * cells won't be unselected. Therefore we unselect them here. */
    m_cells[i].setHighlighted(false);
    m_layouts[i] = Layout();
    m_exactLayouts[i] = Layout();
    m_approximatedLayouts[i] = Layout();
  }
  m_expression = Expression();
}

HighlightCell *ExpressionsListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate ExpressionsListController::nonMemoizedRowHeight(int index) {
  AdditionnalResultCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

void ExpressionsListController::willDisplayCellForIndex(HighlightCell *cell,
                                                        int index) {
  /* Note : To further optimize memoization space in the pool, layout
   * serialization could be memoized instead, and layout would be recomputed
   * here, when setting cell's layout. */
  AdditionnalResultCell *myCell = static_cast<AdditionnalResultCell *>(cell);
  myCell->label()->setLayouts(m_layouts[index], m_exactLayouts[index],
                              m_approximatedLayouts[index]);
  myCell->subLabel()->setMessage(messageAtIndex(index));
}

int ExpressionsListController::numberOfRows() const {
  int nbOfRows = 0;
  for (size_t i = 0; i < k_maxNumberOfRows; i++) {
    if (!m_layouts[i].isUninitialized() ||
        !m_exactLayouts[i].isUninitialized() ||
        !m_approximatedLayouts[i].isUninitialized()) {
      nbOfRows++;
    }
  }
  return nbOfRows;
}

void ExpressionsListController::setExpression(Poincare::Expression e) {
  // Reinitialize memoization
  resetMemoization();
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_layouts[i] = Layout();
    m_exactLayouts[i] = Layout();
    m_approximatedLayouts[i] = Layout();
    m_cells[i].label()->setSelectedSubviewPosition(
        ScrollableThreeExpressionsView::SubviewPosition::Left);
  }
  m_expression = e;
}

int ExpressionsListController::textAtIndex(char *buffer, size_t bufferSize,
                                           HighlightCell *cell, int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  ScrollableThreeExpressionsView::SubviewPosition position =
      static_cast<AdditionnalResultCell *>(cell)
          ->label()
          ->selectedSubviewPosition();
  Layout layout;
  if (position == ScrollableThreeExpressionsView::SubviewPosition::Left) {
    // If the line is formula = exact, insert exact instead of formula
    if (!m_exactLayouts[index].isUninitialized()) {
      layout = m_exactLayouts[index];
    } else {
      layout = m_layouts[index];
    }
  } else if (position ==
             ScrollableThreeExpressionsView::SubviewPosition::Center) {
    layout = m_exactLayouts[index];
  } else {
    assert(position == ScrollableThreeExpressionsView::SubviewPosition::Right);
    layout = m_approximatedLayouts[index];
  }
  assert(!layout.isUninitialized());
  return layout.serializeParsedExpression(buffer, bufferSize,
                                          App::app()->localContext());
}

Poincare::Layout ExpressionsListController::getLayoutFromExpression(
    Expression e, Context *context, Poincare::Preferences *preferences) {
  assert(!e.isUninitialized());
  // Simplify or approximate expression
  Expression approximateExpression;
  Expression simplifiedExpression;
  Shared::PoincareHelpers::CloneAndSimplifyAndApproximate(
      e, &simplifiedExpression, &approximateExpression, context,
      SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      UnitConversion::Default, preferences);
  // simplify might have been interrupted, in which case we use approximate
  if (simplifiedExpression.isUninitialized()) {
    assert(!approximateExpression.isUninitialized());
    return Shared::PoincareHelpers::CreateLayout(approximateExpression,
                                                 context);
  }
  return Shared::PoincareHelpers::CreateLayout(simplifiedExpression, context);
}

}  // namespace Calculation
