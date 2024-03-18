#include "function_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <ion/unicode/code_point.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/vertical_offset_layout.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void FunctionListController::computeAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  assert(AdditionalResultsType::HasFunction(input, approximateOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context* context = App::app()->localContext();

  Expression inputClone = input.clone();
  float abscissa = inputClone.getNumericalValue();
  Symbol variable = Symbol::SystemSymbol();
  inputClone.replaceNumericalValuesWithSymbol(variable);

  Expression simplifiedExpression = inputClone;
  PoincareHelpers::CloneAndSimplify(
      &simplifiedExpression, context,
      {.complexFormat = complexFormat(),
       .angleUnit = angleUnit(),
       .target = ReductionTarget::SystemForApproximation});

  /* Use the approximate expression to compute the ordinate to ensure that
   * it's coherent with the output of the calculation.
   * Sometimes when the reduction has some mistakes, the approximation of
   * simplifiedExpression(abscissa) can differ for the approximateOutput.
   */
  float ordinate = PoincareHelpers::ApproximateToScalar<float>(
      approximateOutput, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  m_model.setParameters(simplifiedExpression, abscissa, ordinate);

  m_layouts[0] = HorizontalLayout::Builder(
      Layout::String("y="),
      inputClone
          .replaceSymbolWithExpression(variable, Symbol::Builder(k_symbol))
          .createLayout(displayMode(), numberOfSignificantDigits(), context));
  setShowIllustration(true);
}

void FunctionListController::viewDidDisappear() {
  IllustratedExpressionsListController::viewDidDisappear();
  m_model.tidy();
}

I18n::Message FunctionListController::messageAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::CurveEquation;
}

}  // namespace Calculation
