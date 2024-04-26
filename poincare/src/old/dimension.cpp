#include <poincare/old/dimension.h>
#include <poincare/old/layout.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

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
  if (input.otype() == EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Builder(std::complex<T>(input.numberOfChildren()));
  }
  if (input.otype() != EvaluationNode<T>::Type::MatrixComplex ||
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

  if (c.otype() == ExpressionNode::Type::OList) {
    OExpression result = Rational::Builder(c.numberOfChildren());
    replaceWithInPlace(result);
    return result;
  }

  if (c.otype() != ExpressionNode::Type::OMatrix) {
    if (c.deepIsMatrix(reductionContext.context(),
                       reductionContext.shouldCheckMatrices())) {
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  OMatrix result = OMatrix::Builder();
  OMatrix m = static_cast<OMatrix&>(c);
  result.addChildAtIndexInPlace(Rational::Builder(m.numberOfRows()), 0, 0);
  result.addChildAtIndexInPlace(Rational::Builder(m.numberOfColumns()), 1, 1);
  result.setDimensions(1, 2);
  replaceWithInPlace(result);
  return std::move(result);
}

}  // namespace Poincare
