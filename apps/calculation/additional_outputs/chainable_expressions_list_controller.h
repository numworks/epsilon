#ifndef CALCULATION_ADDITIONAL_OUTPUTS_CHAINABLE_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_CHAINABLE_EXPRESSIONS_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

/* This class does nothing but ensures that inherited classes will not override
 * MemoizedListViewDataSource methods since it may break ChainedExpressions. */

class ChainableExpressionsListController : public ExpressionsListController {
public:
  using ExpressionsListController::ExpressionsListController;

  // MemoizedListViewDataSource
  int typeAtIndex(int index) const override final { return ExpressionsListController::typeAtIndex(index); }
  int reusableCellCount(int type) override final { return ExpressionsListController::reusableCellCount(type); }
  Escher::HighlightCell * reusableCell(int index, int type) override final { return ExpressionsListController::reusableCell(index, type); }
  KDCoordinate nonMemoizedRowHeight(int j) override { return ExpressionsListController::nonMemoizedRowHeight(j); }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override { return ExpressionsListController::willDisplayCellForIndex(cell, index); }
  int numberOfRows() const override final { return ExpressionsListController::numberOfRows(); };
};

}

#endif
