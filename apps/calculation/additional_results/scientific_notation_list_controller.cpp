#include "scientific_notation_list_controller.h"

#include <poincare/additional_results_helper.h>

#include "../app.h"

using namespace Poincare;

namespace Calculation {

void ScientificNotationListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasScientificNotation(
      approximateOutput, m_calculationPreferences));
  Context* context = App::app()->localContext();
  m_layouts[0] = AdditionalResultsHelper::ScientificLayout(
      approximateOutput, context, m_calculationPreferences);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}  // namespace Calculation
