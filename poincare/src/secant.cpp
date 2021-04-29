#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/secant.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>

#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Secant::s_functionHelper;

int SecantNode::numberOfChildren() const { return Secant::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> SecantNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator = CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit).stdComplex();
  if (denominator == (T)0.0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(std::complex<T>(1) / denominator);
}

Layout SecantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Secant(this), floatDisplayMode, numberOfSignificantDigits, Secant::s_functionHelper.name());
}

int SecantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Secant::s_functionHelper.name());
}

Expression SecantNode::shallowReduce(ReductionContext reductionContext) {
  return Secant(this).shallowReduce(reductionContext);
}

Expression Secant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Cosine c = Cosine::Builder(childAtIndex(0));
  Power p = Power::Builder(c, Rational::Builder(-1));
  replaceWithInPlace(p);
  c.shallowReduce(reductionContext);
  return p.shallowReduce(reductionContext);
}


}
