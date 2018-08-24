#include <poincare/hyperbolic_arc_sine.h>
#include <poincare/complex.h>
#include <cmath>

namespace Poincare {

HyperbolicArcSineNode * HyperbolicArcSineNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<HyperbolicArcSineNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

template<typename T>
Complex<T> HyperbolicArcSineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::asinh(c);
  /* asinh has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]+i+0, +i*inf+0[ (warning: atanh takes the other side of the cut values on
   * ]+i-0, +i*inf+0[) and choose the values on ]-inf*i, -i[ to comply with
   * asinh(-x) = -asinh(x). */
  if (c.real() == 0 && c.imag() < 1) {
    result.real(-result.real()); // other side of the cut
  }
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(result));
}

}
