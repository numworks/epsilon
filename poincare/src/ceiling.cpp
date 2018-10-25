#include <poincare/ceiling.h>
#include <poincare/constant.h>
#include <poincare/ceiling_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper Ceiling::s_functionHelper;

int CeilingNode::numberOfChildren() const { return Ceiling::s_functionHelper.numberOfChildren(); }

Layout CeilingNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return CeilingLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int CeilingNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Ceiling::s_functionHelper.name());
}

template<typename T>
Complex<T> CeilingNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>(std::ceil(c.real()));
}

Expression CeilingNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Ceiling(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression Ceiling::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
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
  if (c.type() == ExpressionNode::Type::Constant) {
    Constant s = static_cast<Constant&>(c);
    Expression result;
    if (s.isPi()) {
      result = Rational(4);
    }
    if (s.isExponential()) {
      result = Rational(3);
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result;
    }
    return *this;
  }
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = c.convert<Rational>();
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isInfinity());
  if (div.remainder.isZero()) {
    Expression result = Rational(div.quotient);
    replaceWithInPlace(result);
    return result;
  }
  Integer result = Integer::Addition(div.quotient, Integer(1));
  if (result.isInfinity()) {
    return *this;
  }
  Expression rationalResult = Rational(result);
  replaceWithInPlace(rationalResult);
  return rationalResult;
}

}
