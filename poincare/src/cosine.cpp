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

Layout CosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(Cosine(this), floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.aliasesList().mainAlias(), context);
}

int CosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.aliasesList().mainAlias());
}

Expression CosineNode::shallowReduce(const ReductionContext& reductionContext) {
  Cosine e = Cosine(this);
  return Trigonometry::shallowReduceDirectFunction(e, reductionContext);
}

bool CosineNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Cosine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression CosineNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Cosine(this).unaryFunctionDifferential(reductionContext);
}

bool Cosine::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression Cosine::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Multiplication::Builder(Rational::Builder(-1), Trigonometry::UnitConversionFactor(reductionContext.angleUnit(), Preferences::AngleUnit::Radian), Sine::Builder(childAtIndex(0).clone()));
}

}
