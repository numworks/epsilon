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
  int typeAtRow(int row) const override final {
    return ExpressionsListController::typeAtRow(row);
  }
  int reusableCellCount(int type) override final {
    return ExpressionsListController::reusableCellCount(type);
  }
  Escher::HighlightCell* reusableCell(int index, int type) override final {
    return ExpressionsListController::reusableCell(index, type);
  }
  KDCoordinate nonMemoizedRowHeight(int row) override {
    return ExpressionsListController::nonMemoizedRowHeight(row);
  }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override {
    return ExpressionsListController::fillCellForRow(cell, row);
  }
  int numberOfRows() const override final {
    return ExpressionsListController::numberOfRows();
  };
};

}  // namespace Calculation

#endif
