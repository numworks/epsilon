#include "scientific_notation_list_controller.h"

#include <poincare/additional_results_helper.h>

#include "../app.h"

using namespace Poincare;

namespace Calculation {

void ScientificNotationListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  Context* context = App::app()->localContext();
  assert(AdditionalResultsType::HasScientificNotation(
      approximateOutput, m_calculationPreferences, context));
  m_layouts[0] = AdditionalResultsHelper::ScientificLayout(
                     approximateOutput, context, m_calculationPreferences)
                     .cloneAndTurnEToTenPowerLayout(false);
}

I18n::Message ScientificNotationListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::ScientificNotation;
}

}  // namespace Calculation
