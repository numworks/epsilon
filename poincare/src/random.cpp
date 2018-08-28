#include <poincare/random.h>
#include <poincare/allocation_failure_expression_node.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

RandomNode * RandomNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<RandomNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression RandomNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Random(this).setSign(s, context, angleUnit);
}

LayoutReference RandomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Random(this), floatDisplayMode, numberOfSignificantDigits, name());
}

template <typename T> Evaluation<T> RandomNode::templateApproximate() const {
  return Complex<T>(Random::random<T>());
}

Expression Random::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
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

template float Random::random();
template double Random::random();

}
