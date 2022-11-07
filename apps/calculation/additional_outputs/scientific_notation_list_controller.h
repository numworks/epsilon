#ifndef CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_LIST_CONTROLLER_H

#include "chained_expressions_list_controller.h"

namespace Calculation {

class ScientificNotationListController : public ChainedExpressionsListController {
public:
  static bool HasAdditionalOutputs(Poincare::Expression approximateExpression);

  ScientificNotationListController(EditExpressionController * editExpressionController) :
    ChainedExpressionsListController(editExpressionController, true) {}

  void setExactAndApproximateExpression(Poincare::Expression exactExpression, Poincare::Expression approximateExpression) override;
private:
  static Poincare::Layout ScientificLayout(Poincare::Expression approximateExpression);

  I18n::Message messageAtIndex(int index) override;
};

}

#endif


