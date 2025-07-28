#ifndef CALCULATION_INTEGER_LIST_CONTROLLER_H
#define CALCULATION_INTEGER_LIST_CONTROLLER_H

#include "chainable_expressions_list_controller.h"

namespace Calculation {

class IntegerListController : public ChainableExpressionsListController {
 public:
  IntegerListController(EditExpressionController* editExpressionController)
      : ChainableExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  constexpr static int k_indexOfFactorExpression = 3;
  I18n::Message messageAtIndex(int index) override;
};

}  // namespace Calculation

#endif
