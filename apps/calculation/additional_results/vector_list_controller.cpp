#include "vector_list_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/round.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare_expressions.h>
#include <string.h>

#include "../app.h"
#include "vector_helper.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void VectorListController::computeAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  assert(AdditionalResultsType::HasVector(exactOutput, approximateOutput,
                                          m_calculationPreferences));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context *context = App::app()->localContext();
  ComputationContext computationContext(context, complexFormat(), angleUnit());
  computationContext.updateComplexFormat(exactOutput);

  setShowIllustration(false);
  size_t index = 0;
  Expression exactClone = exactOutput.clone();

  // 1. Vector norm
  Expression norm = VectorHelper::BuildVectorNorm(exactClone, context,
                                                  m_calculationPreferences);
  assert(!norm.isUninitialized() && !norm.isUndefined());
  setLineAtIndex(index++, Expression(), norm, computationContext);

  // 2. Normalized vector
  Expression approximatedNorm = PoincareHelpers::Approximate<double>(
      norm, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  if (approximatedNorm.isNull(context) != TrinaryBoolean::False ||
      Expression::IsInfinity(approximatedNorm)) {
    return;
  }
  Expression normalized = Division::Builder(exactClone, norm);
  PoincareHelpers::CloneAndSimplify(
      &normalized, context,
      {.complexFormat = complexFormat(),
       .angleUnit = angleUnit(),
       .target = k_target,
       .symbolicComputation = k_symbolicComputation});
  if (normalized.type() != ExpressionNode::Type::Matrix) {
    // The reduction might have failed
    return;
  }
  setLineAtIndex(index++, Expression(), normalized, computationContext);

  // 3. Angle with x-axis
  assert(approximateOutput.type() == ExpressionNode::Type::Matrix);
  Matrix vector = static_cast<const Matrix &>(approximateOutput);
  assert(vector.isVector());
  if (vector.numberOfChildren() != 2) {
    // Vector is not 2D
    return;
  }
  assert(normalized.numberOfChildren() == 2);
  Expression angle = ArcCosine::Builder(normalized.childAtIndex(0));
  if (normalized.childAtIndex(1).isPositive(context) == TrinaryBoolean::False) {
    angle = Subtraction::Builder(
        Trigonometry::AnglePeriodInAngleUnit(computationContext.angleUnit()),
        angle);
  }
  float angleApproximation = PoincareHelpers::ApproximateToScalar<float>(
      angle, context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  if (!std::isfinite(angleApproximation)) {
    return;
  }
  setLineAtIndex(index++,
                 Poincare::Symbol::Builder(UCodePointGreekSmallLetterTheta),
                 angle, computationContext);

  // 4. Illustration
  float xApproximation = PoincareHelpers::ApproximateToScalar<float>(
      vector.childAtIndex(0), context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
  float yApproximation = PoincareHelpers::ApproximateToScalar<float>(
      vector.childAtIndex(1), context,
      {.complexFormat = complexFormat(), .angleUnit = angleUnit()});
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
