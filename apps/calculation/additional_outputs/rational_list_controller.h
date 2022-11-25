#ifndef CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_RATIONAL_LIST_CONTROLLER_H

#include "chainable_expressions_list_controller.h"

namespace Calculation {

class RationalListController : public ChainableExpressionsListController {
public:
  RationalListController(EditExpressionController * editExpressionController) :
    ChainableExpressionsListController(editExpressionController, true) {}

  void setExpression(Poincare::Expression e) override;

private:
  I18n::Message messageAtIndex(int index) override;
  int textAtIndex(char * buffer, size_t bufferSize, Escher::HighlightCell * cell, int index) override;
};

}

#endif


