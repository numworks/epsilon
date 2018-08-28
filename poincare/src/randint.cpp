#include <poincare/randint.h>
#include <poincare/random.h>
#include <poincare/allocation_failure_expression_node.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

RandintNode * RandintNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<RandintNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Randint(this), floatDisplayMode, numberOfSignificantDigits, name());
}

template <typename T> Evaluation<T> RandintNode::templateApproximate(Context & context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  if (std::isnan(a) || std::isnan(b) || a != std::round(a) || b != std::round(b) || a > b) {
    return Complex<T>::Undefined();

  }
  T result = std::floor(Random::random<T>()*(b+1.0-a)+a);
  return Complex<T>(result);
}

}
