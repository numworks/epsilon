#include <poincare/ceiling.h>
#include <poincare/ceiling_layout_node.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

CeilingNode * CeilingNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<CeilingNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference CeilingNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return CeilingLayoutRef(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int CeilingNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Complex<T> CeilingNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>(std::ceil(c.real()));
}

Expression CeilingNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Ceiling(this).shallowReduce(context, angleUnit, futureParent);
}

Expression Ceiling::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
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
  if (c.type() == ExpressionNode::Type::Symbol) {
    Symbol s = static_cast<Symbol>(c);
    if (s.name() == Ion::Charset::SmallPi) {
      return Rational(4);
    }
    if (s.name() == Ion::Charset::Exponential) {
      return Rational(3);
    }
  }
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isInfinity());
  if (div.remainder.isZero()) {
    return Rational(div.quotient);
  }
  Integer result = Integer::Addition(div.quotient, Integer(1));
  if (result.isInfinity()) {
    return *this;
  }
  return Rational(result);
}

}
