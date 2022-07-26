#include "vector_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include "poincare/expression.h"
#include "poincare/layout_helper.h"
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

  Context * context = App::app()->localContext();
  ExpressionNode::ReductionContext reductionContext(
    context,
    preferences->complexFormat(),
    preferences->angleUnit(),
    GlobalPreferences::sharedGlobalPreferences()->unitFormat(),
    ExpressionNode::ReductionTarget::SystemForApproximation,
    ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);

  assert(m_expression.type() == ExpressionNode::Type::Matrix);
  Matrix &vector = static_cast<Matrix &>(m_expression);
  assert(vector.numberOfColumns() == 1 || vector.numberOfRows() == 1);
  bool isColumn = (vector.numberOfRows() != 1);
  bool is2D = (isColumn ? vector.numberOfRows() : vector.numberOfColumns()) == 2;
  size_t index = 0;
  size_t messageIndex = 0;

  // 1. Vector norm
  Expression norm = VectorNorm::Builder(m_expression).cloneAndReduce(reductionContext);
  m_indexMessageMap[index] = messageIndex++;
  m_exactLayouts[index] = getLayoutFromExpression(norm, context, preferences);
  m_approximatedLayouts[index] = getLayoutFromExpression(norm.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit()), context, preferences);
  index++;

  // We can't determine it if norm is null
  // TODO: Handle ExpressionNode::NullStatus::Unknown
  if (!norm.isUndefined() && norm.nullStatus(context) != ExpressionNode::NullStatus::Null) {
    // 2. Normalized vector
    m_indexMessageMap[index] = messageIndex++;
    Expression normalized = Division::Builder(m_expression, norm).cloneAndReduce(reductionContext);
    assert(normalized.type() == ExpressionNode::Type::Matrix);
    if (isColumn) {
      m_layouts[index++] = HorizontalLayout::Builder(getLayoutFromExpression(MatrixTranspose::Builder(normalized), context, preferences), VerticalOffsetLayout::Builder(CodePointLayout::Builder('T'), VerticalOffsetLayoutNode::Position::Superscript));
    } else {
      m_layouts[index++] = getLayoutFromExpression(normalized, context, preferences);
    }
    if (is2D) {
      // 3. Angle with x-axis
      Expression x = static_cast<Matrix &>(vector).matrixChild(0, 0);
      Expression y = static_cast<Matrix &>(vector).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      float xApproximation = x.approximateToScalar<float>(context, preferences->complexFormat(), preferences->angleUnit());
      float yApproximation = y.approximateToScalar<float>(context, preferences->complexFormat(), preferences->angleUnit());
      m_model.setVectorX(xApproximation);
      m_model.setVectorY(yApproximation);
      illustrationCell()->reloadCell();
      x = static_cast<Matrix &>(normalized).matrixChild(0, 0);
      y = static_cast<Matrix &>(normalized).matrixChild(isColumn ? 1 : 0, isColumn ? 0 : 1);
      setShowIllustration(xApproximation != 0.f || yApproximation != 0.f);
      Expression angle = ArcCosine::Builder(x);
      if (y.sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
        angle = Subtraction::Builder(Poincare::Constant::Builder("π"), angle);
      }
      m_indexMessageMap[index] = messageIndex++;
      m_layouts[index] = LayoutHelper::String("θ");
      m_approximatedLayouts[index] = getLayoutFromExpression(angle, context, preferences);
      index++;
    } else {
      setShowIllustration(false);
    }
  } else {
    setShowIllustration(false);
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
