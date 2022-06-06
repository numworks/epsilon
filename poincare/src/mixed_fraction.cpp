#include <poincare/mixed_fraction.h>
#include <poincare/complex.h>

namespace Poincare {

Expression MixedFractionNode::shallowReduce(ReductionContext reductionContext) {
  return MixedFraction(this).shallowReduce(reductionContext);
}

Layout MixedFractionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return Layout();
}

int MixedFractionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  buffer[0] = '1';
  buffer[1] = 0;
  return 1;
}

template <typename T> Evaluation<T> MixedFractionNode::templateApproximate() const {
  return Complex<T>::Builder(1.0);
}

Expression MixedFraction::shallowReduce(ExpressionNode::ReductionContext context) {
  return *this;
}

}
