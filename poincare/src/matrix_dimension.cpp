#include <poincare/matrix_dimension.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixDimension::s_functionHelper;

int MatrixDimensionNode::numberOfChildren() const { return MatrixDimension::s_functionHelper.numberOfChildren(); }

Expression MatrixDimensionNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixDimension(this).shallowReduce(reductionContext.context());
}

Layout MatrixDimensionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixDimension(this), floatDisplayMode, numberOfSignificantDigits, MatrixDimension::s_functionHelper.name());
}

int MatrixDimensionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixDimension::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixDimensionNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
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


Expression MatrixDimension::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.deepIsMatrix(context) && c.type() != ExpressionNode::Type::Matrix) {
    return *this;
  }
  Matrix result = Matrix::Builder();
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix &>(c);
    result.addChildAtIndexInPlace(Rational::Builder(m.numberOfRows()), 0, 0);
    result.addChildAtIndexInPlace(Rational::Builder(m.numberOfColumns()), 1, 1);
  } else {
    result.addChildAtIndexInPlace(Rational::Builder(1), 0, 0);
    result.addChildAtIndexInPlace(Rational::Builder(1), 1, 1);
  }
  result.setDimensions(1, 2);
  replaceWithInPlace(result);
  return std::move(result);
}

}
