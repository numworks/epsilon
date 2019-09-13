#include <poincare/matrix_dimension.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixDimension::s_functionHelper;

int MatrixDimensionNode::numberOfChildren() const { return MatrixDimension::s_functionHelper.numberOfChildren(); }

Expression MatrixDimensionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return MatrixDimension(this).shallowReduce();
}

Layout MatrixDimensionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixDimension(this), floatDisplayMode, numberOfSignificantDigits, MatrixDimension::s_functionHelper.name());
}

int MatrixDimensionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixDimension::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixDimensionNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  std::complex<T> operands[2];
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    operands[0] = std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfRows());
    operands[1] = std::complex<T>(static_cast<MatrixComplex<T>&>(input).numberOfColumns());
  } else {
    operands[0] = std::complex<T>(1.0);
    operands[1] = std::complex<T>(1.0);
  }
  return MatrixComplex<T>::Builder(operands, 1, 2);
}


Expression MatrixDimension::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix &>(c);
    Matrix result = Matrix::Builder();
    result.addChildAtIndexInPlace(Rational::Builder(m.numberOfRows()), 0, 0);
    result.addChildAtIndexInPlace(Rational::Builder(m.numberOfColumns()), 1, 1);
    result.setDimensions(1, 2);
    return result;
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    Matrix result = Matrix::Builder();
    result.addChildAtIndexInPlace(Rational::Builder(1), 0, 0);
    result.addChildAtIndexInPlace(Rational::Builder(1), 1, 1);
    result.setDimensions(1, 2);
    return result;
  }
  return *this;
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    Matrix result = Matrix::Builder();
    result.addChildAtIndexInPlace(Rational::Builder(1), 0, 0);
    result.addChildAtIndexInPlace(Rational::Builder(1), 1, 1);
    result.setDimensions(1, 2);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
#endif
}

}
