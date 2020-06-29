#ifndef CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H

#include <escher.h>
#include <poincare/expression.h>
#include <apps/i18n.h>
#include "list_controller.h"

namespace Calculation {

class ExpressionsListController : public ListController {
public:
  ExpressionsListController(EditExpressionController * editExpressionController);

  // Responder
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  //ListViewDataSource
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int numberOfRows() const override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) override;

protected:
  constexpr static int k_maxNumberOfRows = 5;
  int textAtIndex(char * buffer, size_t bufferSize, int index) override;
  Poincare::Expression m_expression;
  // Memoization of layouts
  mutable Poincare::Layout m_layouts[k_maxNumberOfRows];
private:
  Poincare::Layout layoutAtIndex(int index);
  virtual I18n::Message messageAtIndex(int index) = 0;
  // Cells
  ExpressionTableCellWithPointer m_cells[k_maxNumberOfRows];
};

}

#endif

