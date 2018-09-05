#include <poincare/floor.h>
#include <poincare/floor_layout_node.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

FloorNode * FloorNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<FloorNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutRef FloorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return FloorLayoutRef(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int FloorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Complex<T> FloorNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>(std::floor(c.real()));
}

Expression FloorNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Floor(this).shallowReduce(context, angleUnit);
}

Expression Floor::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefinedOrAllocationFailure()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.type() == ExpressionNode::Type::Symbol) {
    Symbol s = static_cast<Symbol &>(c);
    Expression result;
    if (s.name() == Ion::Charset::SmallPi) {
      result = Rational(3);
    }
    if (s.name() == Ion::Charset::Exponential) {
      result = Rational(2);
    }
    replaceWithInPlace(result);
    return result;
  }
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational &>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.quotient.isInfinity());
  Expression result = Rational(div.quotient);
  replaceWithInPlace(result);
  return result;
}

}
