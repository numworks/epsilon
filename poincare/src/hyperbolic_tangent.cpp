#include <poincare/hyperbolic_tangent.h>

namespace Poincare {

HyperbolicTangentNode * HyperbolicTangentNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<HyperbolicTangentNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

template<typename T>
Complex<T> HyperbolicTangentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::tanh(c)));
}

template Complex<float> Poincare::HyperbolicTangentNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicTangentNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

}
