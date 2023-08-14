#ifndef CALCULATION_RATIONAL_LIST_CONTROLLER_H
#define CALCULATION_RATIONAL_LIST_CONTROLLER_H

#include "chainable_expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ChainableExpressionsListController {
 public:
  RationalListController(EditExpressionController* editExpressionController)
      : ChainableExpressionsListController(editExpressionController, true) {}

  void computeAdditionalResults(
      const Poincare::Expression input, const Poincare::Expression exactOutput,
      const Poincare::Expression approximateOutput) override;

 private:
  I18n::Message messageAtIndex(int index) override;
  int textAtIndex(char* buffer, size_t bufferSize, Escher::HighlightCell* cell,
                  int index) override;
};

}  // namespace Calculation

#endif
