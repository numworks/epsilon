#ifndef CALCULATION_ADDITIONAL_OUTPUTS_SECOND_DEGREE_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_SECOND_DEGREE_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class SecondDegreeListController : public ExpressionsListController {
public:
  SecondDegreeListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(editExpressionController),
    m_numberOfSolutions(0) {}

  void setExpression(Poincare::Expression e) override;

private:
  Poincare::Expression getOppositeIfExists(Poincare::Expression e, Poincare::ExpressionNode::ReductionContext * reductionContext);
  I18n::Message messageAtIndex(int index) override;
  int m_numberOfSolutions;
};

}

#endif


