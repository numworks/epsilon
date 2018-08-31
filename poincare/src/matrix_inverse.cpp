#include <poincare/matrix_inverse.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

MatrixInverseNode * MatrixInverseNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<MatrixInverseNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression MatrixInverseNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return MatrixInverse(this).shallowReduce(context, angleUnit, futureParent);
}

LayoutReference MatrixInverseNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixInverse(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int MatrixInverseNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> MatrixInverseNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> inverse;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    inverse = static_cast<MatrixComplex<T>&>(input).inverse();
  } else if (input.type() == EvaluationNode<T>::Type::Complex) {
    inverse = Complex<T>(std::complex<T>(1)/(static_cast<Complex<T>&>(input).stdComplex()));
  }
  if (inverse.isUninitialized()) {
    inverse = Complex<T>::Undefined();
  }
  return inverse;
}

Expression MatrixInverse::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return Power(c, Rational(-1).shallowReduce(context, angleUnit);
  }
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix mat = static_cast<Matrix&>(c);
    if (mat.numberOfRows() != mat.numberOfColumns()) {
      return Undefined();
    }
  }
  return *this;
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    return Power(c, Rational(-1)).shallowReduce(context, angleUnit);
  }
  return *this;
#endif
}

}
