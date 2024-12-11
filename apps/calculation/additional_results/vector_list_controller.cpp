#include "vector_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/round.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/sign.h>
#include <poincare/src/expression/projection.h>
#include <poincare/trigonometry.h>
#include <string.h>

#include "../app.h"
#include "vector_helper.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void VectorListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasVector(exactOutput, approximateOutput,
                                          m_calculationPreferences));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context* context = App::app()->localContext();
  Internal::ProjectionContext ctx = {
      .m_complexFormat = complexFormat(),
      .m_angleUnit = angleUnit(),
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = context};
  assert(!Internal::Projection::UpdateComplexFormatWithExpressionInput(
      exactOutput, &ctx));

  setShowIllustration(false);
  size_t index = 0;
  UserExpression exactClone = exactOutput.clone();

  // 1. Vector norm
  UserExpression norm = VectorHelper::BuildVectorNorm(exactClone, context,
                                                      m_calculationPreferences);
  assert(!norm.isUninitialized() && !norm.isUndefined());
  setLineAtIndex(index++, UserExpression(), norm, &ctx);

  // 2. Normalized vector
  SystemExpression approximatedNorm = PoincareHelpers::Approximate<double>(
      norm, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  Sign sign = approximatedNorm.sign();
  assert(!sign.canBeStrictlyNegative());
  if (sign.canBeNull() || approximatedNorm.isPlusOrMinusInfinity()) {
    return;
  }
  UserExpression normalized =
      UserExpression::Create(KDiv(KA, KB), {.KA = exactClone, .KB = norm});
  PoincareHelpers::CloneAndSimplify(
      &normalized, context,
      {.complexFormat = complexFormat(),
       .angleUnit = angleUnit(),
       .target = k_target,
       .symbolicComputation = k_symbolicComputation});
  if (!normalized.isMatrix()) {
    // The reduction might have failed
    return;
  }
  setLineAtIndex(index++, UserExpression(), normalized, &ctx);

  // 3. Angle with x-axis
  assert(approximateOutput.isMatrix());
  Matrix vector = static_cast<const Matrix&>(approximateOutput);
  assert(vector.isVector());
  if (vector.tree()->numberOfChildren() != 2) {
    // Vector is not 2D
    return;
  }
  assert(normalized.tree()->numberOfChildren() == 2);
  UserExpression angle = UserExpression::Create(
      KACos(KA), {.KA = normalized.cloneChildAtIndex(0)});
  /* ComplexSign needs a reduced expression. Using approximation here, but a
   * reduction would also work. */
  SystemExpression yApprox = PoincareHelpers::Approximate<double>(
      normalized.cloneChildAtIndex(1), context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  sign = yApprox.sign();
  // HasVector should be false if any vector's child is complex.
  if (sign.canBeStrictlyNegative() && !sign.canBeStrictlyPositive()) {
    angle = UserExpression::Create(
        KSub(KA, KB),
        {.KA = Trigonometry::Period(ctx.m_angleUnit), .KB = angle});
  }
  float angleApproximation =
      angle.approximateToScalar<float>(angleUnit(), complexFormat(), context);
  if (!std::isfinite(angleApproximation)) {
    return;
  }
  setLineAtIndex(
      index++,
      Poincare::SymbolHelper::BuildSymbol(UCodePointGreekSmallLetterTheta),
      angle, &ctx);

  // 4. Illustration
  float xApproximation = vector.cloneChildAtIndex(0).approximateToScalar<float>(
      angleUnit(), complexFormat(), context);
  float yApproximation = vector.cloneChildAtIndex(1).approximateToScalar<float>(
      angleUnit(), complexFormat(), context);
  if (!std::isfinite(xApproximation) || !std::isfinite(yApproximation) ||
      (OMG::LaxToZero(xApproximation) == 0.f &&
       OMG::LaxToZero(yApproximation) == 0.f)) {
    return;
  }
  m_model.setVector(xApproximation, yApproximation);
  angleApproximation =
      Trigonometry::ConvertAngleToRadian(angleApproximation, angleUnit());
  m_model.setAngle(angleApproximation);
  setShowIllustration(true);
}

I18n::Message VectorListController::messageAtIndex(int index) {
  // Message index is mapped in setExpression because it depends on the Matrix.
  assert(index < k_maxNumberOfOutputRows && index >= 0);
  I18n::Message messages[k_maxNumberOfOutputRows] = {
      I18n::Message::NormVector,
      I18n::Message::UnitVector,
      I18n::Message::AngleWithFirstAxis,
  };
  return messages[index];
}

}  // namespace Calculation
