#include <poincare/arc_sine.h>
#include <poincare/complex.h>
#include <poincare/arc_cosecant.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcCosecantNode::numberOfChildren() const { return ArcCosecant::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> ArcCosecantNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return Complex<T>::Undefined();
  }
  return ArcSineNode::computeOnComplex<T>(std::complex<T>(1) / c, complexFormat, angleUnit);
}

Layout ArcCosecantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ArcCosecant(this), floatDisplayMode, numberOfSignificantDigits, ArcCosecant::s_functionHelper.aliasesList().mainAlias(), context);
}

int ArcCosecantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcCosecant::s_functionHelper.aliasesList().mainAlias());
}

Expression ArcCosecantNode::shallowReduce(const ReductionContext& reductionContext) {
  return ArcCosecant(this).shallowReduce(reductionContext);
}

Expression ArcCosecant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return Trigonometry::shallowReduceInverseAdvancedFunction(*this, reductionContext);
}


}
