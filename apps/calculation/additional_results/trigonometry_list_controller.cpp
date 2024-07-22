#include "trigonometry_list_controller.h"

#include <apps/shared/expression_display_permissions.h>
#include <poincare/additional_result_helper.h>
#include <poincare/expression.h>
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
  AdditionalResultHelper::TrigonometryAngleHelper(
      input, exactOutput, approximateOutput, m_directTrigonometry,
      m_calculationPreferences, &ctx, exactAngle, &approximatedAngle,
      m_isStrictlyEqual + 0);

  UserExpression radianExpr = Unit::Builder(Preferences::AngleUnit::Radian);
  UserExpression degreeExpr = Unit::Builder(Preferences::AngleUnit::Degree);
  UserExpression exactAngleWithUnit = UserExpression::Create(
      KMult(KA, KB),
      {.KA = exactAngle,
       .KB = angleUnit() == Preferences::AngleUnit::Radian ? radianExpr
                                                           : degreeExpr});

  size_t index = 0;
  m_layouts[index] = Layout::String("θ");

  m_exactLayouts[index] = getExactLayoutFromExpression(
      UserExpression::Create(KUnitConversion(KA, KB),
                             {.KA = exactAngleWithUnit, .KB = radianExpr}),
      &ctx);
  m_approximatedLayouts[index] = getExactLayoutFromExpression(
      UserExpression::Create(KUnitConversion(KA, KB),
                             {.KA = exactAngleWithUnit, .KB = degreeExpr}),
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
  m_isStrictlyEqual[index] =
      UserExpression::ExactAndApproximateExpressionsAreEqual(
          UserExpression::Parse(exactBuffer, context),
          UserExpression::Parse(approximateBuffer, context));
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
