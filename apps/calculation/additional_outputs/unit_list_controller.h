#ifndef CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_UNIT_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class UnitListController : public ExpressionsListController {
public:
  UnitListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(editExpressionController) {}

  void setExpression(Poincare::Expression e) override;

private:
  I18n::Message messageAtIndex(int index) override;
};

}

#endif
