#ifndef CALCULATION_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_CHAINED_EXPRESSIONS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <poincare/layout.h>

#include "chainable_expressions_list_controller.h"

namespace Calculation {

// Allows an expression list to be continued with another expression list

class ChainedExpressionsListController : public ExpressionsListController {
 public:
  ChainedExpressionsListController(
      EditExpressionController* editExpressionController,
      bool highlightWholeCells,
      Escher::SelectableListViewDelegate* delegate = nullptr)
      : ExpressionsListController(editExpressionController, highlightWholeCells,
                                  delegate),
        m_tail(nullptr) {}

  void setTail(ChainableExpressionsListController* tail) { m_tail = tail; }

  // Responder
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;

  // ListViewDataSource
  int typeAtRow(int row) const override;
  void initWidth(Escher::TableView* tableView) override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int numberOfRows() const override;

 protected:
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;

 private:
  ChainableExpressionsListController* m_tail;
};

}  // namespace Calculation

#endif
