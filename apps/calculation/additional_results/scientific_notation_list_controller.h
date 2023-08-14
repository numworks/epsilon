#ifndef CALCULATION_SCIENTIFIC_NOTATION_LIST_CONTROLLER_H
#define CALCULATION_SCIENTIFIC_NOTATION_LIST_CONTROLLER_H

#include "chained_expressions_list_controller.h"

namespace Calculation {

class ScientificNotationListController
    : public ChainedExpressionsListController {
 public:
  ScientificNotationListController(
      EditExpressionController* editExpressionController)
      : ChainedExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput,
      const Poincare::Expression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override;
};

}  // namespace Calculation

#endif
