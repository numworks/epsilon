#ifndef CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_EXPRESSIONS_LIST_CONTROLLER_H

#include <escher.h>
#include <poincare/expression.h>
#include <apps/i18n.h>
#include "list_controller.h"

namespace Calculation {

class ExpressionsListController : public ListController {
public:
  ExpressionsListController(Responder * parentResponder);

  // Responder
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  //ListViewDataSource
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override { return 0; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  // IllustratedListController
  void setExpression(Poincare::Expression e) override { m_expression = e; }

protected:
  Poincare::Expression m_expression;
private:
  virtual Poincare::Layout layoutAtIndex(int index) = 0;
  virtual I18n::Message messageAtIndex(int index) = 0;
  constexpr static int k_maxNumberOfCells = 4;
  // Cells
  ExpressionTableCellWithPointer m_cells[k_maxNumberOfCells];
};

}

#endif

