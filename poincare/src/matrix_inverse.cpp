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

constexpr Expression::FunctionHelper MatrixInverse::s_functionHelper;

int MatrixInverseNode::numberOfChildren() const { return MatrixInverse::s_functionHelper.numberOfChildren(); }

Expression MatrixInverseNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixInverse(this).shallowReduce(reductionContext);
}

Layout MatrixInverseNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixInverse(this), floatDisplayMode, numberOfSignificantDigits, MatrixInverse::s_functionHelper.name());
}

int MatrixInverseNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixInverse::s_functionHelper.name());
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> MatrixInverseNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> inverse;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    inverse = static_cast<MatrixComplex<T>&>(input).inverse();
  } else if (input.type() == EvaluationNode<T>::Type::Complex) {
    inverse = Complex<T>::Builder(std::complex<T>(1)/(static_cast<Complex<T>&>(input).stdComplex()));
  }
  if (inverse.isUninitialized()) {
    inverse = Complex<T>::Undefined();
  }
  return inverse;
}


Expression MatrixInverse::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return Power::Builder(c, Rational::Builder(-1).shallowReduce(reductionContext);
  }
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix mat = static_cast<Matrix&>(c);
    if (mat.numberOfRows() != mat.numberOfColumns()) {
      return Undefined::Builder();
    }
  }
  return *this;
#endif
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    Expression result = Power::Builder(c, Rational::Builder(-1));
    replaceWithInPlace(result);
    result = result.shallowReduce(reductionContext);
    return result;
  }
  return *this;
#endif
}

}
