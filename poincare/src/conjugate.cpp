#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Layout ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "conj");
}

Expression ConjugateNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Conjugate(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::conj(c));
}

Conjugate::Conjugate() : Expression(TreePool::sharedPool()->createTreeNode<ConjugateNode>()) {}

Expression Conjugate::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.type() == ExpressionNode::Type::Rational) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

}
