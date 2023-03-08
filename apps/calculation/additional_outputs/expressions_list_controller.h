#ifndef CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <poincare/expression.h>

#include "list_controller.h"
#include "scrollable_three_expressions_cell_with_message.h"

namespace Calculation {

class ExpressionsListController : public ListController {
 public:
  ExpressionsListController(
      EditExpressionController* editExpressionController,
      bool highlightWholeCells,
      Escher::SelectableListViewDelegate* delegate = nullptr);

  // Responder
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;

  // MemoizedListViewDataSource
  int reusableCellCount(int type) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  int numberOfRows() const override;

  // ListController
  void setExpression(Poincare::Expression e) override;
  int textAtIndex(char* buffer, size_t bufferSize, Escher::HighlightCell* cell,
                  int index) override;

 protected:
  constexpr static int k_expressionCellType = 0;
  constexpr static int k_maxNumberOfRows = 5;
  Poincare::Layout getLayoutFromExpression(Poincare::Expression e,
                                           Poincare::Context* context,
                                           Poincare::Preferences* preferences);
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

}  // namespace Calculation

#endif
