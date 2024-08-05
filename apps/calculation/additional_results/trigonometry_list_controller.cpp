#include "trigonometry_list_controller.h"

#include <apps/shared/expression_display_permissions.h>
#include <poincare/additional_results_helper.h>
#include <poincare/expression.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/old/undefined.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void TrigonometryListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert((m_directTrigonometry &&
          AdditionalResultsType::HasDirectTrigo(input, exactOutput,
                                                m_calculationPreferences)) ||
         (!m_directTrigonometry &&
          AdditionalResultsType::HasInverseTrigo(input, exactOutput,
                                                 m_calculationPreferences)));

  Context* context = App::app()->localContext();
  Internal::ProjectionContext ctx = {
      .m_complexFormat = complexFormat(),
      .m_angleUnit = angleUnit(),
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = context};

  UserExpression exactAngle;
  UserExpression approximateAngle;
  float approximatedAngle;
  AdditionalResultsHelper::TrigonometryAngleHelper(
      input, exactOutput, approximateOutput, m_directTrigonometry,
      m_calculationPreferences, &ctx,
      ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation, exactAngle,
      &approximatedAngle, &(m_isStrictlyEqual[0]));

  UserExpression exactAngleWithUnit = UserExpression::Create(
      KMult(KA, KB), {.KA = exactAngle, .KB = Unit::Builder(angleUnit())});

  size_t index = 0;
  m_layouts[index] = Layout::String("θ");

  m_exactLayouts[index] = getExactLayoutFromExpression(
      UserExpression::Create(
          KUnitConversion(KA, KB),
          {.KA = exactAngleWithUnit,
           .KB = Unit::Builder(Preferences::AngleUnit::Radian)}),
      &ctx);
  m_approximatedLayouts[index] = getExactLayoutFromExpression(
      UserExpression::Create(
          KUnitConversion(KA, KB),
          {.KA = exactAngleWithUnit,
           .KB = Unit::Builder(Preferences::AngleUnit::Degree)}),
      &ctx);

  constexpr KTree k_symbol = "θ"_e;
  setLineAtIndex(++index, UserExpression::Create(KCos(k_symbol), {}),
                 UserExpression::Create(KCos(KA), {.KA = exactAngle}), &ctx);
  updateIsStrictlyEqualAtIndex(index, context);
  setLineAtIndex(++index, UserExpression::Create(KSin(k_symbol), {}),
                 UserExpression::Create(KSin(KA), {.KA = exactAngle}), &ctx);
  updateIsStrictlyEqualAtIndex(index, context);
  setLineAtIndex(++index, UserExpression::Create(KTan(k_symbol), {}),
                 UserExpression::Create(KTan(KA), {.KA = exactAngle}), &ctx);
  updateIsStrictlyEqualAtIndex(index, context);

  /* Set illustration */
  m_model.setAngle(approximatedAngle);
  setShowIllustration(true);
}

void TrigonometryListController::updateIsStrictlyEqualAtIndex(
    int index, Context* context) {
  if (m_approximatedLayouts[index].isUninitialized()) {
    /* Only one layout is displayed, so there is no equal sign. */
    return;
  }
  char approximateBuffer[::Constant::MaxSerializedExpressionSize];
  m_approximatedLayouts[index].serializeForParsing(
      approximateBuffer, ::Constant::MaxSerializedExpressionSize);
  if (strcmp(approximateBuffer, Undefined::Name()) == 0) {
    // Hide exact result if approximation is undef (e.g tan(1.5707963267949))
    m_exactLayouts[index] = Layout();
    return;
  }
  char exactBuffer[::Constant::MaxSerializedExpressionSize];
  m_exactLayouts[index].serializeForParsing(
      exactBuffer, ::Constant::MaxSerializedExpressionSize);
  assert(strcmp(exactBuffer, approximateBuffer) != 0);
  Internal::ProjectionContext ctx;  // TODO: pass parameters to context
  m_isStrictlyEqual[index] =
      Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
          UserExpression::Parse(exactBuffer, context),
          UserExpression::Parse(approximateBuffer, context), &ctx);
}

void TrigonometryListController::fillCellForRow(Escher::HighlightCell* cell,
                                                int row) {
  if (typeAtRow(row) == k_expressionCellType) {
    int expressionIndex = row - 1;
    assert(0 <= expressionIndex && expressionIndex < k_numberOfExpressionRows);
    static_cast<AdditionalResultCell*>(cell)
        ->label()
        ->setExactAndApproximateAreStriclyEqual(
            m_isStrictlyEqual[expressionIndex]);
  }
  return IllustratedExpressionsListController::fillCellForRow(cell, row);
}

I18n::Message TrigonometryListController::messageAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::AngleInZeroTwoPi;
  }
  return I18n::Message::Default;
}

}  // namespace Calculation
