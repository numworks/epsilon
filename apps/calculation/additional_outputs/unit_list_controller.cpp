#include "unit_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"

namespace Calculation {

int UnitListController::numberOfRows() const { //FIXME
  return 2;
}

void UnitListController::computeLayoutAtIndex(int index) { //FIXME
  Poincare::Expression expressionAtIndex;
  if (index == 0) {
    expressionAtIndex = m_expression;
  } else {
    assert(index == 1);
    Poincare::ExpressionNode::ReductionContext reductionContext(
        App::app()->localContext(),
        Poincare::Preferences::sharedPreferences()->complexFormat(),
        Poincare::Preferences::sharedPreferences()->angleUnit(),
        Poincare::ExpressionNode::ReductionTarget::SystemForApproximation,
        Poincare::ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    expressionAtIndex = m_expression.reduce(reductionContext);
  }
  m_layouts[index] = Shared::PoincareHelpers::CreateLayout(expressionAtIndex);
}

I18n::Message UnitListController::messageAtIndex(int index) {
  return (I18n::Message)0;
}

}
