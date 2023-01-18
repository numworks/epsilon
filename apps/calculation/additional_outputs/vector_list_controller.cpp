#include "vector_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <apps/global_preferences.h>
#include <poincare_expressions.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <string.h>


using namespace Poincare;
using namespace Shared;

namespace Calculation {

void VectorListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  assert(!m_expression.isUninitialized());
  static_assert(k_maxNumberOfRows >= k_maxNumberOfOutputRows, "k_maxNumberOfRows must be greater than k_maxNumberOfOutputRows");

  Context * context = App::app()->localContext();
  Poincare::Preferences preferencesCopy = Preferences::ClonePreferencesWithNewComplexFormat(Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(Poincare::Preferences::sharedPreferences()->complexFormat(), e, context));

  setShowIllustration(false);

  assert(m_expression.type() == ExpressionNode::Type::Matrix);
  Matrix vector = static_cast<Matrix &>(m_expression);
  assert(vector.numberOfColumns() == 1 || vector.numberOfRows() == 1);
  bool isColumn = (vector.numberOfColumns() == 1);
  bool is2D = (isColumn ? vector.numberOfRows() : vector.numberOfColumns()) == 2;
  size_t index = 0;
  size_t messageIndex = 0;

  constexpr static ReductionTarget k_target = ReductionTarget::SystemForApproximation;
  constexpr static SymbolicComputation k_symbolicComputation = SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
  // 1. Vector norm
  Expression norm = VectorNorm::Builder(m_expression);
  PoincareHelpers::CloneAndReduce(&norm, context, k_target, k_symbolicComputation);
  m_indexMessageMap[index] = messageIndex++;
  Layout exact = getLayoutFromExpression(norm, context, &preferencesCopy);
  Expression approximatedNorm = PoincareHelpers::Approximate<double>(norm, context);
  setLineAtIndex(index++, Expression(), norm, context, &preferencesCopy);

  if (!norm.isUndefined() && approximatedNorm.isNull(context) == TrinaryBoolean::False && !Expression::IsInfinity(approximatedNorm, context)) {
    // 2. Normalized vector
    Expression normalized = Division::Builder(m_expression, norm);
    PoincareHelpers::CloneAndReduce(&normalized, context, k_target, k_symbolicComputation);
    if (normalized.type() != ExpressionNode::Type::Matrix) {
      // The reduction might have failed
      return;
    }
    m_indexMessageMap[index] = messageIndex++;
    setLineAtIndex(index++, Expression(), normalized, context, &preferencesCopy);
    if (is2D) {
      // 3. Angle with x-axis
      Expression x = static_cast<Matrix &>(vector).matrixChild(0, 0);
      Expression y = static_cast<Matrix &>(vector).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      float xApproximation = PoincareHelpers::ApproximateToScalar<float>(x, context);
      float yApproximation = PoincareHelpers::ApproximateToScalar<float>(y, context);
      if (std::isfinite(xApproximation*xApproximation + yApproximation*yApproximation) && (xApproximation != 0.f || yApproximation != 0.f)) {
        m_model.setVector(xApproximation, yApproximation);
        illustrationCell()->reloadCell();
        setShowIllustration(true);
      } else {
        setShowIllustration(false);
      }
      x = static_cast<Matrix &>(normalized).matrixChild(0, 0);
      y = static_cast<Matrix &>(normalized).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      Expression angle = ArcCosine::Builder(x);
      if (y.isPositive(context) == TrinaryBoolean::False) {
        angle = Subtraction::Builder(Multiplication::Builder(Rational::Builder(2), Poincare::Constant::Builder("π")), angle);
      }
      m_indexMessageMap[index] = messageIndex++;
      setLineAtIndex(index++, Poincare::Symbol::Builder(UCodePointGreekSmallLetterTheta), angle, context, &preferencesCopy);
    }
  }
}

I18n::Message VectorListController::messageAtIndex(int index) {
  // Message index is mapped in setExpression because it depends on the Matrix.
  assert(index < k_maxNumberOfOutputRows && index >=0);
  I18n::Message messages[k_maxNumberOfOutputRows] = {
    I18n::Message::NormVector,
    I18n::Message::UnitVector,
    I18n::Message::AngleWithFirstAxis,
  };
  return messages[m_indexMessageMap[index]];
}

}
