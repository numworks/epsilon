#ifndef CALCULATION_ADDITIONAL_OUTPUTS_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H

#include <poincare/expression.h>
#include <apps/i18n.h>
#include "chainable_expressions_list_controller.h"

namespace Calculation {

// Allows an expression list to be continued with another expression list

class ChainedExpressionsListController : public ExpressionsListController {
public:
  ChainedExpressionsListController(EditExpressionController * editExpressionController, Escher::SelectableTableViewDelegate * delegate = nullptr);
  void setTail(ChainableExpressionsListController * tail) { m_tail = tail; }

  // Responder
  void viewDidDisappear() override;

  // ListViewDataSource
  int typeAtIndex(int index) const override;
  void initCellSize(Escher::TableView * view) override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override;
protected:
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
private:
  ChainableExpressionsListController * m_tail;
};

}

#endif
