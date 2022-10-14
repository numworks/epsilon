#include <poincare/arc_cosine.h>
#include <poincare/complex.h>
#include <poincare/arc_secant.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcSecantNode::numberOfChildren() const { return ArcSecant::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> ArcSecantNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return Complex<T>::Undefined();
  }
  return ArcCosineNode::computeOnComplex<T>(std::complex<T>(1) / c, complexFormat, angleUnit);
}

Layout ArcSecantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ArcSecant(this), floatDisplayMode, numberOfSignificantDigits, ArcSecant::s_functionHelper.aliasesList().mainAlias(), context);
}

int ArcSecantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcSecant::s_functionHelper.aliasesList().mainAlias());
}

Expression ArcSecantNode::shallowReduce(const ReductionContext& reductionContext) {
  return ArcSecant(this).shallowReduce(reductionContext);
}

Expression ArcSecant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return Trigonometry::shallowReduceInverseAdvancedFunction(*this, reductionContext);
}

}
