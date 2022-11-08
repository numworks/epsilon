#include "scientific_notation_list_controller.h"
#include "scientific_notation_helper.h"
#include "../app.h"

namespace Calculation {

void ScientificNotationListController::setExactAndApproximateExpression(Poincare::Expression e, Poincare::Expression a) {
  Poincare::Context * context = App::app()->localContext();
  assert(ScientificNotationHelper::HasAdditionalOutputs(a, context));
  m_layouts[0] = ScientificNotationHelper::ScientificLayout(a, context);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}
