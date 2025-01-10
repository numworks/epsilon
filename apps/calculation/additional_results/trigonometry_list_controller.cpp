#include "trigonometry_list_controller.h"

#include <poincare/additional_results_helper.h>
#include <poincare/cas.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void TrigonometryListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  Context* context = App::app()->localContext();
  assert((m_directTrigonometry &&
          AdditionalResultsType::HasDirectTrigo(
              input, exactOutput, m_calculationPreferences, context)) ||
         (!m_directTrigonometry &&
          AdditionalResultsType::HasInverseTrigo(
              input, exactOutput, m_calculationPreferences, context)));

  Internal::ProjectionContext ctx = {
      .m_complexFormat = complexFormat(),
      .m_angleUnit = angleUnit(),
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = context};

  UserExpression exactAngle;
  UserExpression approximateAngle;
  float approximatedAngle;
  AdditionalResultsHelper::TrigonometryAngleHelper(
      input, exactOutput, approximateOutput, m_directTrigonometry,
      m_calculationPreferences, &ctx, CAS::ShouldOnlyDisplayApproximation,
      exactAngle, &approximatedAngle, &(m_isStrictlyEqual[0]));

  UserExpression exactAngleWithUnit = UserExpression::Create(
      KMult(KA, KB), {.KA = exactAngle, .KB = Unit::Builder(angleUnit())});

  size_t index = 0;
  m_layouts[index] = Layout::String("θ");

  m_exactLayouts[index] = GetExactLayoutFromExpression(
      UserExpression::Create(
          KUnitConversion(KA, KB),
          {.KA = exactAngleWithUnit,
           .KB = Unit::Builder(Preferences::AngleUnit::Radian)}),
      &ctx);
  m_approximatedLayouts[index] = GetExactLayoutFromExpression(
      UserExpression::Create(
          KUnitConversion(KA, KB),
          {.KA = exactAngleWithUnit,
           .KB = Unit::Builder(Preferences::AngleUnit::Degree)}),
      &ctx);

  constexpr KTree k_symbol = "θ"_e;
  setLineAtIndex(++index, UserExpression::Create(KCos(k_symbol), {}),
                 UserExpression::Create(KCos(KA), {.KA = exactAngle}), &ctx);
  setLineAtIndex(++index, UserExpression::Create(KSin(k_symbol), {}),
                 UserExpression::Create(KSin(KA), {.KA = exactAngle}), &ctx);
  setLineAtIndex(++index, UserExpression::Create(KTan(k_symbol), {}),
                 UserExpression::Create(KTan(KA), {.KA = exactAngle}), &ctx);

  /* Set illustration */
  m_model.setAngle(approximatedAngle);
  setShowIllustration(true);
}

I18n::Message TrigonometryListController::messageAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::AngleInZeroTwoPi;
  }
  return I18n::Message::Default;
}

}  // namespace Calculation
