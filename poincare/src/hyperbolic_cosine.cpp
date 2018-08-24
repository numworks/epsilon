#include <poincare/hyperbolic_cosine.h>

namespace Poincare {

HyperbolicCosineNode * HyperbolicCosineNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<HyperbolicCosineNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

template<typename T>
Complex<T> HyperbolicCosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::cosh(c)));
}

}
