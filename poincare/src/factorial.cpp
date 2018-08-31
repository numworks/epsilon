#include <poincare/factorial.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/symbol.h>
#include <poincare/simplification_helper.h>
#include <poincare/parenthesis.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

FactorialNode * FactorialNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<FactorialNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

// Layout

bool FactorialNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  return static_cast<const ExpressionNode *>(e)->type() == Type::Factorial;
}

// Simplification

Expression FactorialNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Factorial(this).shallowReduce(context, angleUnit);
}

Expression FactorialNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Factorial(this).shallowBeautify(context, angleUnit);
}

template<typename T>
Complex<T> FactorialNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  T n = c.real();
  if (c.imag() != 0 || std::isnan(n) || n != (int)n || n < 0) {
    return Complex<T>::Undefined();
  }
  T result = 1;
  for (int i = 1; i <= (int)n; i++) {
    result *= (T)i;
    if (std::isinf(result)) {
      return Complex<T>(result);
    }
  }
  return Complex<T>(std::round(result));
}

LayoutRef FactorialNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef result;
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  int childrenCount = result.numberOfChildren();
  result.addChildAtIndex(CharLayoutRef('!'), childrenCount, childrenCount, nullptr);
  return result;
}

int FactorialNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (childAtIndex(0)->needsParenthesesWithParent(this)) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (childAtIndex(0)->needsParenthesesWithParent(this)) {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '!';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

Expression Factorial::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational r = static_cast<Rational>(childAtIndex(0));
    if (!r.integerDenominator().isOne() || r.sign() == ExpressionNode::Sign::Negative) {
      return Undefined();
    }
    if (Integer(k_maxOperandValue).isLowerThan(r.unsignedIntegerNumerator())) {
      return *this;
    }
    Rational fact = Rational(Integer::Factorial(r.unsignedIntegerNumerator()));
    assert(!fact.numeratorOrDenominatorIsInfinity()); // because fact < k_maxOperandValue!
    return fact;
  }
  if (childAtIndex(0).type() == ExpressionNode::Type::Symbol) {
    Symbol s = static_cast<Symbol>(childAtIndex(0));
    if (s.name() == Ion::Charset::SmallPi || s.name() == Ion::Charset::Exponential) {
      return Undefined();
    }
  }
  return *this;
}

Expression Factorial::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  // +(a,b)! ->(+(a,b))!
  if (childAtIndex(0).type() == ExpressionNode::Type::Addition
      || childAtIndex(0).type() == ExpressionNode::Type::Multiplication
      || childAtIndex(0).type() == ExpressionNode::Type::Power)
  {
    return Factorial(Parenthesis(childAtIndex(0)));
  }
  return *this;
}

#if 0
int Factorial::simplificationOrderGreaterType(const Expression * e) const {
  if (SimplificationOrder(childAtIndex(0),e) == 0) {
    return 1;
  }
  return SimplificationOrder(childAtIndex(0), e);
}

int Factorial::simplificationOrderSameType(const Expression * e) const {
  return SimplificationOrder(childAtIndex(0), e->childAtIndex(0));
}
#endif

}
