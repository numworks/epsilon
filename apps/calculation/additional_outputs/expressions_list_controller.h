#ifndef CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H

#include <poincare/expression.h>
#include <apps/i18n.h>
#include "scrollable_three_expressions_cell_with_message.h"
#include "list_controller.h"

namespace Calculation {

class ExpressionsListController : public ListController {
public:
  ExpressionsListController(EditExpressionController * editExpressionController, Escher::SelectableTableViewDelegate * delegate = nullptr);

  // Responder
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) override;

protected:
  constexpr static int k_maxNumberOfRows = 5;
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  Poincare::Layout getLayoutFromExpression(Poincare::Expression e, Poincare::Context * context, Poincare::Preferences * preferences);
  Poincare::Expression m_expression;
  // Memoization of layouts
  mutable Poincare::Layout m_layouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_exactLayouts[k_maxNumberOfRows];
  mutable Poincare::Layout m_approximatedLayouts[k_maxNumberOfRows];
  ScrollableThreeExpressionsCellWithMessage m_cells[k_maxNumberOfRows];
private:
  virtual I18n::Message messageAtIndex(int index) = 0;
  // Cells
};


}

#endif
