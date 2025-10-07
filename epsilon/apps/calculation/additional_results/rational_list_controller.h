#pragma once

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
  I18n::Message messageAtIndex(int index) override { return m_message[index]; }
  Poincare::Layout layoutAtIndex(Escher::HighlightCell* cell,
                                 int index) override;
  constexpr static int k_maxNumberOfOutputRows = 2;
  I18n::Message m_message[k_maxNumberOfOutputRows];
};

}  // namespace Calculation
