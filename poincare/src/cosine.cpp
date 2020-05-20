#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/sine.h>

#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Cosine::s_functionHelper;

int CosineNode::numberOfChildren() const { return Cosine::s_functionHelper.numberOfChildren(); }

float CosineNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Cosine(this), context, angleUnit);
}

template<typename T>
Complex<T> CosineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::cos(angleInput);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(res, angleInput));
}

Layout CosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Cosine(this), floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.name());
}

int CosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.name());
}

Expression CosineNode::shallowReduce(ReductionContext reductionContext) {
  return Cosine(this).shallowReduce(reductionContext);
}

bool CosineNode::didDerivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return Cosine(this).didDerivate(reductionContext, symbol, symbolValue);
}

Expression CosineNode::unaryFunctionDifferential() {
  return Cosine(this).unaryFunctionDifferential();
}

Expression Cosine::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceDirectFunction(*this, reductionContext);
}

bool Cosine::didDerivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue);
  return true;
}

Expression Cosine::unaryFunctionDifferential() {
  return Multiplication::Builder(Rational::Builder(-1), Sine::Builder(childAtIndex(0).clone()));
}

}
