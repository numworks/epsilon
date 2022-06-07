#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/secant.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int SecantNode::numberOfChildren() const { return Secant::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> SecantNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator = CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit).complexAtIndex(0);
  if (denominator == static_cast<T>(0.0)) {
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
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceAdvancedFunction(*this, reductionContext);
}


}
