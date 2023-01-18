#include <poincare/tangent.h>
#include <poincare/cosine.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/trigonometry.h>
#include <cmath>

namespace Poincare {

int TangentNode::numberOfChildren() const { return Tangent::s_functionHelper.numberOfChildren(); }

Layout TangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(Tangent(this), floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.aliasesList().mainAlias(), context);
}

int TangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Complex<T> TangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::tan(angleInput);
  /* tan should be undefined at (2n+1)*pi/2 for any integer n.
   * std::tan is not reliable at these values because it is diverging and any
   * approximation errors on pi could easily yield a finite result.
   * At these values, cos yields 0, but is also greatly affected by
   * approximation error and could yield a non-null value : cos(pi/2+e) ~= -e
   * On the other hand, sin, which should yield either 1 or -1 around these
   * values is much more resilient : sin(pi/2+e) ~= 1 - (e^2)/2.
   * We therefore use sin to identify values at which tan should be undefined.
   */
  std::complex<T> sin = std::sin(angleInput);
  if (sin == std::complex<T>(1) || sin == std::complex<T>(-1)) {
    res = std::complex<T>(NAN, NAN);
  }
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(res, angleInput));
}

Expression TangentNode::shallowReduce(const ReductionContext& reductionContext) {
  return Tangent(this).shallowReduce(reductionContext);
}

bool TangentNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Tangent(this).derivate(reductionContext, symbol, symbolValue);
}

Expression TangentNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Tangent(this).unaryFunctionDifferential(reductionContext);
}

Expression Tangent::shallowReduce(ReductionContext reductionContext) {
  Expression newExpression = Trigonometry::shallowReduceDirectFunction(*this, reductionContext);
  if (newExpression.type() == ExpressionNode::Type::Tangent) {
    Sine s = Sine::Builder(newExpression.childAtIndex(0).clone());
    Cosine c = Cosine::Builder(newExpression.childAtIndex(0));
    Division d = Division::Builder(s, c);
    s.shallowReduce(reductionContext);
    c.shallowReduce(reductionContext);
    newExpression.replaceWithInPlace(d);
    return d.shallowReduce(reductionContext);
  }
  return newExpression;
}

bool Tangent::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression Tangent::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Multiplication::Builder(Trigonometry::UnitConversionFactor(reductionContext.angleUnit(), Preferences::AngleUnit::Radian), Power::Builder(Cosine::Builder(childAtIndex(0).clone()), Rational::Builder(-2)));
}

}
