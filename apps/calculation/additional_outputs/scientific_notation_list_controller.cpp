#include "scientific_notation_list_controller.h"
#include "scientific_notation_helper.h"

namespace Calculation {

void ScientificNotationListController::setExactAndApproximateExpression(Poincare::Expression e, Poincare::Expression a) {
  assert(ScientificNotationHelper::HasAdditionalOutputs(a));
  m_layouts[0] = ScientificNotationHelper::ScientificLayout(a);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}
