#include <poincare/dimension.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/list_complex.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper Dimension::s_functionHelper;

int DimensionNode::numberOfChildren() const { return Dimension::s_functionHelper.numberOfChildren(); }

Expression DimensionNode::shallowReduce(ReductionContext reductionContext) {
  return Dimension(this).shallowReduce(reductionContext.context());
}

Layout DimensionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Dimension(this), floatDisplayMode, numberOfSignificantDigits, Dimension::s_functionHelper.name());
}

int DimensionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Dimension::s_functionHelper.name());
}

template<typename T>
Evaluation<T> DimensionNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() == EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Builder(std::complex<T>(input.numberOfChildren()));
  }
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


Expression Dimension::shallowReduce(Context * context) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::List) {
    Expression result = Rational::Builder(c.numberOfChildren());
    replaceWithInPlace(result);
    return result;
  }
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
