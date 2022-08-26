#include <poincare/sine.h>
#include <poincare/complex.h>
#include <poincare/cosecant.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int CosecantNode::numberOfChildren() const { return Cosecant::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> CosecantNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator = SineNode::computeOnComplex<T>(c, complexFormat, angleUnit).complexAtIndex(0);
  if (denominator == static_cast<T>(0.0)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(std::complex<T>(1) / denominator);
}

Layout CosecantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(Cosecant(this), floatDisplayMode, numberOfSignificantDigits, Cosecant::s_functionHelper.aliasesList().mainAlias(), context);
}

int CosecantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Cosecant::s_functionHelper.aliasesList().mainAlias());
}

Expression CosecantNode::shallowReduce(const ReductionContext& reductionContext) {
  return Cosecant(this).shallowReduce(reductionContext);
}

Expression Cosecant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceAdvancedFunction(*this, reductionContext);
}


}
