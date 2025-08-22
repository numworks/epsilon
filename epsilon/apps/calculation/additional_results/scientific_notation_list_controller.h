#pragma once

#include "chained_expressions_list_controller.h"

namespace Calculation {

class ScientificNotationListController
    : public ChainedExpressionsListController {
 public:
  ScientificNotationListController(
      EditExpressionController* editExpressionController)
      : ChainedExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override;
};

}  // namespace Calculation
