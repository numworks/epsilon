#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class UnitListController : public ExpressionsListController {
public:
  UnitListController(EditExpressionController * editExpressionController);

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;

  void setExpression(Poincare::Expression e) override;
  int reusableCellCount(int type) override { return type == 0 ? ExpressionsListController::reusableCellCount(type) : 1; }
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int numberOfRows() const override;

private:
  I18n::Message messageAtIndex(int index) override;
  I18n::Message m_dimensionMessage;
  MessageTableCell<> m_dimensionCell;
};

}

#endif
