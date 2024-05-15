#ifndef CALCULATION_RATIONAL_LIST_CONTROLLER_H
#define CALCULATION_RATIONAL_LIST_CONTROLLER_H

#include "chainable_expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ChainableExpressionsListController {
 public:
  RationalListController(EditExpressionController* editExpressionController)
      : ChainableExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::UserExpression input,
      const Poincare::UserExpression exactOutput,
      const Poincare::UserExpression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override;
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;
};

}  // namespace Calculation

#endif
