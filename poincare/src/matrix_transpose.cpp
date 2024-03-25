#include <poincare/division.h>
#include <poincare/layout.h>
#include <poincare/matrix.h>
#include <poincare/matrix_transpose.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>

namespace Poincare {

int MatrixTransposeNode::numberOfChildren() const {
  return MatrixTranspose::s_functionHelper.numberOfChildren();
}

OExpression MatrixTransposeNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MatrixTranspose(this).shallowReduce(reductionContext);
}

size_t MatrixTransposeNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      MatrixTranspose::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> MatrixTransposeNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> transpose;
  if (input.otype() == EvaluationNode<T>::Type::MatrixComplex) {
    transpose = static_cast<MatrixComplex<T>&>(input).transpose();
  } else {
    transpose = input;
  }
  assert(!transpose.isUninitialized());
  return transpose;
}

OExpression MatrixTranspose::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::OMatrix) {
    OExpression result = static_cast<OMatrix&>(c).createTranspose();
    replaceWithInPlace(result);
    return result;
  }
  if (c.deepIsMatrix(reductionContext.context(),
                     reductionContext.shouldCheckMatrices())) {
    return *this;
  }
  replaceWithInPlace(c);
  return c;
}

}  // namespace Poincare
