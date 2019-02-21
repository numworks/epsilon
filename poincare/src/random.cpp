#include <poincare/random.h>
#include <poincare/complex.h>
#include <ion.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Random::s_functionHelper;

int RandomNode::numberOfChildren() const { return Random::s_functionHelper.numberOfChildren(); }

Expression RandomNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  return Random(this).setSign(s, context, complexFormat, angleUnit);
}

Layout RandomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Random(this), floatDisplayMode, numberOfSignificantDigits, Random::s_functionHelper.name());
}

int RandomNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Random::s_functionHelper.name());
}

template <typename T> Evaluation<T> RandomNode::templateApproximate() const {
  return Complex<T>::Builder(Random::random<T>());
}

Expression Random::setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  assert(s == ExpressionNode::Sign::Positive);
  return *this;
}

template<typename T> T Random::random() {
  if (sizeof(T) == sizeof(float)) {
    uint32_t r = Ion::random();
    return (float)r/(float)(0xFFFFFFFF);
  } else {
    assert(sizeof(T) == sizeof(double));
    uint64_t r;
    uint32_t * rAddress = (uint32_t *)&r;
    *rAddress = Ion::random();
    *(rAddress+1) = Ion::random();
    return (double)r/(double)(0xFFFFFFFFFFFFFFFF);
  }
}

template Evaluation<float> RandomNode::templateApproximate<float>() const;
template Evaluation<double> RandomNode::templateApproximate<double>() const;
template float Random::random();
template double Random::random();

}
