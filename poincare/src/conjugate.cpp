#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/opposite.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Conjugate::s_functionHelper;

int ConjugateNode::numberOfChildren() const { return Conjugate::s_functionHelper.numberOfChildren(); }

Expression ConjugateNode::realPart(Context & context, Preferences::AngleUnit angleUnit) const {
  return childAtIndex(0)->realPart(context, angleUnit);
}

Expression ConjugateNode::imaginaryPart(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression e = childAtIndex(0)->imaginaryPart(context, angleUnit);
  if (!e.isUninitialized()) {
    return Opposite(e).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation);
  }
  return Expression();
}

Layout ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.name());
}

Expression ConjugateNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Conjugate(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::conj(c));
}

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
