#include "function_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/code_point.h>
#include <poincare/additional_results_helper.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/user_expression.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void FunctionListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasFunction(input, approximateOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  float abscissa;
  UserExpression inputClone =
      AdditionalResultsHelper::CloneReplacingNumericalValuesWithSymbol(
          input, k_symbolName, &abscissa);

  PreparedFunction simplifiedExpression =
      PoincareHelpers::CloneAndReduce(
          inputClone, App::app()->localContext(), complexFormat(), angleUnit(),
          false, ReductionTarget::User,
          SymbolicComputation::ReplaceDefinedSymbols)
          .getPreparedFunction(k_symbolName, true);
  assert(!simplifiedExpression.isUninitialized() &&
         !simplifiedExpression.isFailedSimplification());

  /* Use the approximate expression to compute the ordinate to ensure that
   * it's coherent with the output of the calculation.
   * Sometimes when the reduction has some mistakes, the approximation of
   * simplifiedExpression(abscissa) can differ for the approximateOutput.
   */
  // TODO_CONTEXT: prepare for approximation ?
  float ordinate = approximateOutput.approximateToRealScalar<float>(
      angleUnit(), complexFormat(), App::app()->localContext());
  m_model.setParameters(simplifiedExpression, abscissa, ordinate);

  m_layouts[0] =
      Layout::Create(KA ^ KB, {.KA = Layout::String("y="),
                               .KB = Layout(inputClone.createLayout(
                                   displayMode(), numberOfSignificantDigits(),
                                   App::app()->localContext()))});
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
