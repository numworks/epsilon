#include "expressions_list_controller.h"

#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"
#include "../edit_expression_controller.h"

using namespace Poincare;
using namespace Escher;
using namespace Shared;

namespace Calculation {

/* Inner List Controller */

ExpressionsListController::InnerListController::InnerListController(
    ExpressionsListController* dataSource, SelectableListViewDelegate* delegate)
    : ViewController(dataSource),
      m_selectableListView(this, dataSource, dataSource, delegate) {
  m_selectableListView.resetMargins();
  m_selectableListView.hideScrollBars();
}

void ExpressionsListController::InnerListController::didBecomeFirstResponder() {
  m_selectableListView.reloadData();
}

/* List Controller */

ExpressionsListController::ExpressionsListController(
    EditExpressionController* editExpressionController,
    bool highlightWholeCells, SelectableListViewDelegate* delegate)
    : StackViewController::Default(nullptr, &m_listController,
                                   StackViewController::Style::PurpleWhite),
      m_listController(this, delegate),
      m_editExpressionController(editExpressionController) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].label()->setParentResponder(
        m_listController.selectableListView());
    m_cells[i].setHighlightWholeCell(highlightWholeCells);
  }
}

bool ExpressionsListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() >= 0);
    HighlightCell* cell =
        m_listController.selectableListView()->cell(selectedRow());
    Layout layout = layoutAtIndex(cell, selectedRow());
    App::app()->modalViewController()->dismissModal();
    m_editExpressionController->insertLayout(layout);
    return true;
  }
  return false;
}

void ExpressionsListController::didBecomeFirstResponder() {
  selectRow(0);
  m_listController.selectableListView()->resetSizeAndOffsetMemoization();
  App::app()->setFirstResponder(&m_listController);
  // Additional outputs should have at least one row to display
  assert(numberOfRows() > 0);
}

void ExpressionsListController::viewDidDisappear() {
  tidy();
  StackViewController::viewDidDisappear();
}

void ExpressionsListController::tidy() {
  // Reset layout and cell memoization to avoid taking extra space in the pool
  m_listController.selectableListView()->resetSizeAndOffsetMemoization();
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_cells[i].label()->resetLayouts();
    /* By reseting m_layouts, numberOfRow will go down to 0, and the highlighted
     * cells won't be unselected. Therefore we unselect them here. */
    m_cells[i].setHighlighted(false);
    m_layouts[i] = Layout();
    m_exactLayouts[i] = Layout();
    m_approximatedLayouts[i] = Layout();
    m_isStrictlyEqual[i] = false;
  }
}

int ExpressionsListController::reusableCellCount(int type) const {
  return k_maxNumberOfRows;
}

HighlightCell* ExpressionsListController::reusableCell(int index, int type) {
  return &m_cells[index];
}

KDCoordinate ExpressionsListController::nonMemoizedRowHeight(int row) {
  AdditionalResultCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void ExpressionsListController::fillCellForRow(HighlightCell* cell, int row) {
  /* Note : To further optimize memoization space in the pool, layout
   * serialization could be memoized instead, and layout would be recomputed
   * here, when setting cell's layout. */
  AdditionalResultCell* myCell = static_cast<AdditionalResultCell*>(cell);
  myCell->label()->setLayouts(m_layouts[row], m_exactLayouts[row],
                              m_approximatedLayouts[row]);
  myCell->subLabel()->setMessage(messageAtIndex(row));
  myCell->label()->setExactAndApproximateAreStriclyEqual(
      m_isStrictlyEqual[row]);
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

Poincare::Layout ExpressionsListController::layoutAtIndex(HighlightCell* cell,
                                                          int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  ScrollableThreeLayoutsView::SubviewPosition position =
      static_cast<AdditionalResultCell*>(cell)
          ->label()
          ->selectedSubviewPosition();
  Layout layout;
  if (position == ScrollableThreeLayoutsView::SubviewPosition::Left) {
    // If the line is formula = exact, insert exact instead of formula
    if (!m_exactLayouts[index].isUninitialized()) {
      layout = m_exactLayouts[index];
    } else {
      layout = m_layouts[index];
    }
  } else if (position == ScrollableThreeLayoutsView::SubviewPosition::Center) {
    layout = m_exactLayouts[index];
  } else {
    assert(position == ScrollableThreeLayoutsView::SubviewPosition::Right);
    layout = m_approximatedLayouts[index];
  }
  assert(!layout.isUninitialized());
  return layout;
}

Layout ExpressionsListController::GetExactLayoutFromExpression(
    const UserExpression e, const Internal::ProjectionContext* ctx,
    Layout* approximate, bool* isStrictlyEqual) {
  assert(!e.isUninitialized());
  UserExpression approximateExpression, exactExpression;
  Internal::ProjectionContext tempCtx = *ctx;
  tempCtx.m_symbolic =
      SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
  e.cloneAndSimplifyAndApproximate(&exactExpression, &approximateExpression,
                                   &tempCtx);
  assert(!exactExpression.isUninitialized() &&
         !approximateExpression.isUninitialized());
  Layout approximateLayout = Shared::PoincareHelpers::CreateLayout(
      approximateExpression, ctx->m_context);
  Layout exactLayout = exactExpression.isUninitialized()
                           ? approximateLayout
                           : Shared::PoincareHelpers::CreateLayout(
                                 exactExpression, ctx->m_context);
  if (approximate) {
    // TODO_PCJ: Factorize with ExpressionDisplayPermissions
    if (approximateExpression.isUndefined()) {
      // Hide exact layout if approximation is undef (e.g tan(1.5707963267949))
      exactLayout = Layout();
      *approximate = approximateLayout;
    }
    /* Make it editable to compare equivalent layouts. */
    else if (exactLayout.isIdenticalTo(approximateLayout, true)) {
      *approximate = Layout();
    } else {
      *approximate = approximateLayout;
      if (isStrictlyEqual) {
        *isStrictlyEqual =
            Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
                exactExpression, approximateExpression, ctx);
      }
    }
  }
  return exactLayout;
}

}  // namespace Calculation
