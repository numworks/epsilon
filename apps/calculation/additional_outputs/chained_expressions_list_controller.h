#ifndef CALCULATION_ADDITIONAL_OUTPUTS_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H

#include <poincare/expression.h>
#include <apps/i18n.h>
#include "expressions_list_controller.h"

namespace Calculation {

// Allows an expression list to be continued with another expression list

class ChainedExpressionsListController : public ExpressionsListController {
public:
  ChainedExpressionsListController(EditExpressionController * editExpressionController, Escher::SelectableTableViewDelegate * delegate = nullptr);
  void setTail(ExpressionsListController * tail) { m_tail = tail; }

  // Responder
  void viewDidDisappear() override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override;

  // ListController
  void setExpression(Poincare::Expression e) override;
protected:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
private:
  ExpressionsListController * m_tail;
};

}

#endif
