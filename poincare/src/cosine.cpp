#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/sine.h>
#include <poincare/simplification_helper.h>

#include <cmath>

namespace Poincare {

int CosineNode::numberOfChildren() const { return Cosine::s_functionHelper.numberOfChildren(); }

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

bool CosineNode::derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  return Cosine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression CosineNode::unaryFunctionDifferential(ReductionContext reductionContext) {
  return Cosine(this).unaryFunctionDifferential(reductionContext);
}

Expression Cosine::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceDirectFunction(*this, reductionContext);
}


bool Cosine::derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression Cosine::unaryFunctionDifferential(ExpressionNode::ReductionContext reductionContext) {
  return Multiplication::Builder(Rational::Builder(-1), Trigonometry::UnitConversionFactor(reductionContext.angleUnit(), Preferences::AngleUnit::Radian), Sine::Builder(childAtIndex(0).clone()));
}

}
