#include <poincare/complex.h>
#include <poincare/cosine.h>
#include <poincare/derivative.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>

#include <cmath>

namespace Poincare {

int SineNode::numberOfChildren() const {
  return Sine::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> SineNode::computeOnComplex(const std::complex<T> c,
                                           Preferences::ComplexFormat,
                                           Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::sin(angleInput);
  return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(
      res, angleInput);
}

size_t SineNode::serialize(char* buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Sine::s_functionHelper.aliasesList().mainAlias());
}

OExpression SineNode::shallowReduce(const ReductionContext& reductionContext) {
  Sine e = Sine(this);
  return Trigonometry::ShallowReduceDirectFunction(e, reductionContext);
}

bool SineNode::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  return Sine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression SineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Sine(this).unaryFunctionDifferential(reductionContext);
}

bool Sine::derivate(const ReductionContext& reductionContext, Symbol symbol,
                    OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression Sine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Multiplication::Builder(
      Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                         Preferences::AngleUnit::Radian),
      Cosine::Builder(childAtIndex(0).clone()));
}

}  // namespace Poincare
