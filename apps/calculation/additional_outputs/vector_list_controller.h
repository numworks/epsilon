#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_LIST_CONTROLLER_H

#include "expressions_list_controller.h"

namespace Calculation {

class VectorListController : public ExpressionsListController {
public:
  VectorListController(EditExpressionController * editExpressionController) :
    ExpressionsListController(editExpressionController) {}

  void setExpression(Poincare::Expression e) override;

private:
  I18n::Message messageAtIndex(int index) override;
  Poincare::Layout getLayoutFromExpression(Poincare::Expression e, Poincare::Context * context, Poincare::Preferences * preferences);
  // Map from cell index to message index
  constexpr static int k_maxNumberOfOutputRows = 3;
  int m_indexMessageMap[k_maxNumberOfOutputRows];
};

}

#endif


