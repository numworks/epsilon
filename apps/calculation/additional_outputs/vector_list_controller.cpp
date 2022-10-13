#include "vector_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include "poincare/expression.h"
#include "poincare/layout_helper.h"
#include "poincare/multiplication.h"
#include "poincare/rational.h"
#include <apps/global_preferences.h>
#include <poincare/arc_cosine.h>
#include <poincare/constant.h>
#include <poincare/determinant.h>
#include <poincare/subtraction.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/matrix_transpose.h>
#include <poincare/vector_norm.h>
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

  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Poincare::Preferences::ComplexFormat currentComplexFormat = preferences->complexFormat();
  if (currentComplexFormat == Poincare::Preferences::ComplexFormat::Real) {
    /* Temporary change complex format to avoid all additional expressions to be
     * "nonreal" (with [i] for instance). As additional results are computed from
     * the output, which is built taking ComplexFormat into account, there are
     * no risks of displaying additional results on an nonreal output. */
    preferences->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  }

  setShowIllustration(false);

  assert(m_expression.type() == ExpressionNode::Type::Matrix);
  Matrix vector = static_cast<Matrix &>(m_expression);
  assert(vector.numberOfColumns() == 1 || vector.numberOfRows() == 1);
  bool isColumn = (vector.numberOfColumns() == 1);
  bool is2D = (isColumn ? vector.numberOfRows() : vector.numberOfColumns()) == 2;
  size_t index = 0;
  size_t messageIndex = 0;

  Context * context = App::app()->localContext();
  constexpr static ExpressionNode::ReductionTarget k_target = ExpressionNode::ReductionTarget::SystemForApproximation;
  constexpr static ExpressionNode::SymbolicComputation k_symbolicComputation = ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
  // 1. Vector norm
  Expression norm = VectorNorm::Builder(m_expression);
  PoincareHelpers::CloneAndReduce(&norm, context, k_target, k_symbolicComputation);
  m_indexMessageMap[index] = messageIndex++;
  Layout exact = getLayoutFromExpression(norm, context, preferences);
  Expression approximatedNorm = PoincareHelpers::Approximate<double>(norm, context);
  Layout approximated =  getLayoutFromExpression(approximatedNorm, context, preferences);
  m_exactLayouts[index] = approximated.isIdenticalTo(exact) ? Layout() : exact;
  m_approximatedLayouts[index] = approximated;
  index++;

  if (!norm.isUndefined() && approximatedNorm.isNull(context) == TrinaryBoolean::False) {
    // 2. Normalized vector
    m_indexMessageMap[index] = messageIndex++;
    Expression normalized = Division::Builder(m_expression, norm);
    PoincareHelpers::CloneAndReduce(&normalized, context, k_target, k_symbolicComputation);
    assert(normalized.type() == ExpressionNode::Type::Matrix);
    m_layouts[index++] = getLayoutFromExpression(normalized, context, preferences);
    if (is2D) {
      // 3. Angle with x-axis
      Expression x = static_cast<Matrix &>(vector).matrixChild(0, 0);
      Expression y = static_cast<Matrix &>(vector).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      float xApproximation = PoincareHelpers::ApproximateToScalar<float>(x, context);
      float yApproximation = PoincareHelpers::ApproximateToScalar<float>(y, context);
      m_model.setVector(xApproximation, yApproximation);
      illustrationCell()->reloadCell();
      x = static_cast<Matrix &>(normalized).matrixChild(0, 0);
      y = static_cast<Matrix &>(normalized).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      setShowIllustration(xApproximation != 0.f || yApproximation != 0.f);
      Expression angle = ArcCosine::Builder(x);
      if (y.isPositive(context) == TrinaryBoolean::False) {
        angle = Subtraction::Builder(Multiplication::Builder(Rational::Builder(2), Poincare::Constant::Builder("π")), angle);
      }
      m_indexMessageMap[index] = messageIndex++;
      m_layouts[index] = LayoutHelper::String("θ");
      m_approximatedLayouts[index] = getLayoutFromExpression(angle, context, preferences);
      index++;
    }
  }

  // Reset complex format as before
  preferences->setComplexFormat(currentComplexFormat);
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
