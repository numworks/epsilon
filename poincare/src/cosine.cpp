#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

CosineNode * CosineNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<CosineNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

float CosineNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(this, context, angleUnit);
}

template<typename T>
Complex<T> CosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::cos(angleInput);
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(res));
}

Expression CosineNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Cosine(this).shallowReduce(context, angleUnit);
}

Expression Cosine::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression op = childAtIndex(0);
  if (op.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceDirectFunction(*this, context, angleUnit);
}

}
