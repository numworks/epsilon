#include <poincare/matrix_dimension.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <cmath>

namespace Poincare {

MatrixDimensionNode * MatrixDimensionNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<MatrixDimensionNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression MatrixDimensionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return MatrixDimension(this).shallowReduce(context, angleUnit);
}

LayoutReference MatrixDimensionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixDimension(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int MatrixDimensionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Evaluation<T> MatrixDimensionNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  std::complex<T> operands[2];
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    operands[0] = std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfRows());
    operands[1] = std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfColumns());
  } else {
    operands[0] = std::complex<T>(1.0);
    operands[1] = std::complex<T>(1.0);
  }
  return MatrixComplex<T>(operands, 1, 2);
}

Expression MatrixDimension::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix>(c);
    Matrix result;
    result.addChildAtIndexInPlace(Rational(m.numberOfRows()), 0, 0);
    result.addChildAtIndexInPlace(Rational(m.numberOfColumns()), 1, 1);
    result.setDimensions(1, 2);
    return result;
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    Matrix result;
    result.addChildAtIndexInPlace(Rational(1), 0, 0);
    result.addChildAtIndexInPlace(Rational(1), 1, 1);
    result.setDimensions(1, 2);
    return result;
  }
  return *this;
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    Matrix result;
    result.addChildAtIndexInPlace(Rational(1), 0, 0);
    result.addChildAtIndexInPlace(Rational(1), 1, 1);
    result.setDimensions(1, 2);
    return result;
  }
  return *this;
#endif
}

}
