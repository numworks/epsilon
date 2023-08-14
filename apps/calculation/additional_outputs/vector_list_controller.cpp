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
    Expression input, Expression exactOutput, Expression approximateOutput) {
  assert(Calculation::HasVectorAdditionalResults(exactOutput));
  static_assert(
      k_maxNumberOfRows >= k_maxNumberOfOutputRows,
      "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context *context = App::app()->localContext();
  Poincare::Preferences preferencesCopy =
      Preferences::ClonePreferencesWithNewComplexFormat(
          Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
              Poincare::Preferences::sharedPreferences->complexFormat(),
              exactOutput, context));

  setShowIllustration(false);
  size_t index = 0;

  // 1. Vector norm
  Expression norm = VectorHelper::BuildVectorNorm(exactOutput, context);
  assert(!norm.isUninitialized() && !norm.isUndefined());
  setLineAtIndex(index++, Expression(), norm, context, &preferencesCopy);

  // 2. Normalized vector
  Expression approximatedNorm =
      PoincareHelpers::Approximate<double>(norm, context);
  if (approximatedNorm.isNull(context) != TrinaryBoolean::False ||
      Expression::IsInfinity(approximatedNorm, context)) {
    return;
  }
  Expression normalized = Division::Builder(exactOutput, norm);
  PoincareHelpers::CloneAndSimplify(&normalized, context, k_target,
                                    k_symbolicComputation);
  if (normalized.type() != ExpressionNode::Type::Matrix) {
    // The reduction might have failed
    return;
  }
  setLineAtIndex(index++, Expression(), normalized, context, &preferencesCopy);

  // 3. Angle with x-axis
  assert(approximateOutput.type() == ExpressionNode::Type::Matrix);
  Matrix vector = approximateOutput.convert<Matrix>();
  assert(vector.isVector());
  if (vector.numberOfChildren() != 2) {
    // Vector is not 2D
    return;
  }
  assert(normalized.numberOfChildren() == 2);
  Expression angle = ArcCosine::Builder(normalized.childAtIndex(0));
  if (normalized.childAtIndex(1).isPositive(context) == TrinaryBoolean::False) {
    angle = Subtraction::Builder(
        Multiplication::Builder(Rational::Builder(2),
                                Poincare::Constant::PiBuilder()),
        angle);
  }
  float angleApproximation =
      PoincareHelpers::ApproximateToScalar<float>(angle, context);
  if (!std::isfinite(angleApproximation)) {
    return;
  }
  setLineAtIndex(index++,
                 Poincare::Symbol::Builder(UCodePointGreekSmallLetterTheta),
                 angle, context, &preferencesCopy);

  // 4. Illustration
  float xApproximation = PoincareHelpers::ApproximateToScalar<float>(
      vector.childAtIndex(0), context);
  float yApproximation = PoincareHelpers::ApproximateToScalar<float>(
      vector.childAtIndex(1), context);
  if (!std::isfinite(xApproximation) || !std::isfinite(yApproximation) ||
      (OMG::LaxToZero(xApproximation) == 0.f &&
       OMG::LaxToZero(yApproximation) == 0.f)) {
    return;
  }
  m_model.setVector(xApproximation, yApproximation);
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
