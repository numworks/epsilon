#include "scientific_notation_list_controller.h"

#include "../app.h"
#include "scientific_notation_helper.h"

using namespace Poincare;

namespace Calculation {

void ScientificNotationListController::computeAdditionalResults(
    Expression exactExpression, Expression approximateExpression) {
  Context* context = App::app()->localContext();
  assert(ScientificNotationHelper::HasAdditionalOutputs(approximateExpression,
                                                        context));
  m_layouts[0] = ScientificNotationHelper::ScientificLayout(
      approximateExpression, context);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}  // namespace Calculation
