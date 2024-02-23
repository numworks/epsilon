#include <poincare/dimension.h>
#include <poincare/layout_helper.h>
#include <poincare/list_complex.h>
#include <poincare/matrix.h>
#include <poincare/matrix_complex.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>
#include <utility>

namespace Poincare {

int DimensionNode::numberOfChildren() const {
  return Dimension::s_functionHelper.numberOfChildren();
}

OExpression DimensionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Dimension(this).shallowReduce(reductionContext);
}

size_t DimensionNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Dimension::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> DimensionNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() == EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Builder(std::complex<T>(input.numberOfChildren()));
  }
  if (input.type() != EvaluationNode<T>::Type::MatrixComplex ||
      input.isUndefined()) {
    return Complex<T>::Undefined();
  }
  std::complex<T> operands[] = {
      std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfRows()),
      std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfColumns())};
  return MatrixComplex<T>::Builder(operands, 1, 2);
}

OExpression Dimension::shallowReduce(ReductionContext reductionContext) {
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

  if (c.type() == ExpressionNode::Type::List) {
    OExpression result = Rational::Builder(c.numberOfChildren());
    replaceWithInPlace(result);
    return result;
  }

  if (c.type() != ExpressionNode::Type::Matrix) {
    if (c.deepIsMatrix(reductionContext.context(),
                       reductionContext.shouldCheckMatrices())) {
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  Matrix result = Matrix::Builder();
  Matrix m = static_cast<Matrix&>(c);
  result.addChildAtIndexInPlace(Rational::Builder(m.numberOfRows()), 0, 0);
  result.addChildAtIndexInPlace(Rational::Builder(m.numberOfColumns()), 1, 1);
  result.setDimensions(1, 2);
  replaceWithInPlace(result);
  return std::move(result);
}

}  // namespace Poincare
