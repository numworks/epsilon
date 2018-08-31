#include <poincare/conjugate.h>
#include <poincare/conjugate_layout_node.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

ConjugateNode * ConjugateNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<ConjugateNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutRef ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayoutRef(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "conj");
}

Expression ConjugateNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Conjugate(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::conj(c));
}

Expression Conjugate::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.type() == ExpressionNode::Type::Rational) {
    return c;
  }
  return *this;
}

}

